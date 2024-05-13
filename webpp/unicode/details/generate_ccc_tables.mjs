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
    uint16,
    uint8x2,
    uint32,
    updateProgressBar,
    Span,
    InvalidModifier,
    TableTraits,
} from "./utils.mjs";
import * as path from "node:path";
import {promises as fs} from 'fs';
import * as child_process from "node:child_process";
import {getReadme} from "./readme.mjs";

const ignoreErrors = false;
const cccOutFile = `ccc_tables.hpp`;

const rangeLength = (codePointStart, dataLength) =>
    Math.min(dataLength, codePointStart + 0b1_0000_0000 /* aka 256 */) - codePointStart;

class ModifierComputer {
    constructor(data, codePointStart = 0) {
        const dataLength = rangeLength(codePointStart, data.length);
        this.index = 0;
        const masks = new Set();
        const shifts = new Set();
        let zerosIndex = 0;

        for (let codePoint = codePointStart; codePoint < (codePointStart + dataLength); ++codePoint) {
            const ccc = data.at(codePoint);
            masks.add(ccc);
            shifts.add(ccc);
            if (ccc === 0 && zerosIndex === 0) {
                zerosIndex = codePoint;
            }
        }

        // add some masks
        for (let i = 0; ; ++i) {
            const length = masks.size;
            if (i === length) {
                break;
            }
            for (let pos = i; pos !== length; ++pos) {
                masks.add(masks[i] | masks[i + pos]);
            }
        }
        // for (let i = 0; i !== 0xFF; ++i) {
        //     masks.add(i);
        // }

        let value = 0;
        while (value < zerosIndex) {
            value <<= 1;
            value |= 0x1;
        }
        masks.add(value);
        value >>= 1;
        masks.add(value);

        // add some shifts
        for (const item of shifts) {
            shifts.add(-1 * item);
            // shifts.add(-1 * Math.floor(item / 2));
            // shifts.add(Math.floor(item / 2));
        }

        for (let i = 0; i !== 0xFF; ++i) {
            shifts.add(i);
        }

        masks.add(0);
        masks.add(0xFF);
        masks.add(256 - dataLength);
        masks.add(dataLength);
        masks.add(zerosIndex);
        masks.add(127);
        masks.add(252);
        masks.add(254);

        shifts.add(0);
        shifts.add(0xFF);
        shifts.add(256 - dataLength);
        shifts.add(dataLength);

        this.shifts = Array.from(shifts).filter(val => (val & 0xFF) === val);
        this.masks = Array.from(masks).filter(val => (val & 0xFF) === val);
    }

    get mask() {
        return this.masks[Math.floor(this.index / this.shifts.length)];
    }

    get shift() {
        return this.shifts[this.index % this.shifts.length];
    }

    get length() {
        return this.masks.length * this.shifts.length;
    }

    get percent() {
        return this.index / this.length * 100;
    }

    next() {
        ++this.index;
    }
}

