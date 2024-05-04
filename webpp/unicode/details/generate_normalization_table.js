/***
 * This file downloads UnicodeData.txt, and generates a C++ header file.
 *
 * Details on parsing this file can be found here:
 * UTS #44: https://www.unicode.org/reports/tr44/#UnicodeData.txt
 */

const fs = require('fs').promises;
const process = require('node:process');
const path = require('path');
const fileUrl = 'https://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt';
const readmeUrl = 'https://www.unicode.org/Public/UCD/latest/ucd/ReadMe.txt';
const cacheFilePath = 'UnicodeData.txt';
const cacheReadmePath = 'ReadMe.txt';
const outFilePath = `normalization_tables.hpp`;

const uint8 = Symbol('uint8');
const uint16 = Symbol('uint16');
const uint8x2 = Symbol('uint8');
const uint32 = Symbol('uint32');

class ModifierComputer {
    constructor(data, dataIndex = 0, dataLength = 256) {
        const codePoints = data.slice(dataIndex, dataIndex + dataLength).map(item => item.codePoint);
        this.index = 0;
        const masks = new Set(codePoints);
        masks.add(0);
        masks.add(0xFF);

        // add some masks
        for (let i = 0;; ++i) {
            const length = masks.size;
            if (i === length) {
                break;
            }
            for (let pos = i; pos !== length; ++pos) {
                masks.add(masks[i] | masks[i + pos]);
            }
        }

        const shifts = new Set(codePoints);
        shifts.add(0);
        shifts.add(0xFF);

        // add some shifts
        for (const item of shifts) {
            shifts.add(-1 * item);
            shifts.add(-1 * Math.floor(item / 2));
            shifts.add(Math.floor(item / 2));
        }
        this.shifts = Array.from(shifts);
        this.masks = Array.from(masks);
    }

    get mask() { return this.masks[Math.floor(this.index / this.shifts.length)]; }

    get shift() { return this.shifts[this.index % this.shifts.length]; }

    get length() { return this.masks.length * this.shifts.length; }

    get percent() { return this.index / this.length * 100; }

    next() { ++this.index; }
}

/// This shows how the actual algorithm will work.
const modifiers = {
    sizeof : uint16,
    resetMask : 0xFF,
    resetShift : 0x00,
    reset : 0x00FF, // both of them together
    maxShift : 0xFF,
    minShift : this.resetShift,
    maxMask : this.resetMask,
    minMask : 0x00,
    end : 0xFF00, // end of the mask and shift
    max : this.maxShift | this.maxMask,

    /// only apply the mask
    applyMask : (pos, modifier) => pos & modifier,

    /// Apply the mask and the shift and finding the actual value in the second table
    /// This is the heart of the algorithm that in C++ we have to implement as well
    apply : (pos, modifier, table, range = 0) => (table[range + pos] + (modifier >>> 8)) & modifier,

    /// get the helper code
    helperCode : (pos, modifier) => (pos << 16) | modifier,

    maskOf : modifier => modifier & modifiers.resetMask,
    shiftOf : modifier => modifier >>> 8,
    info : (modifier) => ({mask : modifiers.maskOf(modifier), shift : modifiers.shiftOf(modifier)}),
    compact : (mask, shift) => mask | (shift << 8),

    // nextModifier : modifier => {
    //     let {mask, shift} = modifiers.info(modifier);
    //     ++shift;
    //     if (shift >= modifiers.maxShift) {
    //         shift = modifiers.resetShift;
    //         --mask;
    //     }
    //     if (mask >= modifiers.maxMask) {
    //         mask = modifiers.resetMask;
    //     }
    //     return modifiers.compact(mask, shift);
    // },

    // percent : modifier =>
    //     ((modifiers.maxMask - modifiers.maskOf(modifier)) | (modifiers.shiftOf(modifier) << 8)) /
    //     modifiers.max * 100,
};

