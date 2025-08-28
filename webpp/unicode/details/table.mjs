import * as assert from "node:assert";
import {
    findModifiedSubsetRange,
    genIndexAddenda, ModifiedSpan, Modifier, rangeLength,
} from "./modifiers.mjs";
import {
    alignmentOf,
    commentify,
    cppValueOf, findBestTypeFrom,
    overlapInserts,
    realSizeOf,
    renderTableValues,
    Span,
    splitInto,
    TableTraits,
    uint32,
    uint8, updateProgressBar,
} from "./utils.mjs";

const verbose = process.argv.includes("--verbose");

export class TablePairs {
    #indexAddenda;
    #name = "";
    #description = "";
    data = []; // raw, unprocessed data
    #props = {};
    #indicesTables = []; // it's distilled from this.indices
    #breakpointsTableSize = 0n;

    init(meta) {
        this.#props = meta;
        this.#name = this.#props?.name || "table";
        this.#description = this.#props?.description || "";

        // index table's information
        this.#indexAddenda = (meta?.genIndexAddenda || genIndexAddenda)();
        this.#indexAddenda.name = `${this.#name}_index`;
        this.#indexAddenda.description = `${this.#name[0].toUpperCase()}${this.#name.substring(1)} (Index Table)\n${this.#description}`;

        // the table that points to the values or blocks table
        this.indices = new TableTraits(this.#props?.indices?.max || 435300, this.#props?.indices?.sizeof || uint32);
        this.indices.tableName = this.#props.indices?.tableName || `${this.#name.toLowerCase()}_indices`;

        // the table that contains the results
        if (this.#props?.values !== null) {
            this.values = new TableTraits(this.#props?.values?.max || 655350, this.#props?.values?.sizeof || uint8);
            this.values.tableName = this.#props.values?.tableName || `${this.#name.toLowerCase()}_values`;
        } else {
            this.values = null;
        }
    }

    add(codePoint, value) {
        // fill the data
        this.data[Number(codePoint)] = value;
    }

    #optimizeInserts(inserts, dataView, modifier) {
        if (this.values === null) {
            return {
                valid: true, pos: 0n, inserts: [],
            };
        }
        let pos = modifier.pos;

        if (inserts.length === 0) {
            return {
                valid: true, pos: BigInt(pos), inserts,
            };
        }

        const insertsModifier = modifier.clone();
        insertsModifier.resetOnly(["pos" /* 'max_length' */]);
        const modifiedInserts = new ModifiedSpan(inserts, insertsModifier);

        // validating inserts:
        if (!this.#indexAddenda.verifyInserts({inserts, dataView, modifier})) {
            return {
                valid: false, ...modifier, data: modifiedInserts.split(),
            };
        }

        const overlapped = overlapInserts(this.values, modifiedInserts);
        if (overlapped !== 0) {
            pos = BigInt(this.values.length - overlapped);
            inserts = inserts.slice(overlapped, inserts.length);
        }

        if (inserts.length !== 0) {
            let {valid, start, end} = this.#indexAddenda.optimizeInserts({
                inserts, dataView, modifier,
            });
            if (valid === false) {
                return {valid: false};
            }

            if (this.values.length === 0 && end === 0) {
                end = 1;
            }
            if (start !== 0 && end !== inserts.length) {
                inserts = inserts.slice(start, end);
            }
        }

