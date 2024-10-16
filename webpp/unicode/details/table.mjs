import * as assert from "node:assert";
import {
    genIndexAddenda,
    ModifiedSpan,
    Modifier,
    rangeLength,
} from "./modifiers.mjs";
import {
    cppValueOf,
    overlapInserts,
    realSizeOf,
    renderTableValues,
    Span,
    TableTraits,
    uint32,
    uint8,
} from "./utils.mjs";

export class TablePairs {
    #indexAddenda;
    #name = "";
    #description = "";
    data = []; // raw, unprocessed data
    #props = {};

    init(meta) {
        this.#props = meta;
        this.#name = this.#props?.name || "table";
        this.#description = this.#props?.description || "";

        // index table's information
        this.#indexAddenda = (meta?.genIndexAddenda || genIndexAddenda)();
        this.#indexAddenda.name = `${this.#name}_index`;
        this.#indexAddenda.description = `${this.#name[0].toUpperCase()}${this.#name.substring(1)} (Index Table)\n${this.#description}`;

        // the tables
        this.indices = new TableTraits(
            this.#props?.indices?.max || 43530,
            this.#props?.indices?.sizeof || uint32,
        );
        if (this.#props?.values !== null) {
            this.values = new TableTraits(
                this.#props?.values?.max || 65535,
                this.#props?.values?.sizeof || uint8,
            );
        } else {
            this.values = null;
        }
    }

    add(codePoint, value) {
        // fill the data
        this.data[Number(codePoint)] = value;

        // if (codePoint === 0xFFC4n || codePoint === 0x1F133n) {
        //     debugger;
        //     console.log("-------------------------------------");
        //     console.log(codePoint.toString(16), codePoint, value, utf8To32(value.mappedTo), utf8To32(value.mappedTo)?.codePointAt(0)?.toString(16));
        //     process.exit(0);
        // }
    }

    #optimizeInserts(inserts, dataView, modifier) {
        if (this.values === null) {
            return {
                valid: true,
                pos: 0n,
                inserts: [],
            };
        }
        let pos = modifier.pos;

        if (inserts.length === 0) {
            return {
                valid: true,
                pos: BigInt(pos),
                inserts,
            };
        }

        const insertsModifier = modifier.clone();
        insertsModifier.resetOnly(["pos" /* 'max_length' */]);
        const modifiedInserts = new ModifiedSpan(inserts, insertsModifier);

        // validating inserts:
        if (
            !this.#indexAddenda.verifyInserts({ inserts, dataView, modifier })
        ) {
            return {
                valid: false,
                ...modifier,
                data: [...modifiedInserts],
            };
        }

        const overlapped = overlapInserts(modifiedInserts, this.values);
        if (overlapped !== 0) {
            pos = BigInt(this.values.length - overlapped);
            inserts = inserts.slice(overlapped, inserts.length);
        }