const readmeData = {
    version : "",
    date : ""
};

const progressBarLength = 30; // Define the length of the progress bar
const totalItems = 100;       // Total number of items to process
let lastPercent = 0;
const updateProgressBar = (percent, done = undefined) => {
    if (!process.stdout.isTTY || percent === lastPercent) {
        return;
    }
    // process.stdout.clearLine();
    process.stdout.cursorTo(0); // Move the cursor to the beginning of the line
    if (percent >= totalItems) {
        if (done) {
            console.log(done instanceof Function ? done() : done);
        }
        return;
    }
    const progress = Math.round((percent / totalItems) * progressBarLength);
    const progressBar = '='.repeat(progress) + '>' +
                        '-'.repeat(progressBarLength - progress);
    process.stdout.write(
        `[${progressBar}] ${Math.round((percent / totalItems) * 100)}%`); // Update the progress bar
};
const downloadFile = async (url, file, process) => {
    try {
        // Check if the file already exists in the cache
        await fs.access(file);
        try {
            console.log(`Using cached file ${file}...`);
            const fileContent = await fs.readFile(file);
            process(fileContent.toString());
            return;
        } catch (error) {
            console.error(error);
            return;
        }
    } catch (error) {
        console.log("No cached file exists, let's download it.");
    }

    try {
        const response = await fetch(url);

        if (!response.ok) {
            console.error(`Failed to download file. Status Code: ${response.status}`);
            return;
        }

        const text = await response.text();

        // Save the downloaded file as a cache
        await fs.writeFile(file, text);
        console.log(`Downloaded ${file} from ${url}.`);

        // process the file
        await process(text);
    } catch (error) {
        console.error('Error:', error.message);
    }
};

const start = async () => {
    // readme file for getting the version and what not
    await downloadFile(readmeUrl, cacheReadmePath, processReadmeFile);
    if (readmeData.version === undefined) {
        console.error("Could not find the version from the file content.");
        return;
    }
    if (readmeData.date === undefined) {
        console.error("No date was found.");
        return;
    }
    console.log(`Version:       ${readmeData.version}`);
    console.log(`Creation Date: ${readmeData.date}`);

    // database file
    await downloadFile(fileUrl, cacheFilePath, processCachedFile);
};

const processReadmeFile = content => {
    readmeData.version = findVersion(content);
    readmeData.date = findDate(content);
};

const cleanComments = line => line.split('#')[0].trimEnd()
const splitLine = line => line.split(';').map(seg => seg.trim());
const findVersion = fileContent => fileContent.match(/Version:? (\d+\.\d+\.\d+)/)[1];
const findDate = fileContent => fileContent.match(/Date: ([^\n\r]+)/)[1];
const parseCodePoints = codePoint => parseInt(codePoint, 16);

/**
 * This class will let us handle the types of the tables including:
 *   - unsigned integer 32 bit (uint32)
 *   - unsigned integer 8 bit  (uint8)
 *   - unsigned integer 16 bit (uint16)
 */
class TableTraits {

    constructor(max, type = uint8) {
        switch (type) {
        case uint8:
            this.bytes = new Uint8Array(max);
            break;
        case uint32:
            this.bytes = new Uint32Array(max);
            break;
        case uint8x2:
            this.bytes = [];
            break;
        default:
            throw new Error('Invalid type provided to CodePointMapper.');
        }
        this.type = type;
        this.index = 0;
    }

    get sizeof() {
        switch (this.type) {
        case uint8x2:
            return 16;
        case uint8:
            return 8;
        case uint32:
            return 32;
        default:
            return 0;
        }
    }

    get typeString() { return this.type.description; }

    get postfix() {
        switch (this.type) {
        case uint8x2:
        case uint8:
            return "U";
        case uint32:
            return "ULL";
        default:
            return "";
        }
    }

    get length() { return this.index; }

    get result() { return this.bytes.slice(0, this.length); }

    get(index) { return this.bytes[index]; }

