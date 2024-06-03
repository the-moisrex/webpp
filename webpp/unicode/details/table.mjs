import {
    findSimilarRange,
    overlapInserts,
    popcount,
    sizeOf,
    Span,
    TableTraits,
    uint32,
    uint8,
    writePieces
} from "./utils.mjs";
import {genIndexAddenda, InvalidModifier, ModifiedSpan, rangeLength} from "./modifiers.mjs";

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

        this.#props?.processAdds?.();
    }

    /// This function compresses the specified range based on the input modifier.
    /// For example, an array of zeros, with mask of zero, only needs the first element
    #rightTrimInserts(inserts, modifier) {
        if (inserts.length <= 1) {
            return inserts.length;
        }
        let rtrimPos = 0;
        for (let pos = inserts.length - 1; pos >= 0; --pos) {
            const applied = modifier.applyMask(pos);
            if (applied.pos !== 0) {
                rtrimPos = pos;
                break;
            }
        }
        return this.values.length === 0 ? Math.max(1, rtrimPos) : rtrimPos;
    }

    #optimizeInserts(inserts, dataView, modifier) {
        let pos = null;

        const modifiedInserts = new ModifiedSpan(inserts, 0, modifier);

        // validating inserts:
        for (let index = 0; index !== inserts.length; ++index) {
            const realValue = dataView.at(index);
            const insertValue = modifiedInserts.at(index);
            if (realValue !== insertValue) {
                throw new InvalidModifier({index, realValue, insertValue, ...modifier});
            }
        }

        const overlapped = overlapInserts(modifiedInserts, this.values);
        if (overlapped !== 0) {
            pos = this.values.length - overlapped;
            inserts = inserts.slice(overlapped, inserts.length);
        }

        const rtrimmedPos = this.#rightTrimInserts(inserts, modifier);
        const rtrimmed = inserts.length - rtrimmedPos;
        if (rtrimmed !== 0) {
            inserts = inserts.slice(0, rtrimmedPos);
        }

        return {pos, inserts, overlapped, rtrimmed};
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

    #findSimilarMaskedRange(codePointStart) {
        const length = rangeLength(codePointStart, this.data.length, this.chunkSize);
        let possibilities = [];
        let invalidModifiers = [];
        for (const indexModifier of this.#indexAddenda.generate()) {

            // set the position
            indexModifier.set({pos: this.values.index});

            const dataView =
                new Span(this.data, codePointStart, rangeLength(codePointStart, this.data.length, this.chunkSize));
            const modifiedCCC = new ModifiedSpan(this.values, 0, indexModifier);
            let lastInfoLength = 0;
            let info = {};

            try {
                const startPos = findSimilarRange(dataView, modifiedCCC);
                if (startPos === null) {
                    info = this.#optimizeInserts(dataView, dataView, indexModifier);
                } else {
                    info = {pos: startPos, inserts: new Span()};
                }
                indexModifier.set({pos: info.pos});

                possibilities.push({...info, modifier: indexModifier.clone()});

                // performance trick
                lastInfoLength = info.inserts.length;
                if (lastInfoLength === 0) {
                    break;
                }
            } catch (err) {
                if (err instanceof InvalidModifier) {
                    invalidModifiers.push(err);
                } else {
                    if (!this.#props?.ignoreErrors) {
                        throw err;
                    }
                }
            }

            /// check if we can have "shift"s.
            if (indexModifier.unshiftAll) {
                try {
                    // now, try the shifted inserts as well see if they're any good:
                    info = this.#optimizeInserts(indexModifier.unshiftAll(dataView), dataView, indexModifier);
                    indexModifier.set({pos: info.pos});
                    if (info.inserts.length < lastInfoLength) {
                        possibilities.push({...info, modifier: indexModifier, shifted: indexModifier.shift});

                        if (info.inserts.length === 0) {
                            break;
                        }
                    }
                } catch (err) {
                    if (err instanceof InvalidModifier) {
                        invalidModifiers.push(err);
                    } else {
                        if (!this.#props?.ignoreErrors) {
                            throw err;
                        }
                    }
                }
            }
        }

        const leastInsertLength = possibilities.reduce((acc, curr) => acc >= curr.inserts.length ? acc : curr.inserts.length, this.#indexAddenda.chunkSize);
        possibilities = possibilities.filter(item => {
            return item !== undefined && item.inserts.length <= leastInsertLength;
        });
        possibilities = possibilities.toSorted((a, b) => a.modifier.mask - b.modifier.mask);
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

            console.log(`Batch: #${batchNo++}`, "CodePoint:", codeRange.toString(16),
                "CCC-Table-Length:", this.values.length, "range:", range, "length:", length,
                `Progress: ${Math.floor(range / this.data.length * 100)}%`);

            let {modifier, inserts} = this.#findSimilarMaskedRange(range);
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
                codeRange, modifier.necessaries(),
                "samples:", inserts.filter(item => item).slice(0, 5));
            uniqueModifiers.add(modifier.generableModifier);

            // if (mask !== modifier.resetMask && mask !== modifier.minMask) {
            //     ++reusedMaskedCount;
            // }

            /// verify range
            // for (let ith = 0; ith !== length; ++ith) {
            //     if (!modifiers.matches(this.values, this.data, pos, codeRange, ith, modifier)) {
            //         console.error("  Validation error:", codeRange, ith, pos, inserts);
            //         process.exit(1);
            //         return;
            //     }
            // }
        }

        if (this.indices.length > ((0b1 << this.#indexAddenda.pos.size) - 1)) {
            throw new Error("Table size limit reached; the limit is because " +
                `the pointer to the table is going to be bigger than ${this.#indexAddenda.typeString} size.`);
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

    render() {

        const indices = this.indices.result
        const indicesBits = indices.length * this.indices.sizeof;
        const cccBits = this.values.length * this.values.sizeof;

        let printableValues = [];

        if (this.#props?.disableComments) {
            printableValues = [...this.values.result];
        } else {
            // add comments in the middle of the data
            this.values.result.forEach((value, pos) => {
                const poses = [];

                indices.forEach((code, index) => {
                    const curPos = this.#indexAddenda.addendumValueOf("pos", code);
                    if (curPos === pos) {
                        poses.push(`0x${(index << this.#indexAddenda.chunkShift).toString(16)}`);
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
     * CCC Index Table
     *
     * CCC: Canonical Combining Class
     * These are the indices that are used to find which values from "ccc_values" table correspond to a
     * Unicode Code Point.
     * 
     * Each value contains 3 numbers:
     *     [16bits: pos] + [8bits: shift] + [8bits: mask]
     * 
     *   - pos:   it's the index that points to the \`ccc_values\` table.
     *            it's the starting point of a (at most) 256 length CCC values.
     * 
     *   - shift: add this value to the CCC value, after you applied the mask and found
     *            the actual position of the CCC value.
     *
     *   - mask:  apply this mask (with an & operator), to this: pos + (code_point % 256)
     *            which means, in order to get the CCC value of a \`code_point\`, you need to do this:
     *            @code
     *                auto code  = ccc_index[code_point >> 8]
     *                auto shift = (code >> 8) & 0xFF;
     *                auto mask  = code & 0xFF;
     *                auto pos   = (code >> 16) & 0xFF;
     *                auto ccc   = ccc_values[pos + ((code_point % 256) & mask)] + shift;
     *            @endcode
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
     * CCC Table
     *
     * CCC: Canonical Combining Class
     * These values are calculated and individually represent actual CCC values, but they have no
     * valid order by themselves, and they only make sense if they're being used in conjunction with
     * the "ccc_indices" table.
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