        if (inserts.length !== 0) {
            let { valid, start, end } = this.#indexAddenda.optimizeInserts({
                inserts,
                dataView,
                modifier,
            });
            if (valid === false) {
                return { valid: false };
            }

            if (this.values.length === 0 && end === 0) {
                end = 1;
            }
            if (start !== 0 && end !== inserts.length) {
                inserts = inserts.slice(start, end);
            }
        }

        return {
            valid: true,
            pos: BigInt(pos),
            inserts,
            overlapped,
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

    dataView(
        codePointStart,
        length = this.rangeLengthStarting(codePointStart),
    ) {
        if (this.#props?.dataView) {
            return this.#props.dataView.call(this, codePointStart, length);
        }
        return new Span(this.data, codePointStart, length);
    }

    insertsDataView(
        codePointStart,
        length = this.rangeLengthStarting(codePointStart),
    ) {
        if (this.#props?.insertsDataView) {
            return this.#props.insertsDataView.call(
                this,
                codePointStart,
                length,
            );
        }
        return this.dataView(codePointStart, length);
    }

    #findSubsetRange(dataView, modifier) {
        if (this.values === null) {
            return null;
        }

        modifier = modifier.clone();
        const left = dataView;
        const right = new ModifiedSpan(this.values, modifier);
        // if (left.length > this.chunkSize) {
        //     return null;
        // }
        // try {
        top: for (let rpos = 0; rpos !== this.values.length; ++rpos) {
            modifier.set({
                pos: BigInt(rpos),
            });
            for (let lpos = 0; lpos !== left.length; ++lpos) {
                const rvalue = right.at(lpos);
                const lvalue = left.at(lpos);
                // if (!Number.isSafeInteger(rvalue) || !Number.isSafeInteger(lvalue)) {
                //     return null;
                // }
                if (rvalue !== lvalue) {
                    continue top;
                }
            }
            return BigInt(rpos);
        }
        // } catch (err) {
        //     if (!(err instanceof RangeError)) {
        //         throw err;
        //     }
        //     else, just say we found nothing
        // }
        return null;

        // modifier.set({pos: 0});
        // const modifiedValues = new ModifiedSpan(this.values, modifier);
        // return findSimilarRange(dataView, modifiedValues);

        // for (let index = 0; index < this.values.length; index += this.chunkSize) {
        //     modifier.set({pos: index});
        //     const pos = findSimilarRange(modifiedValues, this.values);
        //     if (pos !== null) {
        //         return pos;
        //     }
        // }
        // return null;
    }

    #findSimilarMaskedRange(codePointStart) {
        const length = this.rangeLengthStarting(codePointStart);
        let possibilities = [];
        let invalidModifiers = [];
        const dataView = this.dataView(codePointStart);
        const insertsDataView = this.insertsDataView(codePointStart);
        const additionalAddendumValues =
            this.#props?.getModifierAddenda?.call(this, {
                codePointStart,
                length,
                data: this.data,
                dataView,
            }) || {};

        for (const indexModifier of this.#indexAddenda.generate({
            dataView,
            length,
        })) {
            // set the position
            if (!this.#indexAddenda.has("pos")) {
                indexModifier.set({ ...additionalAddendumValues });
            } else {
                indexModifier.set({
                    pos: BigInt(this.values?.index || 0),
                    ...additionalAddendumValues,
                });
            }

            let lastInfoLength = 0;
            let info = {};

            // try {
            const startPos = this.#findSubsetRange(dataView, indexModifier);
            if (startPos === null) {
                info = this.#optimizeInserts(
                    insertsDataView,
                    dataView,
                    indexModifier,
                );
            } else {
                info = {
                    valid: true,
                    pos: startPos,
                    inserts: new Span(),
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
                if (
                    "mask" in indexModifier &&
                    BigInt(indexModifier.pos) !== 0n &&
                    BigInt(indexModifier.mask) === 0n
                ) {
                    debugger;
                    throw new Error(
                        `Invalid calculations. If mask is zero, the position must come out zero too; pos: ${indexModifier.pos}, mask: ${indexModifier.mask}`,
                    );
                }
                possibilities.push({
                    ...info,
                    modifier: indexModifier.clone(),
                });

                // performance trick
                lastInfoLength = info.inserts.length;
                if (lastInfoLength === 0) {
                    break;
                }
            }
            // } catch (err) {
            //     if (err instanceof InvalidModifier) {
            //         invalidModifiers.push(err);
            //     } else {
            //         if (!this.#props?.ignoreErrors) {
            //             throw err;
            //         }
            //     }
            // }

            /// check if we can have "shift"s.
            if (indexModifier.unshiftAll) {
                // try {
                // now, try the shifted inserts as well see if they're any good:
                info = this.#optimizeInserts(
                    indexModifier.unshiftAll(insertsDataView),
                    dataView,
                    indexModifier,
                );
                if (!info.valid) {
                    invalidModifiers.push({
                        ...info,
                    });
                } else {
                    indexModifier.set({
                        pos: info.pos,
                    });
                    if (info.inserts.length < lastInfoLength) {
                        assert.ok(
                            Number.isSafeInteger(indexModifier.pos),
                            "Position should not be null",
                        );
                        if (
                            BigInt(indexModifier.pos) !== 0n &&
                            BigInt(indexModifier.mask) === 0n
                        ) {
                            debugger;
                            throw new Error(
                                "Invalid calculations. If mask is zero, the position must come out zero too.",
                            );
                        }
                        possibilities.push({
                            ...info,
                            modifier: indexModifier.clone(),
                            shifted: indexModifier.shift,
                        });

                        if (info.inserts.length === 0) {
                            break;
                        }
                    }
                }
                // } catch (err) {
                //     if (err instanceof InvalidModifier) {
                //         invalidModifiers.push(err);
                //     } else {
                //         if (!this.#props?.ignoreErrors) {
                //             throw err;
                //         }
                //     }
                // }
            }
        }

        const leastInsertLength = possibilities.reduce(
            (acc, curr) =>
                acc >= Number(curr.inserts.length)
                    ? acc
                    : Number(curr.inserts.length),
            Number(this.#indexAddenda.chunkSize),
        );
        possibilities = possibilities.filter((item) => {
            return (
                item !== undefined && item.inserts.length <= leastInsertLength
            );
        });

        // sort them
        if (this.#props?.toSortedPossibilities) {
            possibilities = this.#props.toSortedPossibilities(possibilities);
        } else if (this.#indexAddenda.has("mask")) {
            possibilities = possibilities.toSorted(
                (a, b) => Number(a.modifier.mask) - Number(b.modifier.mask),
            );
        }

        const codePointStartHex = codePointStart.toString(16);
        const codePointEndHex =
            (codePointStart + length).toString(16) || "infinite";
        console.log(
            `  0x${codePointStartHex}-0x${codePointEndHex}`,
            "invalid-modifiers:",
            invalidModifiers.length,
            "Possibilities:",
            possibilities.length,
            possibilities.slice(0, 5).map((item) => ({
                ...item,
                ...item.modifier.necessaries(),
                inserts: item.inserts.length,
            })),
        );
        if (possibilities.length === 0) {
            console.error(
                `  Empty possibilities:`,
                possibilities,
                this.values?.length || 0,
                this.data.length,
            );
            console.error(
                `  Invalid Modifiers:`,
                invalidModifiers.length,
                invalidModifiers,
            );
            debugger;
            process.exit(1);
        }
        return possibilities.at(0);
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
        for (
            let range = 0n;
            range < this.data.length;
            range += this.#indexAddenda.chunkSize
        ) {
            const codeRange = range >> this.#indexAddenda.chunkShift;
            const length = Math.min(
                this.data.length - Number(range),
                Number(this.#indexAddenda.chunkSize),
            );
            const valueStart = this.values?.index || 0;

            console.log(
                `Batch: #${batchNo++}`,
                "CodePoint:",
                codeRange.toString(16),
                "Values-Table-Length:",
                this.values?.length || 0,
                "range:",
                range,
                "length:",
                length,
                `Progress: ${Math.floor((Number(range) / this.data.length) * 100)}%`,
            );

            let { modifier, inserts, rtrimmed, overlapped } =
                this.#findSimilarMaskedRange(range);
            // assert.ok(Number.isSafeInteger(modifier.pos), "Position should not be null");

            const modifiedValues = this.#props?.modify?.({
                codeRange,
                modifier,
                inserts,
                rtrimmed,
                overlapped,
            });
            modifier = modifiedValues?.modifier || modifier;
            inserts = modifiedValues?.inserts || inserts;
            rtrimmed = modifiedValues?.rtrimmed || rtrimmed;
            overlapped = modifiedValues?.overlapped || overlapped;

            assert.ok(
                modifier instanceof Modifier,
                "The modifier should be an instance of Modifier.",
            );
            // assert.ok(Array.isArray(inserts), "Inserts should be an array.");

            const code = modifier.modifier;

            this.indices.append(code);
            if (inserts.length > 0) {
                if (this.values !== null) {
                    this.values.appendList(inserts);
                    ++insertedCount;
                    saves += length - inserts.length;
                }
            } else {
                ++reusedCount;
                saves += length;
            }
            console.log(
                `  Code Range (${inserts.length ? "Inserted-" + inserts.length : "Reused"}):`,
                codeRange,
                "rtrimmed:",
                rtrimmed,
                "overlapped:",
                overlapped,
                "last-pos",
                valueStart,
                "modifier.pos:",
                modifier.pos,
                modifier.necessaries(),
                "samples:",
                inserts.filter((item) => item).slice(0, 5),
            );
            uniqueModifiers.add(modifier.categorizableModifier);

            // if (mask !== modifier.resetMask && mask !== modifier.minMask) {
            //     ++reusedMaskedCount;
            // }

            /// verify range
            if (this.#props?.validateResults) {
                const dataView = this.dataView(range, length);
                if (null === this.#findSubsetRange(dataView, modifier)) {
                    debugger;
                    this.#findSubsetRange(dataView, modifier);
                    throw new Error("Bad insert");
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

        if (this.#indexAddenda.has("pos")) {
            const maxPossibleLength =
                (0b1n << BigInt(this.#indexAddenda.pos.size)) - 1n;
            if (BigInt(this.indices.length) > maxPossibleLength) {
                debugger;
                throw new Error(
                    "Table size limit reached; the limit is because " +
                        `the pointer to the table is going to be bigger than ${this.#indexAddenda.pos.typeString} size; ` +
                        `indices length: ${this.indices.length}, max possible length: ${maxPossibleLength}, ` +
                        `values length: ${this.values?.length || 0}`,
                );
            }
        }

        console.log("Inserted: ", insertedCount, "reused:", reusedCount);
        // console.log("Successful masks:", reusedMaskedCount);
        console.log("Indices Table Length:", this.indices.length);
        console.log("Values Table Length:", this.values?.length || 0);
        console.log("Insert saves:", saves);
        console.log(
            "Modifiers Used:",
            uniqueModifiers.size,
            [...uniqueModifiers].map((mod) => {
                let res = this.#indexAddenda.valuesOf(mod);
                delete res.pos;
                return res;
            }),
        );
        console.log("Chunk Size:", this.chunkSize);
        console.log("Chunk Mask:", this.chunkMask);
        console.log("Chunk Shift:", this.chunkShift);
        console.log("Processing: done.");
        console.timeEnd("Process");
    }

    indicesTableSizeInBits() {
        return BigInt(this.indices.length) * realSizeOf(this.indices.type);
    }

    valuesTableSizeInBits() {
        if (this.values === null) {
            return 0n;
        }
        return BigInt(this.values.length) * realSizeOf(this.values.type);
    }

    totalTablesSizeInBits() {
        return this.indicesTableSizeInBits() + this.valuesTableSizeInBits();
    }

    render() {
        if (this.indices.length === 0 || this.values?.length === 0) {
            throw new Error(
                `Index or values table is empty: (index: ${this.indices.length}) (values: ${this.values?.length})`,
            );
        }

        const indices = this.indices.result;
        const indicesBits = Number(this.indicesTableSizeInBits());
        const valuesBits = Number(this.valuesTableSizeInBits());

        let printableValues = [];

        if (this.values !== null) {
            if (this.#props?.disableComments) {
                printableValues = [[...this.values.result]];
            } else {
                printableValues = [];

                const poses = {};
                const posesMeta = {};
                indices.forEach((code, index) => {
                    const curPos = Number(
                        this.#indexAddenda.addendumValueOf("pos", code),
                    );
                    if (poses[curPos] === undefined) {
                        poses[curPos] = [];
                        posesMeta[curPos] = {
                            lastRangeStart: NaN,
                            rangeStart: 0,
                        };
                    }
                    const rangeStart =
                        index << Number(this.#indexAddenda.chunkShift);
                    const codeStr = `0x${rangeStart.toString(16)}`;
                    if (
                        rangeStart ===
                        posesMeta[curPos].lastRangeStart +
                            Number(this.#indexAddenda.chunkSize)
                    ) {
                        poses[curPos][poses[curPos].length - 1] =
                            `${posesMeta[curPos].rangeStart}-${codeStr}`;
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
                    printableValues.at(-1).comment =
                        `Start of ${poses[pos].join(", ")}:`;
                });
            }
        }

        const renderFunc =
            this.#props?.processRendered || ((content) => content);
        return renderFunc(`

${this.#indexAddenda.render()}


    /**
     * ${this.#name.toUpperCase()} Index Table
     *
     * ${this.#props?.indices?.description?.replace("\n", "\n     * ") || ""}
     *
     * Each value contains ${this.#indexAddenda.addenda.length} numbers hidden inside:
     *     ${this.#indexAddenda.renderPlacements()}
     *
     * Table size:
     *   - in bits:       ${indicesBits}
     *   - in bytes:      ${indicesBits / 8} B
     *   - in KibiBytes:  ${Math.ceil(indicesBits / 8 / 1024)} KiB
     */
    static constexpr std::array<${this.#indexAddenda.name}, ${indices.length}ULL> ${this.#name.toLowerCase()}_indices{
        ${indices.join(", ")}
    };
    ${
        this.values === null
            ? ""
            : `
    /**
     * ${this.#name.toUpperCase()} Values Table
     *
     * ${this.#props?.indices?.description?.replace("\n", "\n     * ") || ""}
     *
     * Table size:
     *   - in bits:       ${valuesBits}
     *   - in bytes:      ${valuesBits / 8} B
     *   - in KibiBytes:  ${Math.ceil(valuesBits / 8 / 1024)} KiB
     */
    ${renderTableValues({
        name: this.#name,
        type: this.values.type,
        printableValues,
        len: this.values.length,
    })}
    `
    }
        `);
    }
}
