/***
 * This file downloads UnicodeData.txt, and generates a C++ header file.
 *
 * Details on parsing this file can be found here:
 * UTS #44: https://www.unicode.org/reports/tr44/#UnicodeData.txt
 */
import * as readme from "./readme.mjs";
import * as UnicodeData from "./UnicodeData.mjs";
import {
    uint8,
    uint32,
    Span,
    TableTraits, findSimilarRange, overlapInserts,
} from "./utils.mjs";
import {
    ModifiedSpan,
    rangeLength,
    InvalidModifier,
    Addenda,
    genDefaultAddendaPack, genIndexAddenda
} from "./modifiers.mjs"
import * as path from "node:path";
import {promises as fs} from 'fs';
import * as child_process from "node:child_process";
import {getReadme} from "./readme.mjs";

const ignoreErrors = false;
const cccOutFile = `ccc_tables.hpp`;

const start = async () => {
    await readme.download();

    // database file
    const cccsTables = new CCCTables(256);
    await UnicodeData.parse(cccsTables, UnicodeData.properties.ccc);
    cccsTables?.finalize?.();
    await createTableFile([cccsTables]);
    console.log('File processing completed.');
};

class CCCTables {
    #indexAddenda;

    constructor() {
        this.lastZero = 0;

        // these numbers are educated guesses from other projects, they're not that important!
        this.indeces = new TableTraits(4353 * 10, uint32);
        this.cccs = new TableTraits(65535, uint8);

        this.#indexAddenda = genIndexAddenda();
        this.#indexAddenda.name = "ccc_indices";
        this.#indexAddenda.description = `CCC Indexes`;

        this.data = [];
    }

    add(codePoint, ccc) {

        // fill the data
        this.data[codePoint] = ccc;

        // calculating the last item that it's ccc is zero
        if (ccc !== 0) {
            this.lastZero = codePoint + 1;
        }
    }

    #tests() {

        /// Sanity check: see if we have skipped adding some code points to the table
        const undefinedIndex = this.data.findIndex(codePoint => codePoint === undefined);
        if (undefinedIndex !== -1) {
            console.error("Error: Undefined Code Point.", undefinedIndex, this.data.at(undefinedIndex),
                this.data);
            process.exit(1);
        }