    set(index, value) { return this.bytes[index] = value; }

    append(value) { this.bytes[this.index++] = value; }

    appendList(list) {
        for (const value of list) {
            this.append(value);
        }
    }
}

class CCCTables {
    constructor() {
        this.lastZero = 0;

        // these numbers are educated guesses from other projects, they're not that important!
        this.indeces = new TableTraits(4353 * 10, uint32);
        this.cccs = new TableTraits(65535, uint8);

        this.data = [];
    }

    add(codePoint, ccc) {

        // fill the data
        this.data.push({codePoint, ccc});

        // calculating the last item that it's ccc is zero
        if (ccc !== 0) {
            this.lastZero = codePoint + 1;
        }
    }

    /// Post-Processing
    finalize() {
        console.time("Finalize");
        console.log("Finalizing...");

        /// Find the start position of "range" in "all"
        const findSubRange = (left, right, callable = (range_value, all_value) => range_value === all_value,
                              start = 0, length = left.length) => {
            const end = (right.length / length) * length;
            top: for (let rpos = 0; rpos < end; ++rpos) {                 // iterate over "right" array
                for (let lpos = start; lpos < (start + length); ++lpos) { // iterate over "left" array
                    if (!callable({
                            lvalue : left?.[lpos],         // left table current value
                            rvalue : right?.[rpos + lpos], // right table current value
                            rpos,                          // right index
                            lpos                           // left index
                        })) {
                        continue top;
                    }
                }
                return rpos;
            }
            return null;
        };

        // Overlap Inserts Optimization:
        ///    if the "this.cccs" table's tail has a match for the beginning of the "range" table,
        ///    then we can omit inserting the first part of the "range" table.
        const overlapInserts = (left, right, equalCondition) => {
            if (left.length === 0) {
                return 0;
            }
            let rpos = (right.length - 1) - left.length;
            top: for (; rpos !== right.length; ++rpos) {
                const length = right.length - rpos;
                for (let lpos = 0; lpos !== length; ++lpos) {
                    if (!equalCondition({lpos, rpos})) {
                        continue top;
                    }
                }
                return length;
            }
            return 0;
        };

        /// This function finds a place in the "this.cccs" table where the specified range
        /// will be there.
        const findSimilarRange = (equalCondition, index = 0, length = 256, modifier = modifiers.reset) => {
            const pos = findSubRange(this.data, this.cccs, equalCondition, index, length);

            // found a sub-range, no need for inserts
            if (pos !== null) {
                return {valid : true, pos, inserts : []};
            }

            // didn't find anything, so let's insert everything:
            const inserts = this.data.slice(index, index + length);
            let isValidModifier = true;
            for (let pos = 0; pos !== inserts.length; ++pos) {
                const oldccc = inserts[pos]?.ccc;
                const newccc = modifiers.apply(pos, modifier, inserts)?.ccc;
                if (oldccc !== newccc || oldccc === undefined) {
                    isValidModifier = false;
                    break;
                }
            }
            return {valid : isValidModifier, pos : null, inserts};
        };

        /// This function compresses the specified range based on the input modifier.
        /// For example, an array of zeros, with mask of zero, only needs the first element
        const compressInserts = (inserts, modifier = modifiers.reset) => {
            if (inserts.length <= 1 || inserts[0].ccc !== 0) {
                return inserts;
            }
            let rtrimPos = 0;
            for (let pos = inserts.length - 1; pos !== 0; --pos) {
                if (modifiers.applyMask(pos, modifier) !== 0) {
                    rtrimPos = pos;
                    break;
                }
            }
            // let ltrimPos = 0;
            // for (let pos = 0; pos !== rtrimPos; ++pos) {
            //     if (applyMask(pos, mask) !== 0) {
            //         ltrimPos = pos;
            //         break;
            //     }
            // }
            // const trimLength = ltrimPos + (inserts.length - rtrimPos);
            // if (trimLength !== 0) {
            //     console.log(`  Trim Inserts:`, trimLength, ltrimPos, rtrimPos);
            // }
            return inserts.slice(0, rtrimPos + 1);
        };

        const findSimilarMaskedRange = (index = 0, length = 256) => {
            updateProgressBar(0);
            const computer = new ModifierComputer(this.data, index, length);
            let possibilities = [];
            for (; computer.index !== computer.length; computer.next()) {
                updateProgressBar(computer.percent);
                const modifier = modifiers.compact(computer.mask, computer.shift);
                // console.log(modifier, computer.mask, computer.shift);

                const equalCondition = ({rpos, lpos}) => {
                    const ccc = modifiers.apply(lpos, modifier, this.cccs, rpos);
                    return this.data[lpos].ccc === ccc;
                };

                let {valid, inserts, pos} = findSimilarRange(equalCondition, index, length, modifier);
                if (!valid) {
                    continue;
                }
                inserts = compressInserts(inserts, modifier);
                let overlapped = overlapInserts(inserts, this.cccs, ({rpos, lpos}) => {
                    const ccc = modifiers.apply(rpos, modifier, inserts)?.ccc;
                    if (ccc === undefined) {
                        return false;
                    }
                    return ccc === this.cccs[rpos + lpos];
                    // const lx = applyMask(lpos, mask);
                    // const rx = rpos + lx;
                    // const lxccc = inserts[lx]?.ccc;
                    // if (rx < 0 || lxccc === undefined) {
                    //     return false;
                    // }
                    // return lxccc === this.cccs.get(rx);
                });
                pos = this.cccs.length - overlapped;
                inserts = inserts.slice(overlapped, inserts.length);
                possibilities[inserts.length] = {pos, modifier, inserts, overlap : overlapped};
            }
            updateProgressBar(100);

            const codePointStart = this.data[index].codePoint.toString(16);
            const codePointEnd = this.data[index + length]?.codePoint?.toString(16) || "infinite";
            console.log(`  0x${codePointStart}-0x${codePointEnd}`, "modifiers-count:", computer.length,
                        "Possibilities:",
                        possibilities.filter(item => item !== undefined)
                            .slice(0, 5)
                            .map(item => ({...item, inserts : item.inserts.length})));
            return possibilities.find(item => item !== undefined) ||
                   {pos : null, modifier : modifiers.reset, inserts : this.data.slice(index, index + length)};
        };

        let batchNo = 0;
        let insertedCount = 0;
        let reusedCount = 0;
        let reusedMaskedCount = 0;
        for (let range = 0; range < this.data.length; range += 256) {

            const codeRange = range >>> 8;
            const length = Math.min(this.data.length - range, 256);

            console.log(`Batch #${batchNo++}: `, codeRange.toString(16), "CCC-Length:", this.cccs.length,
                        `Progress: ${Math.floor(range / this.data.length * 100)}%`);

            let {pos, modifier, inserts} = findSimilarMaskedRange(range, length);
            pos = pos === null ? this.cccs.index : pos;
            const helperCode = modifiers.helperCode(pos, modifier);
            this.indeces.append(helperCode);
            if (inserts.length > 0) {
                inserts = inserts.map(item => item?.ccc);
                this.cccs.appendList(inserts);
                ++insertedCount;
            } else {
                ++reusedCount;
            }
            const mask = modifiers.maskOf(modifier);
            const shift = modifiers.shiftOf(modifier);
            console.log(`  Code Range (${inserts.length ? "Inserted-" + inserts.length : "Reused"}):`,
                        codeRange, "mask:", mask, "shift:", shift, "pos:", pos, 'code:', helperCode,
                        "samples:", inserts.filter(item => item).slice(0, 5));

            if (mask !== modifiers.resetMask && mask !== 0) {
                ++reusedMaskedCount;
            }
        }

        console.log("Inserted: ", insertedCount, "reused:", reusedCount);
        console.log("Successful masks:", reusedMaskedCount);
        console.log("CCCs Table Length:", this.cccs.length);
        console.log("Finalizing: done.");
        console.timeEnd("Finalize");
    }

