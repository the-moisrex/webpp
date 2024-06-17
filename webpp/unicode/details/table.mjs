import {
    bitCeil,
    findSimilarRange,
    overlapInserts,
    Span,
    TableTraits,
    uint32,
    uint8,
} from "./utils.mjs";
import {genIndexAddenda, InvalidModifier, ModifiedSpan, Modifier, rangeLength} from "./modifiers.mjs";
import * as assert from "node:assert";

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


        // the tables
        this.indices = new TableTraits(this.#props?.indices?.max || 43530, this.#props?.indices?.sizeof || uint32);
        this.values = new TableTraits(this.#props?.values?.max || 65535, this.values || uint8);

        // index table's information
        this.#indexAddenda = (meta?.genIndexAddenda || genIndexAddenda)();
        this.#indexAddenda.name = `${this.#name}_index`;
        this.#indexAddenda.description = `${this.#name[0].toUpperCase()}${this.#name.substring(1)} (Index Table)\n${this.#description}`;
    }

    add(codePoint, value) {

        // fill the data
        this.data[codePoint] = value;
    }

    /// This function compresses the specified range based on the input modifier.
    /// For example, an array of zeros, with mask of zero, only needs the first element
    #rightTrimInserts(inserts, modifier) {
        if (this.#indexAddenda.has("mask")) {
            return Math.min(inserts.length, bitCeil(modifier.mask));
        } else {
            return inserts.length;
        }
        // if (inserts.length <= 1) {
        //     return inserts.length;
        // }
        // let rtrimPos = 0;
        // for (let pos = inserts.length - 1; pos >= 0; --pos) {
        //     const applied = modifier.applyMask(pos);
        //     if (applied.pos !== 0) {
        //         rtrimPos = pos;
        //         break;
        //     }
        // }
        // return rtrimPos;
    }

    #optimizeInserts(inserts, dataView, modifier) {
        let pos = modifier.pos;

        const insertsModifier = modifier.clone();
        insertsModifier.set({pos: 0});
        const modifiedInserts = new ModifiedSpan(inserts, insertsModifier);

        // validating inserts:
        for (let index = 0; index !== inserts.length; ++index) {
            const realValue = dataView.at(index);
            const insertValue = modifiedInserts.at(index);
            if (realValue !== insertValue) {
                // throw new InvalidModifier({index, realValue, insertValue, ...modifier});
                return {valid: false, index, realValue, insertValue, ...modifier};
            }
        }

        const overlapped = overlapInserts(modifiedInserts, this.values);
        if (overlapped !== 0) {
            pos = this.values.length - overlapped;
            inserts = inserts.slice(overlapped, inserts.length);
        }

        let rtrimmed = 0;
        if (inserts.length !== 0) {
            let rtrimmedPos = this.#rightTrimInserts(inserts, modifier);
            if (this.values.length === 0 && rtrimmedPos === 0) {
                rtrimmedPos = 1;
            }
            rtrimmed = inserts.length - rtrimmedPos;
            if (rtrimmed !== 0) {
                inserts = inserts.slice(0, rtrimmedPos);
            }

            if (!Number.isSafeInteger(rtrimmed) || rtrimmed < 0) {
                debugger;
                throw new Error(`Negative rtrimmed is not ok; rtrimmed: ${rtrimmed}; rtrimmedPos: ${rtrimmedPos}, insertsLength: ${inserts.length}`);
            }
        }

        return {valid: true, pos, inserts, overlapped, rtrimmed};
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

    dataView(codePointStart, length = this.rangeLengthStarting(codePointStart)) {
        if (this.#props?.dataView) {
            return this.#props.dataView.call(this, codePointStart, length);
        }
        return new Span(this.data, codePointStart, length);
    }

    #findSubsetRange(dataView, modifier) {
        modifier = modifier.clone();
        const left = dataView;
        const right = new ModifiedSpan(this.values, modifier);
        // try {
        top: for (let rpos = 0; rpos !== this.values.length; ++rpos) {
            modifier.set({pos: rpos});
            for (let lpos = 0; lpos !== left.length; ++lpos) {
                const rvalue = right.at(lpos);
                const lvalue = left.at(lpos);
                if (!Number.isSafeInteger(rvalue) && Number.isSafeInteger(lvalue)) {
                    return null;
                }
                if (rvalue !== lvalue) {
                    continue top;
                }
            }
            return rpos;
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
        const additionalAddendumValues = this.#props?.getModifierAddenda?.call(this, {
            codePointStart,
            length,
            dataView
        }) || {};

        if (codePointStart === 0x2f6e) {
            debugger
        }

        for (const indexModifier of this.#indexAddenda.generate()) {

            // set the position
            indexModifier.set({pos: this.values.index, ...additionalAddendumValues});

            let lastInfoLength = 0;
            let info = {};

            // try {
            const startPos = this.#findSubsetRange(dataView, indexModifier);
            if (startPos === null) {
                info = this.#optimizeInserts(dataView, dataView, indexModifier);
            } else {
                info = {valid: true, pos: startPos, inserts: new Span()};
            }
            if (!info.valid) {
                invalidModifiers.push({...info});
            } else {
                indexModifier.set({pos: info.pos});

                assert.ok(Number.isSafeInteger(indexModifier.pos), "Position should not be null");
                if (indexModifier.pos !== 0 && indexModifier.mask === 0) {
                    debugger;
                    throw new Error(`Invalid calculations. If mask is zero, the position must come out zero too; pos: ${indexModifier.pos}, mask: ${indexModifier.mask}`);
                }
                possibilities.push({...info, modifier: indexModifier.clone()});

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
                info = this.#optimizeInserts(indexModifier.unshiftAll(dataView), dataView, indexModifier);
                if (!info.valid) {
                    invalidModifiers.push({...info});
                } else {
                    indexModifier.set({pos: info.pos});
                    if (info.inserts.length < lastInfoLength) {
                        assert.ok(Number.isSafeInteger(indexModifier.pos), "Position should not be null");
                        if (indexModifier.pos !== 0 && indexModifier.mask === 0) {
                            debugger;
                            throw new Error("Invalid calculations. If mask is zero, the position must come out zero too.");
                        }
                        possibilities.push({...info, modifier: indexModifier.clone(), shifted: indexModifier.shift});

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

        const leastInsertLength = possibilities.reduce((acc, curr) => acc >= curr.inserts.length ? acc : curr.inserts.length, this.#indexAddenda.chunkSize);
        possibilities = possibilities.filter(item => {
            return item !== undefined && item.inserts.length <= leastInsertLength;
        });

        // sort them
        if (this.#props?.toSortedPossibilities) {
            possibilities = this.#props.toSortedPossibilities(possibilities);
        } else if (this.#indexAddenda.has("mask")) {
            possibilities = possibilities.toSorted((a, b) => a.modifier.mask - b.modifier.mask);
        }

        const codePointStartHex = codePointStart.toString(16);
        const codePointEndHex = (codePointStart + length).toString(16) || "infinite";
        console.log(`  0x${codePointStartHex}-0x${codePointEndHex}`,
            "invalid-modifiers:", invalidModifiers.length, "Possibilities:", possibilities.length,
            possibilities.slice(0, 5).map(item => ({
                ...item, ...item.modifier.necessaries(),
                inserts: item.inserts.length
            })));
        if (possibilities.length === 0) {
            console.error(`  Empty possibilities:`, possibilities, this.values.length, this.data.length);
            console.error(`  Invalid Modifiers:`, invalidModifiers.length,
                invalidModifiers.map(item => item?.toString() || item));
            debugger;
            process.exit(1);
        }
        return possibilities.at(0);
    }

    /// Post-Processing
    process() {
        console.time("Process");
        console.log("Processing...");

        this.#props?.tests?.();

        let batchNo = 0;
        let insertedCount = 0;
        let reusedCount = 0;
        // let reusedMaskedCount = 0;
        let saves = 0;
        let uniqueModifiers = new Set();
        for (let range = 0; range < this.data.length; range += this.#indexAddenda.chunkSize) {

            const codeRange = range >>> this.#indexAddenda.chunkShift;
            const length = Math.min(this.data.length - range, this.#indexAddenda.chunkSize);
            const valueStart = this.values.index;

            console.log(`Batch: #${batchNo++}`, "CodePoint:", codeRange.toString(16),
                "Values-Table-Length:", this.values.length, "range:", range, "length:", length,
                `Progress: ${Math.floor(range / this.data.length * 100)}%`);

            let {modifier, inserts, rtrimmed, overlapped} = this.#findSimilarMaskedRange(range);
            assert.ok(Number.isSafeInteger(modifier.pos), "Position should not be null");

            const modifiedValues = this.#props?.modify?.({modifier, inserts, rtrimmed, overlapped});
            modifier = modifiedValues?.modifier || modifier;
            inserts = modifiedValues?.inserts || inserts;
            rtrimmed = modifiedValues?.rtrimmed || rtrimmed;
            overlapped = modifiedValues?.overlapped || overlapped;

            assert.ok(modifier instanceof Modifier, "The modifier should be an instance of Modifier.");
            // assert.ok(Array.isArray(inserts), "Inserts should be an array.");

            const code = modifier.modifier;

            this.indices.append(code);
            if (inserts.length > 0) {
                this.values.appendList(inserts);
                ++insertedCount;
                saves += length - inserts.length;
            } else {
                ++reusedCount;
                saves += length;
            }
            console.log(`  Code Range (${inserts.length ? "Inserted-" + inserts.length : "Reused"}):`,
                codeRange, "rtrimmed:", rtrimmed, "overlapped:", overlapped, "last-pos", valueStart, "modifier.pos:", modifier.pos, modifier.necessaries(),
                "samples:", inserts.filter(item => item).slice(0, 5));
            uniqueModifiers.add(modifier.generableModifier);

            // if (mask !== modifier.resetMask && mask !== modifier.minMask) {
            //     ++reusedMaskedCount;
            // }

            /// verify range
            if (this.#props?.validateResults) {
                const dataView = this.dataView(range, length);
                const modifiedValues = new ModifiedSpan(this.values, modifier);
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

        const maxPossibleLength = ((0b1 << this.#indexAddenda.pos.size) - 1);
        if (this.indices.length > maxPossibleLength) {
            debugger;
            throw new Error("Table size limit reached; the limit is because " +
                `the pointer to the table is going to be bigger than ${this.#indexAddenda.pos.typeString} size; ` +
                `indices length: ${this.indices.length}, max possible length: ${maxPossibleLength}, ` +
                `values length: ${this.values.length}`
            );
        }

        console.log("Inserted: ", insertedCount, "reused:", reusedCount);
        // console.log("Successful masks:", reusedMaskedCount);
        console.log("Indices Table Length:", this.indices.length);
        console.log("Values Table Length:", this.values.length);
        console.log("Insert saves:", saves);
        console.log("Modifiers Used:", uniqueModifiers.size, [...uniqueModifiers].map(mod => {
            let res = this.#indexAddenda.valuesOf(mod);
            delete res.pos;
            return res;
        }));
        console.log("Processing: done.");
        console.timeEnd("Process");
    }

    indicesTableSizeInBits() {
        return this.indices.length * this.indices.sizeof;
    }

    valuesTableSizeInBits() {
        return this.values.length * this.values.sizeof;
    }

    totalTablesSizeInBits() {
        return this.indicesTableSizeInBits() + this.valuesTableSizeInBits();
    }

    render() {

        const indices = this.indices.result
        const indicesBits = this.indicesTableSizeInBits();
        const cccBits = this.valuesTableSizeInBits();

        let printableValues = [];

        if (this.#props?.disableComments) {
            printableValues = [...this.values.result];
        } else {
            // add comments in the middle of the data
            this.values.result.forEach((value, pos) => {
                const poses = [];

                let lastRangeStart = NaN;
                indices.forEach((code, index) => {
                    const curPos = this.#indexAddenda.addendumValueOf("pos", code);
                    if (curPos === pos) {
                        const rangeStart = index << this.#indexAddenda.chunkShift;
                        const code = `0x${rangeStart.toString(16)}`;
                        if (rangeStart === (lastRangeStart + this.#indexAddenda.chunkSize)) {
                            const lastPos = poses.at(-1);
                            let dashPlace = lastPos.indexOf("-");
                            if (dashPlace < 0) {
                                dashPlace = lastPos.length;
                            }
                            poses[poses.length - 1] = `${lastPos.substring(0, dashPlace)}-${code}`;
                        } else {
                            poses.push(code);
                        }
                        lastRangeStart = rangeStart;
                    }
                });
                if (poses.length === 0) {
                    printableValues.push(value);
                    return;
                }
                printableValues.push(`
        // Start of ${poses.join(", ")}:
        ${value}`);
            });
        }

        const renderFunc = this.#props?.processRendered || (content => content);
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
    static constexpr std::array<${this.#indexAddenda.name}, ${indices.length}ULL> ccc_indices{
        ${indices.join(", ")}
    };
    
    
    /**
     * ${this.#name.toUpperCase()} Values Table
     *
     * ${this.#props?.indices?.description?.replace("\n", "\n     * ") || ""}
     * 
     * Table size:
     *   - in bits:       ${cccBits}
     *   - in bytes:      ${cccBits / 8} B
     *   - in KibiBytes:  ${Math.ceil(cccBits / 8 / 1024)} KiB
     */
    static constexpr std::array<${this.values.STLTypeString}, ${this.values.length}ULL> ccc_values{
        ${printableValues}
    };
        `);
    }
}