        if (this.data[0x1CE8] !== 1) {
            console.error("Invalid parsing.");
            process.exit(1);
        }
    }

    /// This function compresses the specified range based on the input modifier.
    /// For example, an array of zeros, with mask of zero, only needs the first element
    #rightTrimInserts(inserts, modifier) {
        if (inserts.length <= 1) {
            return inserts.length;
        }
        let rtrimPos = 0;
        for (let pos = inserts.length - 1; pos >= 0; --pos) {
            if (modifier.applyMask(pos) !== 0) {
                rtrimPos = pos;
                break;
            }
        }
        return Math.max(1, rtrimPos);
    }

    #optimizeInserts(inserts, dataView, modifier) {
        let pos = null;

        const modifiedInserts = new ModifiedSpan(inserts, 0, modifier);

        // validating inserts:
        for (let index = 0; index !== inserts.length; ++index) {
            const realCCC = dataView.at(index);
            const insertCCC = modifiedInserts.at(index);
            if (realCCC !== insertCCC) {
                throw new InvalidModifier({index, realCCC, insertCCC});
            }
        }

        const overlapped = overlapInserts(modifiedInserts, this.cccs);
        if (overlapped !== 0) {
            pos = this.cccs.length - overlapped;
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

    #findSimilarMaskedRange(codePointStart) {
        const length = rangeLength(codePointStart, this.data.length, this.chunkSize);
        let possibilities = [];
        let invalidModifiers = [];
        for (const indexModifier of this.#indexAddenda.generate()) {
            const dataView =
                new Span(this.data, codePointStart, rangeLength(codePointStart, this.data.length, this.chunkSize));
            const modifiedCCC = new ModifiedSpan(this.cccs, 0, indexModifier);
            let lastInfoLength = 0;
            let info = {};

            try {
                const startPos = findSimilarRange(dataView, modifiedCCC);
                if (startPos === null) {
                    info = this.#optimizeInserts(dataView, dataView, indexModifier);
                } else {
                    info = {pos: startPos, inserts: new Span()};
                }

                possibilities.push({...info, modifier: indexModifier});

                // performance trick
                lastInfoLength = info.inserts.length;
                if (lastInfoLength === 0) {
                    break;
                }
            } catch (err) {
                if (err instanceof InvalidModifier) {
                    invalidModifiers.push(err);
                } else {
                    if (!ignoreErrors) {
                        throw err;
                    }
                }
            }

            try {
                // now, try the shifted inserts as well see if they're any good:
                info = this.#optimizeInserts(indexModifier.unshiftAll(dataView), dataView, indexModifier);
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
                    if (!ignoreErrors) {
                        throw err;
                    }
                }
            }
        }

        possibilities = possibilities.filter(item => item !== undefined);
        const codePointStartHex = codePointStart.toString(16);
        const codePointEndHex = (codePointStart + length).toString(16) || "infinite";
        console.log(`  0x${codePointStartHex}-0x${codePointEndHex}`,
            "invalid-modifiers:", invalidModifiers.length, "Possibilities:", possibilities.length,
            possibilities.slice(0, 5).map(item => ({...item, inserts: item.inserts.length})));
        if (possibilities.length === 0) {
            console.error(`  Empty possibilities:`, possibilities, this.cccs.length, this.data.length);
            console.error(`  Invalid Modifiers:`, invalidModifiers.length,
                invalidModifiers.map(item => item?.toString() || item));
            process.exit(1);
        }
        return possibilities.at(0);
    }

    /// Post-Processing
    finalize() {
        console.time("Finalize");
        console.log("Finalizing...");

        this.#tests();

        let batchNo = 0;
        let insertedCount = 0;
        let reusedCount = 0;
        let reusedMaskedCount = 0;
        let saves = 0;
        let uniqueModifiers = new Set();
        for (let range = 0; range < this.data.length; range += 256) {

            const codeRange = range >>> 8;
            const length = Math.min(this.data.length - range, 256);

            console.log(`Batch: #${batchNo++}`, "CodePoint:", codeRange.toString(16),
                "CCC-Table-Length:", this.cccs.length, "range:", range, "length:", length,
                `Progress: ${Math.floor(range / this.data.length * 100)}%`);

            let {pos, modifier, inserts} = this.#findSimilarMaskedRange(range);
            const {mask, shift} = modifier.values();
            pos = pos === null ? (mask === 0 ? 0 : this.cccs.index) : pos;
            const helperCode = modifier.modifier;
            this.indeces.append(helperCode);
            if (inserts.length > 0) {
                this.cccs.appendList(inserts);
                ++insertedCount;
                saves += length - inserts.length;
            } else {
                ++reusedCount;
                saves += length;
            }
            console.log(`  Code Range (${inserts.length ? "Inserted-" + inserts.length : "Reused"}):`,
                codeRange, "mask:", mask, "shift:", shift, "pos:", pos, 'code:', helperCode,
                "samples:", inserts.filter(item => item).slice(0, 5));
            uniqueModifiers.add(modifier);

            if (mask !== modifier.resetMask && mask !== modifier.minMask) {
                ++reusedMaskedCount;
            }

            /// verify range
            // for (let ith = 0; ith !== length; ++ith) {
            //     if (!modifiers.matches(this.cccs, this.data, pos, codeRange, ith, modifier)) {
            //         console.error("  Validation error:", codeRange, ith, pos, inserts);
            //         process.exit(1);
            //         return;
            //     }
            // }
        }

        if (this.cccs.length > 65535) {
            console.error(
                "Table size limit reached; the limit is because the pointer to the table is going to be bigger than uint16_t (16 bits) size.");
            process.exit(1);
            return;
        }

        console.log("Inserted: ", insertedCount, "reused:", reusedCount);
        console.log("Successful masks:", reusedMaskedCount);
        console.log("CCCs Table Length:", this.cccs.length);
        console.log("Insert saves:", saves);
        console.log("Modifiers Used:", uniqueModifiers.size,
            [...uniqueModifiers].map(mod => ({modifier: mod, ...modifiers.info(mod)})));
        console.log("Finalizing: done.");
        console.timeEnd("Finalize");
    }

    render() {

        const indeces = this.indeces.result.slice(0, this.lastZero >>> 8);
        const indecesBits = indeces.length * this.indeces.sizeof;
        const cccBits = this.cccs.length * this.cccs.sizeof;

        const printableCCCs = [];

        // add comments in the middle of the data
        this.cccs.result.forEach((ccc, pos) => {
            const poses = [];

            indeces.forEach((code, index) => {
                const cccIndex = modifiers.cccIndexOf(code);
                if (cccIndex === pos) {
                    poses.push(`0x${(index << 8).toString(16)}`);
                }
            });
            if (poses.length === 0) {
                printableCCCs.push(ccc);
                return;
            }
            printableCCCs.push(`
        // Start of ${poses.join(", ")}:
        ${ccc}`);
        });

        return `
    /**
     * In "ccc_index" table, any code point bigger than this number will have "zero" as it's CCC value;
     * so it's designed this way to reduce the table size.
     */
    static constexpr auto trailing_zero_cccs = 0x${this.lastZero.toString(16).toUpperCase()}UL;
    
    /**
     * CCC Index Table
     *
     * CCC: Canonical Combining Class
     * These are the indeces that are used to find which values from "ccc_values" table correspond to a
     * unicode code point.
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
     *   - in bits:       ${indecesBits}
     *   - in bytes:      ${indecesBits / 8} B
     *   - in KibiBytes:  ${Math.ceil(indecesBits / 8 / 1024)} KiB
     */
    static constexpr std::array<${this.indeces.STLTypeString}, ${indeces.length}ULL> ccc_index{
        ${indeces.join(", ")}
    };
    
    
    /**
     * CCC Table
     *
     * CCC: Canonical Combining Class
     * These values are calculated and individually represend actual CCC values, but they have no
     * valid order by themselves, and they only make sense if they're being used in conjuction with
     * the "ccc_index" table.
     * 
     * Table size:
     *   - in bits:       ${cccBits}
     *   - in bytes:      ${cccBits / 8} B
     *   - in KibiBytes:  ${Math.ceil(cccBits / 8 / 1024)} KiB
     */
    static constexpr std::array<${this.cccs.STLTypeString}, ${this.cccs.length}ULL> ccc_values{
        ${printableCCCs}
    };
        `;
    }
}