/// This shows how the actual algorithm will work.
const modifiers = {
    sizeof: uint16,
    resetMask: 0xFF,
    resetShift: 0x00,
    reset: 0x00FF, // both of them together
    maxShift: 0xFF,
    minShift: 0x00, // resetShift
    maxMask: 0xFF, // resetMask
    minMask: 0x00,
    end: 0xFF00, // end of the mask and shift
    max: 0xFFFF, // maxShift | maxMask

    /// only apply the mask
    applyMask: (pos, modifier) => pos & modifier,

    // applyPosition : (pos, modifier, table, range = 0) => table.at(range + (pos & modifier)),

    // applyShift : (value, modifier) => modifiers.fix(value + (modifier >>> 8)),
    unapplyShift: (value, pos, modifier) => {
        let shift = (modifier >>> 8);
        // const maskedPos = modifiers.applyMask(pos, modifier);
        // if (maskedPos === 0) {
        //     shift = 0;
        // }
        return modifiers.fix(value - shift);
    },
    // modify : (value, pos, modifier) => (pos & modifier) === 0 ? 0 : modifiers.fix(value - (modifier >>>
    // 8)),

    fix: value => value < 256 ? value : undefined,

    /// Apply the mask and the shift and finding the actual value in the second table
    /// This is the heart of the algorithm that in C++ we have to implement as well
    apply: (pos, modifier, table, range = 0) => {
        let shift = (modifier >>> 8) & 0xFF;
        const maskedPos = modifiers.applyMask(pos, modifier);
        // if (maskedPos === 0) {
        //     shift = 0;
        // }
        return modifiers.fix((table.at(range + maskedPos)) + shift);
    },
    // unapply : (pos, modifier, table, range = 0) =>
    //     modifier.fix((table.at(range + (pos & modifier)) - (modifier >>> 8))),

    // matches : (left, right, lstart, rstart, pos, modifier) => {
    //     const lccc = modifiers.unapply(pos, modifier, left, lstart);
    //     if (lccc === undefined || isNaN(lccc)) {
    //         return false;
    //     }
    //     return right.at(rstart + pos) === lccc;
    // },

    // matchesMask : (left, right, lstart, rstart, pos, modifier) => {
    //     const lccc = modifiers.applyPosition(pos, modifier, left, lstart);
    //     if (lccc === undefined || isNaN(lccc)) {
    //         return false;
    //     }
    //     return right.at(rstart + pos) === lccc;
    // },

    unshiftAll: (list, modifier) =>
        list.map((value, index) => modifiers.unapplyShift(value, index, modifier)),

    /// get the helper code
    helperCode: (pos, modifier) => (pos << 16) | modifier,

    maskOf: modifier => modifier & modifiers.resetMask,
    cccIndexOf: code => code >>> 16,
    shiftOf: modifier => modifier >>> 8,
    info: (modifier) => ({mask: modifiers.maskOf(modifier), shift: modifiers.shiftOf(modifier)}),
    compact: (mask, shift) => mask | (shift << 8),
};


const start = async () => {
    await readme.download();

    // database file
    const cccsTables = new CCCTables();
    await UnicodeData.parse(cccsTables, UnicodeData.properties.ccc);
    cccsTables?.finalize?.();
    await createTableFile([cccsTables]);
    console.log('File processing completed.');
};


class Modified {
    #data;
    #modifier;
    #start;

    constructor(data, codePointStart, modifier) {
        this.#data = data;
        this.#modifier = modifier;
        this.#start = codePointStart;
    }