        return {
            valid: true, pos: BigInt(pos), inserts, overlapped,
        };
    }

    get chunkSize() {
        return this.#indexAddenda.chunkSize;
    }

    get chunkMask() {
        return this.#indexAddenda.chunkMask;
    }

    get chunkShift() {
        return this.#indexAddenda.chunkShift;
    }

    rangeLengthStarting(codePointStart) {
        return rangeLength(codePointStart, this.data.length, this.chunkSize);
    }

    dataView(codePointStart, length = this.rangeLengthStarting(codePointStart),) {
        if (this.#props?.dataView) {
            return this.#props.dataView.call(this, codePointStart, length);
        }
        return new Span(this.data, codePointStart, length);
    }

    insertsDataView(codePointStart, length = this.rangeLengthStarting(codePointStart),) {
        if (this.#props?.insertsDataView) {
            return this.#props.insertsDataView.call(this, codePointStart, length,);
        }
        return this.dataView(codePointStart, length);
    }


    #findSimilarMaskedRange(codePointStart) {
        const length = this.rangeLengthStarting(codePointStart);
        let possibilities = [];
        let invalidModifiers = [];
        const dataView = this.dataView(codePointStart);
        const insertsDataView = this.insertsDataView(codePointStart);
        const additionalAddendumValues = this.#props?.getModifierAddenda?.call(this, {
            codePointStart, length, data: this.data, dataView,
        }) || {};

        if (dataView?.getAll?.()?.includes?.(undefined)) {
            console.error(dataView.getAll());
            throw new Error(`Undefined found in data view: ${dataView.getAll()}`);
        }

        for (const indexModifier of this.#indexAddenda.generate({
            dataView, length,
        })) {
            // set the position
            if (!this.#indexAddenda.has("pos")) {
                indexModifier.set({...additionalAddendumValues});
            } else {
                indexModifier.set({
                    pos: BigInt(this.values?.index || 0), ...additionalAddendumValues,
                });
            }

            let lastInfoLength = 0;
            let info = {};

            // try {
            const startPos = findModifiedSubsetRange(dataView, this.values, indexModifier);
            if (startPos === null) {
                info = this.#optimizeInserts(insertsDataView, dataView, indexModifier,);
            } else {
                info = {
                    valid: true, pos: startPos, inserts: new Span(),
                };
            }
            if (!info.valid) {
                invalidModifiers.push({
                    ...info,
                });
            } else {
                if (this.#indexAddenda.has("pos")) {
                    indexModifier.set({
                        pos: info.pos,
                    });
                }

                // assert.ok(Number.isSafeInteger(indexModifier.pos), "Position should not be null");
                if ("mask" in indexModifier && BigInt(indexModifier.pos) !== 0n && BigInt(indexModifier.mask) === 0n) {
                    debugger;
                    throw new Error(`Invalid calculations. If mask is zero, the position must come out zero too; pos: ${indexModifier.pos}, mask: ${indexModifier.mask}`,);
                }
                possibilities.push({
                    ...info, modifier: indexModifier.clone(),
                });

                // performance trick
                lastInfoLength = info.inserts.length;
                if (lastInfoLength === 0) {
                    break;
                }
            }

            /// check if we can have "shift"s.
            if (indexModifier.unshiftAll) {
                // now, try the shifted inserts as well see if they're any good:
                info = this.#optimizeInserts(indexModifier.unshiftAll(insertsDataView), dataView, indexModifier,);
                if (!info.valid) {
                    invalidModifiers.push({
                        ...info,
                    });
                } else {
                    indexModifier.set({
                        pos: info.pos,
                    });
                    if (info.inserts.length < lastInfoLength) {
                        assert.ok(Number.isSafeInteger(indexModifier.pos), "Position should not be null",);
                        if (BigInt(indexModifier.pos) !== 0n && BigInt(indexModifier.mask) === 0n) {
                            debugger;
                            throw new Error("Invalid calculations. If mask is zero, the position must come out zero too.",);
                        }
                        possibilities.push({
                            ...info, modifier: indexModifier.clone(), shifted: indexModifier.shift,
                        });

                        if (info.inserts.length === 0) {
                            break;
                        }
                    }
                }
            }
        }

        const leastInsertLength = possibilities.reduce((acc, curr) => acc >= Number(curr.inserts.length) ? acc : Number(curr.inserts.length), Number(this.#indexAddenda.chunkSize),);
        possibilities = possibilities.filter((item) => {
            return (item !== undefined && item.inserts.length <= leastInsertLength);
        });

        // sort them
        if (this.#props?.toSortedPossibilities) {
            possibilities = this.#props.toSortedPossibilities(possibilities);
        } else if (this.#indexAddenda.has("mask")) {
            possibilities = possibilities.toSorted((a, b) => Number(a.modifier.mask) - Number(b.modifier.mask),);
        }

        const codePointStartHex = codePointStart.toString(16);
        const codePointEndHex = (codePointStart + length).toString(16) || "infinite";
        if (verbose) {
            console.log(`  0x${codePointStartHex}-0x${codePointEndHex}`, "invalid-modifiers:", invalidModifiers.length, "Possibilities:", possibilities.length, possibilities.slice(0, 5).map((item) => ({
                ...item, ...item.modifier.necessaries(), inserts: item.inserts.length,
            })),);
        }
        if (possibilities.length === 0) {
            console.error(`  Empty possibilities:`, possibilities, this.values?.length || 0, this.data.length,);
            console.error(`  Invalid Modifiers:`, invalidModifiers.length, invalidModifiers);
            debugger;
            process.exit(1);
        }
        return possibilities.at(0);
    }

    splitTables() {
        const indicesSplitCount = this.#props?.indices?.splitInto ?? 1;

        this.#indicesTables = splitInto(this.indices.result, indicesSplitCount, (val) => val, this.#props?.indices?.splittingSingleValue ?? true);
        // todo: distil the values table as well
    }

    /// Post-Processing
    process() {
        console.time("Process");
        console.log("Processing...");
        console.log("Chunk Size:", this.chunkSize);
        console.log("Chunk Mask:", this.chunkMask);
        console.log("Chunk Shift:", this.chunkShift);

        this.#props?.tests?.();

        let batchNo = 0n;
        let insertedCount = 0n;
        let reusedCount = 0n;
        // let reusedMaskedCount = 0;
        let saves = 0;
        let uniqueModifiers = new Set();
        for (let range = 0n; range < this.data.length; range += this.#indexAddenda.chunkSize) {
            const codeRange = range >> this.#indexAddenda.chunkShift;
            const length = Math.min(this.data.length - Number(range), Number(this.#indexAddenda.chunkSize),);
            const valueStart = this.values?.index || 0;

            if (verbose) {
                console.log(`Batch: #${batchNo++}`, "CodePoint:", codeRange.toString(16), "Values-Table-Length:", this.values?.length || 0, "range:", range, "length:", length, `Progress: ${Math.floor((Number(range) / this.data.length) * 100)}%`,);
            } else {
                updateProgressBar(Number(range) / this.data.length * 100);
            }

            let {modifier, inserts, rtrimmed, overlapped} = this.#findSimilarMaskedRange(range);
            // assert.ok(Number.isSafeInteger(modifier.pos), "Position should not be null");

            const modifiedValues = this.#props?.modify?.({
                start: range, length, end: range + BigInt(length),
                codeRange, modifier, inserts, rtrimmed, overlapped,
                values: this.data.slice(Number(range), Number(range) + length)
            });
            modifier = modifiedValues?.modifier ?? modifier;
            inserts = modifiedValues?.inserts ?? inserts;
            rtrimmed = modifiedValues?.rtrimmed ?? rtrimmed;
            overlapped = modifiedValues?.overlapped ?? overlapped;

            assert.ok(modifier instanceof Modifier, "The modifier should be an instance of Modifier.",);
            // assert.ok(Array.isArray(inserts), "Inserts should be an array.");

            const code = modifier.modifier;

            this.indices.append(code);
            if (inserts.length > 0) {
                if (this.values !== null) {
                    if (this.#props?.validateResults) {
                        for (const val of inserts) {
                            if (val === undefined || isNaN(val) || val === null) {
                                debugger;
                                console.error(val, inserts, inserts.length, inserts.getAll());
                                console.error(range, code, modifiedValues, modifier, rtrimmed, overlapped);
                                throw new Error(`Invalid value found: ${val}, ${JSON.stringify(inserts)}`);
                            }
                        }
                    }
                    this.values.appendList(inserts);
                    ++insertedCount;
                    saves += length - inserts.length;
                }
            } else {
                ++reusedCount;
                saves += length;
            }
            if (verbose) {
                console.log(`  Code Range (${inserts.length ? "Inserted-" + inserts.length : "Reused"}):`, codeRange, "rtrimmed:", rtrimmed, "overlapped:", overlapped, "last-pos", valueStart, "modifier.pos:", modifier.pos, modifier.necessaries(), "samples:", inserts.filter((item) => item).slice(0, 5));
            }
            uniqueModifiers.add(modifier.categorizableModifier);

            // if (mask !== modifier.resetMask && mask !== modifier.minMask) {
            //     ++reusedMaskedCount;
            // }

            /// verify range
            if (this.#props?.validateResults) {
                const dataView = this.dataView(range, length);
                if (null === findModifiedSubsetRange(dataView, this.values, modifier)) {
                    debugger;
                    findModifiedSubsetRange(dataView, this.values, modifier);
                    // throw new Error(`Bad insert: ${range}-${length}, ${JSON.stringify(dataView)} ${this.data.length} ${JSON.stringify(this.data)}`);
                    throw new Error(`Bad insert: ${range}-${length}, ${JSON.stringify(dataView.getAll())} ${this.data.length}`);
                }
                // for (let ith = 0; ith !== length; ++ith) {
                //     const expected = dataView.at(ith);
                //     let found = modifiedValues.at(ith);
                //     if (expected !== found) {
                //         debugger;
                //         throw new Error(`Validation error:\n` +
                //             `value start: ${valueStart},\n` +
                //             `batch:       ${codeRange},\n` +
                //             `range:       ${range},\n` +
                //             `pos:         ${ith},\n` +
                //             `expected:    ${expected},\n` +
                //             `found:       ${found},\n` +
                //             `modifier:    ${code} ${JSON.stringify(modifier)},\n` +
                //             `inserts:     #${inserts.length} ${JSON.stringify([...inserts])},\n` +
                //             `data:        ${range}-${range + length} #${dataView.length} ${JSON.stringify([...dataView])},\n` +
                //             `values:      #${modifiedValues.length} ${JSON.stringify([...modifiedValues])},\n` +
                //             `values data: #${modifiedValues.data.length} ${JSON.stringify([...modifiedValues.data])}\n` +
                //             `all values:  #${this.values.length} ${JSON.stringify([...this.values.result])}`
                //         );
                //     }
                // }
            }
        }
        if (!verbose) {
            updateProgressBar(100, "----------- Done Finding Ranges -------------");
        }

        if (this.#indexAddenda.has("pos")) {
            const maxPossibleLength = (0b1n << BigInt(this.#indexAddenda.pos.size)) - 1n;
            if (BigInt(this.indices.length) > maxPossibleLength) {
                debugger;
                throw new Error("Table size limit reached; the limit is because " + `the pointer to the table is going to be bigger than ${this.#indexAddenda.pos.typeString} size; ` + `indices length: ${this.indices.length}, max possible length: ${maxPossibleLength}, ` + `values length: ${this.values?.length || 0}`,);
            }
        }

        this.splitTables();

        console.log("Inserted: ", insertedCount, "reused:", reusedCount);
        // console.log("Successful masks:", reusedMaskedCount);
        console.log("Indices Table Length:", this.indices.length);
        console.log("Values Table Length:", this.values?.length || 0);
        console.log("Insert saves:", saves);
        console.log("Modifiers Used:", uniqueModifiers.size, [...uniqueModifiers].map((mod) => {
            let res = this.#indexAddenda.valuesOf(mod);
            delete res.pos;
            return res;
        }),);
        console.log("Chunk Size:", this.chunkSize);
        console.log("Chunk Mask:", this.chunkMask);
        console.log("Chunk Shift:", this.chunkShift);
        console.log("Processing: done.");
        console.timeEnd("Process");
    }

    get #commonIndices() {
        return this.#indicesTables.filter(item => item?.commonValue !== undefined);
    }

    get #uncommonIndices() {
        return this.#indicesTables.filter(item => item?.commonValue === undefined);
    }

    indicesTableSizeInBits() {
        return BigInt(this.#uncommonIndices.reduce((sum, cur) => sum + cur.length, 0)) * this.#indexAddenda.realSize;
    }

    valuesTableSizeInBits() {
        if (this.values === null) {
            return 0n;
        }
        return BigInt(this.values.length) * realSizeOf(this.values.type);
    }

    breakpointsTableSizeInBits() {
        return BigInt(this.#breakpointsTableSize)
    }

    totalTablesSizeInBits() {
        return this.indicesTableSizeInBits() + this.valuesTableSizeInBits() + this.breakpointsTableSizeInBits();
    }

    get breakpointsTableLimit() {
        return this.#props?.indices?.breakpointsTableLimit || this.#uncommonIndices.length;
    }

    // table is a table of starting positions
    getBreakpointsTable(table) {
        table = table.toSorted();
        let breakpointsTableShift = Number(realSizeOf(this.#indexAddenda.sizeof)) - 1;
        const limit = this.breakpointsTableLimit;
        let tableSize = 0;
        let breakpointsTable = [];

        nextShift: for (; breakpointsTableShift > 0; --breakpointsTableShift) {
            tableSize = 0;
            for (let i = 1; i < limit; ++i) {
                const cur = Number(table[i].starting) >> breakpointsTableShift;
                const curE = Number(table[i].ending - 1) >> breakpointsTableShift;
                const prev = Number(table[i - 1].starting) >> breakpointsTableShift;
                const prevE = Number(table[i - 1].ending - 1) >> breakpointsTableShift;
                tableSize = Math.max(curE + 1, tableSize);
                if (cur === prev || cur <= prevE || table[i] === undefined) {
                    continue nextShift;
                }
            }

            if (breakpointsTableShift === 0) {
                throw new Error("Was not able to find a breakpoint shift between the starting positions.");
            }

            if (tableSize === 0) {
                break;
            }

            breakpointsTable = new Array(tableSize);
            breakpointsTable.fill({starting: 0, ending: 0, curIndex: 0, section: 'Optimized Away', offset: 0});
            let index = 1;
            for (let i = 0; i < table.length; ++i, ++index) {
                // console.log(tableSize, breakpointsTableShift, i, table[i])
                let curIndex = table[i].starting >> breakpointsTableShift;
                const curEIndex = (table[i].ending - 1) >> breakpointsTableShift;
                for (; ; ++curIndex /*, ++index*/) {
                    if (breakpointsTable[curIndex]?.starting !== 0) {
                        // continue nextShift;
                        throw new Error(`Replacing breakpoint section: ${tableSize}, ${JSON.stringify(breakpointsTable[curIndex])}; ${JSON.stringify(table[i])}, ${curIndex}, ${index}`);
                    }
                    breakpointsTable[curIndex] = {
                        ...table[i], curIndex, section: index,
                    };
                    if (curIndex === curEIndex) {
                        break;
                    }
                }
            }

            break;
        }
        if (breakpointsTableShift === 0) {
            throw new Error("Was not able to find a breakpoint shift between the starting positions.");
        }
        // console.log(breakpointsTable);

        return {
            breakpointsTableShift, breakpointsTable
        }
    }

    #renderIndicesTables() {
        let printableValues = [];
        let index = 1;
        const commons = this.#commonIndices;
        const uncommons = this.#uncommonIndices;
        const {
            breakpointsTable, breakpointsTableShift
        } = this.getBreakpointsTable(uncommons.map(item => ({
            starting: item.start, ending: item.start + item.length, offset: item.offset
        })));
        const commonValues = commons.map(item => item.commonValue);
        const isSingleCommonValue = commonValues.every(val => val === commonValues[0]);
        // if (!isSingleCommonValue) {
        //     console.error(commonValues);
        //     throw new Error("Multiple common values are not yet implemented, thought it's easy to implement.");
        // }
        const startingType = findBestTypeFrom(breakpointsTable, 'starting');
        const endingType = findBestTypeFrom(breakpointsTable, 'ending');
        const offsetType = findBestTypeFrom(breakpointsTable, 'offset');
        // const align = `std::uint${alignmentOf([realSizeOf(startingType), realSizeOf(endingType), realSizeOf(offsetType)])}_t`
        const sumSize = BigInt(realSizeOf(startingType) + realSizeOf(endingType) + realSizeOf(offsetType)) + (isSingleCommonValue ? 0n : realSizeOf(this.#indexAddenda.sizeof));
        this.#breakpointsTableSize = BigInt(breakpointsTable.length) * sumSize * 8n;
        let allIndicesBits = 0
        let allLength = 0;
        for (const info of uncommons) {
            const {start, length, table} = info;
            if (info?.commonValue !== undefined) {
                continue;
            }
            const indicesBits = table.length * Number(this.#indexAddenda.realSize);
            allIndicesBits += indicesBits;
            allLength += table.length;
            table.trailing_comment = `End of Section #${index} [${start}, ${start + length}) containing ${length} values (${(indicesBits / 8 / 1024).toFixed(2)} KiB).`
            if (commons[index - 1]) {
                table.trailing_comment += ` Skipping ${commons[index - 1].length} values.`;
            }
            printableValues.push(table);
            ++index;
        }

        if (!isSingleCommonValue) {
            if (verbose) {
                for (const {start, length, commonValue: curCommonValue} of commons) {
                    console.log('Common Value Range:', start, length, curCommonValue);
                }
            }
            // let secIndex = 0;
            const mapper = this.#props.indices?.map ?? ((val) => val);
            let lastEnding = 0n;
            for (let i = 0; i !== breakpointsTable.length; ++i) {
                let commonValue = 0n;
                if (breakpointsTable[i].ending >= lastEnding) {
                    lastEnding =  breakpointsTable[i].ending;
                } else {
                    breakpointsTable[i] = breakpointsTable[i - 1];
                }
                for (const {start, commonValue: curCommonValue} of commons) {
                    // const curIndex = start >> breakpointsTableShift;
                    // const curEIndex = (start + length - 1) >> breakpointsTableShift;
                    if (start >= lastEnding) {
                        commonValue = curCommonValue;
                        break;
                    }
                }
                breakpointsTable[i].commonValue = mapper([commonValue])[0];
                // console.log(breakpointsTable[i], commonValue)
                // secIndex += breakpointsTableShift - 1;
            }
        }

        return `
        ${this.#indexAddenda.render()}
        
        ${commons.length < 1 ? "" : commons.length === 1 ? `
            // You can choose between the indices' table using these breakpoints:
            static constexpr std::size_t breakpoint_start = ${commons[0].start}U;
            static constexpr std::size_t breakpoint_end = ${commons[0].start + commons[0].length}U;

            // The removed part of the table has this value in them:
            static constexpr ${this.values.type.description} breakpoint_value = 0x${commons[0].commonValue.toString(16)}U;
        ` : `
            struct ${this.#name}_breakpoint_type {
                ${startingType.description} starting;
                ${endingType.description} ending;
                ${offsetType.description} offset;
                ${isSingleCommonValue ? '' : `${this.#indexAddenda.name} common_value; // if it's not in the specified range, this value should be used.`}
            };
            

            /**
             * You can choose between the indices' table using these breakpoints:
             * 
             * Table size in KibiBytes:  ${(Number(this.#breakpointsTableSize) / 8 / 1024).toFixed(2)} KiB
             */
            static constexpr std::array<${this.#name}_breakpoint_type, ${breakpointsTable.length}U> ${this.#name}_breakpoints{${breakpointsTable.map((item, index) => `
               ${index === 0 ? `${this.#name}_breakpoint_type` : ''}{.starting = ${item.starting}, .ending = ${item.ending}, .offset = ${item.offset} ${isSingleCommonValue ? '' : `, .common_value = ${item?.commonValue ?? 0}`}}, // Section ${item.section}`).join("")}
            };

            static constexpr ${this.#indexAddenda.STLTypeString} ${this.#name}_last_breakpoint{0x${breakpointsTable[breakpointsTable.length - 1].ending.toString(16).toUpperCase()}U};
            static constexpr ${this.#indexAddenda.STLTypeString} ${this.#name}_breakpoint_shift{${breakpointsTableShift}U};
            static constexpr ${this.#indexAddenda.name} ${this.#name}_common_pos{${commonValues.at(-1)}U}; // this is the last common value position
        `}

    /**
     * ${this.#name} Index Table (combined ${uncommons.length} sections)
     *
     * ${commentify(this.#props?.indices?.description)}
     * ${this.#indexAddenda.addenda.length === 1 ? `
     * Each value is a ${this.#indexAddenda.renderPlacements()}
     * ` : `
     * Each value contains ${this.#indexAddenda.addenda.length} numbers hidden inside:
     *     ${this.#indexAddenda.renderPlacements()}
     * `}
     * Table size:
     *   - in bits:       ${allIndicesBits}
     *   - in bytes:      ${allIndicesBits / 8} B
     *   - in KibiBytes:  ${(allIndicesBits / 8 / 1024).toFixed(2)} KiB
     */
    ${renderTableValues({
            name: this.indices.tableName,
            type: this.#indexAddenda,
            printableValues,
            len: allLength,
            map: this.#props?.indices?.map
        })}
        `;
    }


    #renderValuesTables() {
        if (!this.values) {
            return "";
        }

        const indices = this.indices.result;
        const valuesBits = Number(this.valuesTableSizeInBits());
        let printableValues = [];

        if (this.#props?.disableComments) {
            printableValues = [[...this.values.result]];
        } else {
            printableValues = [];

            const poses = {};
            const posesMeta = {};
            indices.forEach((code, index) => {
                const curPos = Number(this.#indexAddenda.addendumValueOf("pos", code));
                if (poses[curPos] === undefined) {
                    poses[curPos] = [];
                    posesMeta[curPos] = {
                        lastRangeStart: NaN, rangeStart: 0,
                    };
                }
                const rangeStart = index << Number(this.#indexAddenda.chunkShift);
                const codeStr = `0x${rangeStart.toString(16)}`;
                if (rangeStart === posesMeta[curPos].lastRangeStart + Number(this.#indexAddenda.chunkSize)) {
                    poses[curPos][poses[curPos].length - 1] = `${posesMeta[curPos].rangeStart}-${codeStr}`;
                } else {
                    poses[curPos].push(codeStr);
                    posesMeta[curPos].rangeStart = codeStr;
                }
                posesMeta[curPos].lastRangeStart = rangeStart;
            });

            // add comments in the middle of the data
            this.values.result.forEach((value, pos) => {
                value = cppValueOf(value, this.values.type);
                if ((poses?.[pos]?.length || 0) === 0) {
                    printableValues.at(-1).push(value);
                    return;
                }
                printableValues.push([]);
                printableValues.at(-1).push(value);
                printableValues.at(-1).comment = `Start of ${poses[pos].join(", ")}:`;
            });
        }
        return `
    /**
     * ${this.#name.toUpperCase()} Values Table
     *
     * ${commentify(this.#props?.values?.description)}
     *
     * Table size:
     *   - in bits:       ${valuesBits}
     *   - in bytes:      ${valuesBits / 8} B
     *   - in KibiBytes:  ${(valuesBits / 8 / 1024).toFixed(2)} KiB
     */
    ${renderTableValues({
            name: this.values.tableName,
            type: this.values.type,
            printableValues,
            len: this.values.length,
            map: this.#props?.values?.map
        })}
    `;
    }

    render() {
        if (this.indices.length === 0 || this.values?.length === 0) {
            throw new Error(`Index or values table is empty: (index: ${this.indices.length}) (values: ${this.values?.length})`);
        }
        const renderFunc = this.#props?.processRendered || ((content) => content);

        return renderFunc(`
    ${this.#renderIndicesTables()}

    ${this.#renderValuesTables()}
        `);
    }
}