    render() {

        const indeces = this.indeces.result.slice(0, this.lastZero >>> 8);
        const indecesBits = indeces.length * this.indeces.sizeof;
        const cccBits = this.cccs.length * this.cccs.sizeof;

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
     * Usage:
     * @code
     *    uint32_t code_point = ...;
     *    auto helper_code = ccc_index[code_point >> 8];
     *    auto rem_code    = code_point  &  0xFF;              // remaining part of the code point
     *    auto pos         = helper_code >> 8;                 // starting position
     *    auto mask        = helper_code &  0xFF;              // which positions should be converted to zero
     *    auto index       = pos         +  (mask & rem_code); // now use ccc_values[index]
     * @endcode
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
        ${this.cccs.result.join(", ")}
    };
        `;
    }
}

const processCachedFile = async fileContent => {
    const lines = fileContent.split('\n');
    const cccsTables = new CCCTables();
    let lastCodePoint = 0;

    lines.forEach((line, index) => {
        line = cleanComments(line);

        // ignore empty lines
        if (line.length === 0) {
            return "";
        }

        const [codePointStr, codePointName, GeneralCategory, CanonicalCombiningClass, BidiClass,
               DecompositionType,
               // DecompositionMapping,
               NumericType,
               // NumericValue,
               BidiMirrored, Unicode1Name, ISOComment, SimpleUppercaseMapping, SimpleLowercaseMapping,
               SimpleTitlecaseMapping] = splitLine(line);
        const codePoint = parseCodePoints(codePointStr);
        const ccc = parseInt(CanonicalCombiningClass);

        /// stats
        // if (count % 1000 === 0) {
        //     console.log(`Parsing Line ${index}: `, `codePoint(${codePoint})`, `count(${count})`);
        // }
        updateProgressBar(index / lines.length * 100);

        for (let curCodePoint = lastCodePoint + 1; curCodePoint !== codePoint + 1; ++curCodePoint) {
            const curCCC = curCodePoint === codePoint ? ccc : 0;

            cccsTables.add(curCodePoint, curCCC);
        }
        lastCodePoint = codePoint;
    });
    updateProgressBar(100, `Lines parsed: ${lines.length}`);
    cccsTables.finalize?.();
    await createTableFile([ cccsTables ]);
    console.log('File processing completed.');
};

const createTableFile = async (tables) => {
    const begContent = `
/**
 * Attention: Auto-generated file, don't modify.
 * 
 *   Auto generated from:                ${path.basename(__filename)}
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
 *       ${fileUrl}
 *   UCD README file (used to check the version and creation date):
 *       ${readmeUrl}
 */
 
#ifndef WEBPP_UNICODE_NORMALIZATION_TABLES_HPP
#define WEBPP_UNICODE_NORMALIZATION_TABLES_HPP

#include <array>
#include <cstdint>

namespace webpp::unicode::details {

`;

    const endContent = `
} // webpp::unicode::details

#endif // WEBPP_UNICODE_NORMALIZATION_TABLES_HPP
    `;

    await fs.writeFile(outFilePath, begContent);
    for (const table of tables) {
        await fs.appendFile(outFilePath, table.render());
    }
    // console.log(`  Trailings Removed: ${trailingsRemoved} Bytes`);
    // console.log(`  Trailings Removed: ${Math.ceil(trailingsRemoved / 1024)} KiB`);
    await fs.appendFile(outFilePath, endContent);

    // Reformat the file
    require('child_process').exec(`clang-format -i "${outFilePath}"`, err => {
        if (err) {
            console.error("Could not re-format the file.", err);
        } else {
            console.log("Clang-format completed.");
        }
    });
};

start();