const createTableFile = async (tables) => {
    const readmeData = await getReadme();
    const begContent = `
/**
 * Attention: Auto-generated file, don't modify.
 * 
 *   Auto generated from:                ${path.basename(new URL(import.meta.url).pathname)}
 *   Unicode UCD Database Creation Date: ${readmeData.date}
 *   This file's generation date:        ${new Date().toUTCString()}
 *   Unicode Version:                    ${readmeData.version}
 *
 * Details about the contents of this file can be found here:
 *   UTS #15: https://www.unicode.org/reports/tr15/
 *   UTS #44: https://www.unicode.org/reports/tr44/#UnicodeData.txt
 *   IDN FAQ: https://www.unicode.org/faq/idn.html
 *
 *   UCD Database Code Points (used the get the CCC values and what not):
 *       ${UnicodeData.fileUrl}
 *   UCD README file (used to check the version and creation date):
 *       ${readme.fileUrl}
 */
 
#ifndef WEBPP_UNICODE_CCC_TABLES_HPP
#define WEBPP_UNICODE_CCC_TABLES_HPP

#include <array>
#include <cstdint>

namespace webpp::unicode::details {

`;

    const endContent = `
} // namespace webpp::unicode::details

#endif // WEBPP_UNICODE_CCC_TABLES_HPP
    `;

    await fs.writeFile(cccOutFile, begContent);
    for (const table of tables) {
        await fs.appendFile(cccOutFile, table.render());
    }
    await fs.appendFile(cccOutFile, endContent);

    // Reformat the file
    child_process.exec(`clang-format -i "${cccOutFile}"`, err => {
        if (err) {
            console.error("Could not re-format the file.", err);
        } else {
            console.log("Clang-format completed.");
        }
    });
};

start();