    at(index) {
        if (index < 0 && index >= this.length) {
            throw new Error(`Index out of bounds ${index} out of ${this.length} elements.`);
        }

        const res = modifiers.apply(index, this.#modifier, this.#data, this.#start);
        if (res === undefined) {
            throw new InvalidModifier(this.#modifier);
        }
        return res;
    }

    get length() { return rangeLength(this.#start, this.#data.length); }

    slice(index = 0, endIndex = this.length - index) {
        let values = [];
        for (let pos = index; pos !== endIndex; ++pos) {
            values.push(this.at(pos));
        }
        return values;
    }

    * [ Symbol.iterator ]() {
        for (let pos = 0; pos !== this.length; pos++) {
            yield this.at(pos);
        }
    }

    filter(func) { return this.slice().filter(func); }
}

class CCCTables {
    constructor() {
        this.lastZero = 0;

        // these numbers are educated guesses from other projects, they're not that important!
        this.indeces = new TableTraits(4353 * 10, uint32);
        this.cccs = new TableTraits(65535, uint8);

        // this.cccs.append(0);

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

    /// Find the start position of "range" in "all"
    /// This function finds a place in the "this.cccs" table where the specified range
    /// will be there.
    #findSimilarRange(left, right) {
        top: for (let rpos = 0; rpos !== right.length; ++rpos) {
            for (let lpos = 0; lpos !== left.length; ++lpos) {
                const rvalue = right.at(rpos + lpos);
                const lvalue = left.at(lpos);
                if (rvalue !== lvalue) {
                    continue top;
                }
            }
            return rpos;
        }
        return null;
    }

    // Overlap Inserts Optimization:
    ///    if the "this.cccs" table's tail has a match for the beginning of the "range" table,
    ///    then we can omit inserting the first part of the "range" table.
    #overlapInserts(left, right) {
        if (left.length === 0) {
            return 0;
        }
        let rpos = Math.max(0, (right.length - 1) - left.length);
        top: for (; rpos !== right.length; ++rpos) {
            const length = right.length - rpos;
            for (let lpos = 0; lpos !== length; ++lpos) {
                const lvalue = left.at(lpos);
                const rvalue = right.at(rpos + lpos);
                if (lvalue !== rvalue) {
                    continue top;
                }
            }
            return length;
        }
        return 0;
    }

    /// This function compresses the specified range based on the input modifier.
    /// For example, an array of zeros, with mask of zero, only needs the first element
    #rightTrimInserts(inserts, modifier) {
        if (inserts.length <= 1) {
            return inserts.length;
        }
        let rtrimPos = 0;
        for (let pos = inserts.length - 1; pos >= 0; --pos) {
            if (modifiers.applyMask(pos, modifier) !== 0) {
                rtrimPos = pos;
                break;
            }
        }
        return Math.max(1, rtrimPos);
    }

    #optimizeInserts(inserts, dataView, modifier) {
        let pos = null;

        // inserts = this.#compressInserts(inserts, modifier);
        const modifiedInserts = new Modified(inserts, 0, modifier);

        // validating inserts:
        for (let index = 0; index !== inserts.length; ++index) {
            const realCCC = dataView.at(index);
            const insertCCC = modifiedInserts.at(index);
            if (realCCC !== insertCCC) {
                throw new InvalidModifier({index, realCCC, insertCCC});
            }
        }

        const overlapped = this.#overlapInserts(modifiedInserts, this.cccs);
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

    #findSimilarMaskedRange(codePointStart) {
        updateProgressBar(0);
        const computer = new ModifierComputer(this.data, codePointStart);
        const length = rangeLength(codePointStart, this.data.length);
        let possibilities = [];
        let invalidModifiers = [];
        for (; computer.index !== computer.length; computer.next()) {
            updateProgressBar(computer.percent);
            const modifier = modifiers.compact(computer.mask, computer.shift);
            const dataView =
                new Span(this.data, codePointStart, rangeLength(codePointStart, this.data.length));
            const modifiedCCC = new Modified(this.cccs, 0, modifier);
            let lastInfoLength = 0;
            let info = {};

            try {
                const startPos = this.#findSimilarRange(dataView, modifiedCCC);
                if (startPos === null) {
                    info = this.#optimizeInserts(dataView, dataView, modifier);
                } else {
                    info = {pos: startPos, inserts: new Span()};
                }

                possibilities.push({...info, modifier});

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
                info = this.#optimizeInserts(modifiers.unshiftAll(dataView), dataView, modifier);
                if (info.inserts.length < lastInfoLength) {
                    possibilities.push({...info, modifier, shifted: modifiers.shiftOf(modifier)});

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
        updateProgressBar(100);

        possibilities = possibilities.filter(item => item !== undefined);
        const codePointStartHex = codePointStart.toString(16);
        const codePointEndHex = (codePointStart + length).toString(16) || "infinite";
        console.log(`  0x${codePointStartHex}-0x${codePointEndHex}`, "modifiers-count:", computer.length,
            "invalid-modifiers:", invalidModifiers.length, "Possibilities:", possibilities.length,
            possibilities.slice(0, 5).map(item => ({...item, inserts: item.inserts.length})));
        if (possibilities.length === 0) {
            console.error(`  Empty possibilities:`, possibilities, this.cccs.length, this.data.length);
            console.error(`  Invalid Modifiers:`, invalidModifiers.length,
                invalidModifiers.map(item => item?.toString() || item));
            process.exit(1);
            // return {pos: null, inserts: this.data.slice(codePointStart, length), modifier:
            // modifiers.reset};
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
            const {mask, shift} = modifiers.info(modifier);
            pos = pos === null ? (mask === 0 ? 0 : this.cccs.index) : pos;
            const helperCode = modifiers.helperCode(pos, modifier);
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

            if (mask !== modifiers.resetMask && mask !== 0) {
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
    static constexpr std::array<std::${this.indeces.typeString}_t, ${indeces.length}ULL> ccc_index{
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
    static constexpr std::array<std::${this.cccs.typeString}_t, ${this.cccs.length}ULL> ccc_values{
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
