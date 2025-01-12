/**
 * This file downloads IdnaMappingTable.txt, and generates a C++ header file.
 *
 * Details on parsing this file can be found here:
 * UTS #46: https://www.unicode.org/reports/tr46/#IDNA_Mapping_Table
 */

import {
    downloadFile,
    splitLine,
    findVersion,
    findDate,
    cleanComments,
    TableTraits,
    uint16,
    char8_8,
    utf32To8All,
    runClangFormat,
    writePieces,
    renderTableValues,
    parseCodePointRangeExclusive,
    uint8,
    char8,
    sizeOf,
    findSimilarRange
} from "../../../unicode/details/utils.mjs";

import * as path from "node:path";

const fileUrl = 'https://www.unicode.org/Public/idna/latest/IdnaMappingTable.txt';
const cacheFilePath = 'IdnaMappingTable.txt';
const outFilePath = `idna_mapping_table.hpp`;


const start = async () => {
    await downloadFile(fileUrl, cacheFilePath, processCachedFile);
};

const parseMappedCodePoints = codePoints => codePoints.split(" ").map(codePoint => parseInt(codePoint, 16));

const MAPPED = 0b000 << 13;
const NOT_MAPPED = 0b100 << 13;
const VALID = NOT_MAPPED | 0b001;
const DISALLOWED = NOT_MAPPED | 0b010;

const isMapped = (flags) => flags < NOT_MAPPED;
const isValid = (flags) => flags === VALID;
const isDisallowed = (flags) => flags === DISALLOWED;
const flagsStatus = (flags) => {
    switch (flags) {
        case VALID: return "<Valid>";
        case DISALLOWED: return "<Disallowed>";
        default:
            return isMapped(flags) ? `<Mapped:${flags}>` : `<invalid:${flags.toString(16)}>`;
    }
};
const flagsOr = (flags, pos) => flags | (isMapped(flags) ? pos : 0);


class MappingTable {

    #refs = []; // items in this table points to the #refBlocks
    #refBlocks = []; // items in this table stores flags and points to the #maps
    #maps = []; // the mapped code points that the other table point to

    // The last code point that after that everything is disallowed
    #lastDisallowed = 0n;

    #rawMaps = [];

    // the modulus value
    #magicRem = 1n;


    #batchBitCount;
    #batchSize;

    constructor(max) {
        this.#refs = [];
        this.#refBlocks = [];
        this.#maps = [];

        this.#refs.type = uint16;
        this.#refBlocks.type = uint16;
        this.#maps.type = char8;

        this.#refs.sizeof = sizeOf(this.#refs.type);
        this.#refBlocks.sizeof = sizeOf(this.#refBlocks.type);
        this.#maps.sizeof = sizeOf(this.#maps.type);

        this.#batchBitCount = 8n;
        this.#batchSize = 0b1 << Number(this.#batchBitCount);
        //     this.#bitLength = Number(this.#refs.sizeof);
    }

    append(start, end, flags, mappedTo = []) {
        // console.assert((mappedTo?.length || 1) > 0 && !isMapped(flags), `Flags don't match the other inputs.`, start, end, flags, mappedTo);

        for (; start <= end; ++start) {
            const raw = {
                codePoint: BigInt(start),
                flags,
                mappedTo,
                utf8MappedTo: utf32To8All(mappedTo)
            };

            // calculating the last disallowed code point
            if (!isDisallowed(flags)) {
                this.#lastDisallowed = raw.codePoint + 1n;
            }

            // console.log(raw)
            this.#rawMaps.push(raw);
        }
    }

    /// Calculate the length of utf8-encoded code points from the specified range
    #calcLen(start, end) {
        let maxLen = 0;
        let sumLen = 0;
        const length = end - start;
        for (let i = start; i < end; ++i) {
            const { utf8MappedTo } = this.#rawMaps[i];
            if (utf8MappedTo.length > maxLen) {
                maxLen = utf8MappedTo.length;
            }
            sumLen = utf8MappedTo.length;
        }
        return { sumLen, maxLen, requiredLen: maxLen * length };
    };

    /// check if the mapping for both code point indexes specified are the same
    #isSameMapping(lhsIndex, rhsIndex) {
        if (lhsIndex === rhsIndex) {
            return true;
        }
        const lhs = this.#rawMaps[lhsIndex];
        const rhs = this.#rawMaps[rhsIndex];
        return rhs?.flags === lhs?.flags && lhs.mappedTo.every((el, idx) => el === rhs.mappedTo.at(idx));
    }

    #findSimilarMappedTo(index) {
        const { utf8MappedTo, flags } = this.#rawMaps.at(index);
        if (isMapped(flags)) {
            let localUtf8MappedTo = utf8MappedTo.slice();
            localUtf8MappedTo.push(0); // add the last EOF '\0'
            return findSimilarRange(localUtf8MappedTo, this.#maps);
        }
        return null;
    }

    /// Find the specified range from source table in target table
    #findOrInsertBlock(start, length) {
        length = length <= (this.#rawMaps.length - start) ? length : (this.#rawMaps.length - start);
        const end = start + length;
        let targetIndex = 0;
        let pos = start;
        nextTarget: for (; ; ++targetIndex) {
            for (pos = start; pos != end; ++pos) {
                const index = pos - start;
                const block = this.#refBlocks.at(targetIndex);
                if (!block) {
                    pos = end;
                    break;
                }
                const { blockStart } = block;
                if (!this.#isSameMapping(blockStart + index, pos)) { // todo
                    continue nextTarget;
                }
            }
            break;
        }
        if (pos === end) { // we didn't find it, let's insert it then
            const statuses = [];
            for (let i = start; i !== end; ++i) {
                const { codePoint, flags, utf8MappedTo } = this.#rawMaps.at(i);

                // insert the utf-8 encoded stuff to the table
                let mapsPosition = this.#findSimilarMappedTo(i);
                if (mapsPosition === null) {
                    mapsPosition = this.#maps.length;
                    this.#maps[mapsPosition] = [];
                    this.#maps[mapsPosition].comment = `#${mapsPosition}: \\x${codePoint.toString(16)}`;
                    for (const codeUnit of utf8MappedTo) {
                        this.#maps[mapsPosition].push(codeUnit);
                    }
                    this.#maps[mapsPosition].push(0); // the last EOF '\0' character
                }


                statuses.push(flagsOr(flags, mapsPosition));
            }
            this.#refBlocks[targetIndex] = {
                rawStart: start,
                rawEnd: end,
                statuses
            };
        }
        return targetIndex;
    }

    // calculate the position value for the specified range (it's for the refs table)
    // #calcBatch(start, end) {
    //     let pos = (0b1n << BigInt(this.#bitLength)) - 1n;
    //     const { sumLen, maxLen, requiredLen } = this.#calcLen(start, end);
    //     console.log(sumLen, maxLen, requiredLen, end - start);
    //     let utf8Values = new Array(requiredLen).fill(0);
    //     for (let i = start; i < end; ++i) {
    //         const { flags, utf8MappedTo } = this.#rawMaps[i];
    //         const index = i - start;
    //         const bitIndex = BigInt(Math.floor(index % this.#bitLength));
    //         if (bitIndex > this.#bitLength) {
    //             console.log(bitIndex, index, this.#bitLength, index / this.#bitLength)
    //             throw new Error(`Can't hold ${bitIndex} in ${this.#bitLength} length`);
    //         }
    //         // const bit = (pos >>> (bitIndex - 1)) & 0b1;
    //         const bitMask = 0b1n << bitIndex;
    //         if (!isValid(flags)) {
    //             pos &= ~bitMask;
    //             for (let j = 0; j !== utf8MappedTo.length; ++j) {
    //                 utf8Values[j] = utf8MappedTo[j]; // todo
    //             }
    //         }
    //     }
    //
    //     if (pos >= (0b1 << this.#bitLength)) {
    //         throw new Error(`We don't have the enough bits to store ${pos}; (${start}-${end})`);
    //     }
    //     return { pos, utf8Values };
    // }

    process() {
        // Removing everything that's disallowed after the last one:
        const startLen = this.#rawMaps.length;
        this.#rawMaps = this.#rawMaps.filter(({codePoint}) => codePoint >= this.#lastDisallowed);
        const endLen = this.#rawMaps.length;
        console.log("Removed trailing disallowed mappings:", startLen - endLen);

        for (let batchIndex = 0; batchIndex < this.#rawMaps.length; batchIndex += this.#batchSize) {
            const { codePoint } = this.#rawMaps[batchIndex];
            const tbl1Loc = Number(codePoint) >>> Number(this.#batchBitCount);
            // const { pos, utf8Values } = this.#calcBatch(i, i + this.#batchSize);

            // if (!this.#maps.isAll(Number(pos), utf8Values.length, 0)) {
            //     throw new Error(`Replacing is happening: ${pos}+${utf8Values.length}`);
            // }

            this.#refs[tbl1Loc] = {
                blockPtr: this.#findOrInsertBlock(batchIndex, this.#batchSize)
            };
            // if (pos !== ((0b1n << BigInt(this.#bitLength)) - 1n)) { // contains mapped or disallowed values
            //     console.log(pos, utf8Values);
            //     this.#maps.setAt(Number(pos), utf8Values);
            // }
        }
    }

    // process() {
    //     const cutSize = 8n;
    //     const batchSize = 0b1 << Number(cutSize);
    //     const bitLength = Number(this.#refs.sizeof);
    //
    //     // calculate the position value for the specified range (it's for the refs table)
    //     const calcBatch = (start, end) => {
    //         let pos = (0b1n << BigInt(bitLength)) - 1n;
    //         const { sumLen, maxLen, requiredLen } = this.#calcLen(start, end);
    //         console.log(sumLen, maxLen, requiredLen, end - start);
    //         let utf8Values = new Array(requiredLen).fill(0);
    //         for (let i = start; i < end; ++i) {
    //             const { flags, utf8MappedTo } = this.#rawMaps[i];
    //             const index = i - start;
    //             const bitIndex = BigInt(Math.floor(index % bitLength));
    //             if (bitIndex > bitLength) {
    //                 console.log(bitIndex, index, bitLength, index / bitLength)
    //                 throw new Error(`Can't hold ${bitIndex} in ${bitLength} length`);
    //             }
    //             // const bit = (pos >>> (bitIndex - 1)) & 0b1;
    //             const bitMask = 0b1n << bitIndex;
    //             if (!isValid(flags)) {
    //                 pos &= ~bitMask;
    //                 for (let j = 0; j !== utf8MappedTo.length; ++j) {
    //                     utf8Values[j] = utf8MappedTo[j]; // todo
    //                 }
    //             }
    //         }
    //
    //         if (pos >= (0b1 << bitLength)) {
    //             throw new Error(`We don't have the enough bits to store ${pos}; (${start}-${end})`);
    //         }
    //         return { pos, utf8Values };
    //     }
    //
    //     for (let i = 0; i < this.#rawMaps.length; i += batchSize) {
    //         const { codePoint } = this.#rawMaps[i];
    //         const tbl1Loc = Number(codePoint) >>> Number(cutSize);
    //         const { pos, utf8Values } = calcBatch(i, i + batchSize);
    //
    //         if (!this.#maps.isAll(Number(pos), utf8Values.length, 0)) {
    //             throw new Error(`Replacing is happening: ${pos}+${utf8Values.length}`);
    //         }
    //
    //         this.#refs.set(tbl1Loc, Number(pos));
    //         if (pos !== ((0b1n << BigInt(bitLength)) - 1n)) { // contains mapped or disallowed values
    //             console.log(pos, utf8Values);
    //             this.#maps.setAt(Number(pos), utf8Values);
    //         }
    //     }
    // }

    // calculate() {
    //     const invalidFlag = 0xFFFF;
    //     let tryNum = 0;
    //     this.#magicRem = BigInt(Math.floor(this.#rawMaps.length / 1000));
    //     const nextAttempt = (...info) => {
    //         console.log(`Attempt #${tryNum} with magic rem of ${this.#magicRem} failed.`, ...info);
    //         // this.#refs.clear(invalidFlag);
    //         // this.#maps.clear();
    //         ++this.#magicRem;
    //         ++tryNum;
    //     };
    //
    //     retry: for (; ;) {
    //         let refs = [];
    //         let maps = [];
    //
    //         let pos = 0;
    //         for (let i = 0; i !== this.#rawMaps.length; ++i) {
    //             const { codePoint, flags, utf8MappedTo } = this.#rawMaps[i];
    //             const loc = codePoint % this.#magicRem;
    //             const val = isMapped(flags) ? flags | pos : flags;
    //             const mag = refs?.[loc] || invalidFlag;
    //             // console.log(codePoint, loc, this.#magicRem, mag, invalidFlag, flagsStatus(mag), this.#refs)
    //             if (mag !== val && mag !== invalidFlag) {
    //                 nextAttempt(codePoint, loc, val, flagsStatus(mag));
    //                 continue retry;
    //             }
    //
    //             refs[loc] = val;
    //
    //             pos += utf8MappedTo.length + 1;
    //
    //             // Add the UTF-8 encoded mapped to code points:
    //             // for (const cu of utf8MappedTo) {
    //             //     this.#maps.push(cu);
    //             // }
    //             // this.#maps.push(0);
    //         }
    //
    //         for (let pos = 0; pos !== refs.length; ++pos) {
    //             const value = refs.at(pos) || invalidFlag;
    //             this.#refs.set(pos, value);
    //         }
    //         for (let pos = 0; pos !== refs.length; ++pos) {
    //             const value = maps.at(pos) || invalidFlag;
    //             this.#maps.set(pos, value);
    //         }
    //
    //         break;
    //     }
    //     console.log(`Success on #${tryNum}th try with magic rem of ${this.#magicRem}`);
    // }

    serializeTable(table) {
        let res = "";
        for (let pos = 0; pos !== table.length;) {
            const val = table.at(pos);
            if (val === undefined) {
                throw new Error(`value: ${val}, pos: ${pos}, length: ${table.length}`);
            }
            res += `0x${val.toString(16)}${table.postfix}, `;
            ++pos;
            if (pos % 20 === 0) {
                res += '\n';
            }
        }
        return res;
    }

    render(version, creationDate) {
        const refsBitLength = this.#refs.length * Number(this.#refs.sizeof);
        const blockBitLength = this.#refBlocks.length * Number(this.#refBlocks.sizeof);
        const mapsBitLength = this.#maps.length * Number(this.#maps.sizeof);
        const sumBitLength = refsBitLength + blockBitLength + mapsBitLength;
        console.log(`Reference Table size:`);
        console.log(`  Count       : ${this.#refs.length} * ${this.#refs.sizeof}`);
        console.log(`  in bytes    : ${Math.ceil(refsBitLength / 8)},`);
        console.log(`  in KibiBytes: ${Math.ceil(refsBitLength / 8 / 1024)} KiB\n`);
        console.log(`Ref Blocks Table size:`);
        console.log(`  Count       : ${this.#refBlocks.length} * ${this.#refBlocks.sizeof}`);
        console.log(`  in bytes    : ${Math.ceil(blockBitLength / 8)},`);
        console.log(`  in KibiBytes: ${Math.ceil(blockBitLength / 8 / 1024)} KiB\n`);
        console.log(`Map Table size:`);
        console.log(`  Count       : ${this.#maps.length} * ${this.#maps.sizeof}`);
        console.log(`  in bytes    : ${Math.ceil(mapsBitLength / 8)},`);
        console.log(`  in KibiBytes: ${Math.ceil(mapsBitLength / 8 / 1024)} KiB\n`);
        console.log(`Total Table size:`);
        console.log(`  in bytes    : ${Math.ceil(sumBitLength / 8)},`);
        console.log(`  in KibiBytes: ${Math.ceil(sumBitLength / 8 / 1024)} KiB\n`);
        console.log(`Last Disallowed Code Point: ${this.#lastDisallowed}\n`);

        return `
/**
 * Attention: Auto-generated file, don't modify.
 * 
 *   Auto generated from:          ${path.basename(new URL(import.meta.url).pathname)}
 *   IDNA Creation Date:           ${creationDate}
 *   This file's generation date:  ${new Date().toUTCString()}
 *   IDNA Mapping Table Version:   ${version}
 *   Size:                         ${sumBitLength / 8} B
 *                                 ${sumBitLength / 8 / 1024} KiB
 *
 * Details about the contents of this file can be found here:
 *   UTS #46: https://www.unicode.org/reports/tr46/#IDNA_Mapping_Table
 *   IDN FAQ: https://www.unicode.org/faq/idn.html
 */
 
#ifndef WEBPP_URI_IDNA_MAPPING_TABLE_HPP
#define WEBPP_URI_IDNA_MAPPING_TABLE_HPP

#include <array>
#include <cstdint>
#include <string_view>

namespace webpp::uri::idna::details {

    static constexpr std::uint16_t magic_rem = ${this.#magicRem}U;
    static constexpr char32_t last_diallowed = U'\\x${this.#lastDisallowed.toString(16)}';

    struct idna_ref_unit {
        std::uint16_t block_index = 0U;
    };

    /**
     * IDNA Reference Table
     * 
     * Table size:
     *   - in bytes:      ${refsBitLength / 8} B
     *   - in KibiBytes:  ${Math.ceil(refsBitLength / 8 / 1024)} KiB
     */
    static constexpr std::array<idna_ref_unit, ${this.#refs.length}ULL> idna_refs {
       ${this.#refs.map(({blockPtr}) => `0x${(blockPtr || 0).toString(16)}`).join(", ")}
    };
    

    /**
     * IDNA Reference Blocks Table
     * 
     * Table size:
     *   - in bytes:      ${blockBitLength / 8} B
     *   - in KibiBytes:  ${Math.ceil(blockBitLength / 8 / 1024)} KiB
     */
    static constexpr std::array<${this.#refBlocks.typeString}, ${this.#refBlocks.length}ULL> idna_ref_blocks {
       ${this.#refBlocks.map((block, blkIndex) => `
           // Block #${blkIndex}
           { ${block.statuses.map(flags => `0x${(flags || 0).toString(16)}`).join(", ")} }
       `).join(", ")}
    };
    
    /**
     * IDNA Mapped Code Points Table
     * 
     * Table size:
     *   - in bytes:      ${mapsBitLength / 8} B
     *   - in KibiBytes:  ${Math.ceil(mapsBitLength / 8 / 1024)} KiB
     */
    ${renderTableValues({
            name: "idna_mappings",
            type: this.#maps.type,
            printableValues: this.#maps,
            len: this.#maps.length,
        })}
    
    
} // webpp::uri::idna::details

#endif // WEBPP_URI_IDNA_MAPPING_TABLE_HPP
  `;
    }
}


const processCachedFile = async fileContent => {
    const lines = fileContent.split('\n');
    const version = findVersion(fileContent);
    const creationDate = findDate(fileContent);
    console.assert(version !== undefined, "Could not find the version from the file content.");
    console.assert(creationDate !== undefined, "No date was found.");
    console.log(`Version: ${version}`);
    console.log(`Creation Date: ${creationDate}`);

    const tables = new MappingTable(1114111 + 1);
    let maxMappedCount = 0;
    let cpSum = 0n;
    lines.forEach((line, index) => {
        line = cleanComments(line)

        // ignore empty lines
        if (line.length === 0) {
            return "";
        }

        const [codePoints, status, mapping, IDNA2008Status] = splitLine(line);
        const [rangeStart, rangeEnd] = parseCodePointRangeExclusive(codePoints);
        const mappedValues = mapping ? parseMappedCodePoints(mapping) : undefined;

        let flags = 0;
        switch (status) {
            case 'deviation': // https://www.unicode.org/reports/tr46/#Deviations
            // Deviations are considered valid in IDNA2008 and UTS #46.
            case 'valid':
                flags |= VALID;
                break;
            case 'mapped':
                flags |= MAPPED;
                break;
            case 'ignored':
                flags |= MAPPED;
                break;
            case 'disallowed':
                flags |= DISALLOWED;
                break;
            default:
                console.error(`Invalid 'status' found: ${status}; line: ${line}`);
                process.exit(1);
        }
        tables.append(rangeStart, rangeEnd, flags, mappedValues);

        if (mappedValues?.length > maxMappedCount) {
            maxMappedCount = mappedValues.length;
        }
        cpSum += rangeEnd - rangeStart + 1n;

        console.log(`${index}/${cpSum}:`, rangeStart, rangeEnd, status, mappedValues || "", IDNA2008Status || "");
    });


    console.log("Max Mapped Count: ", maxMappedCount);
    // tables.calculate();
    tables.process();

    await writePieces(outFilePath, [tables.render(version, creationDate)]);

    // Reformat the file
    await runClangFormat(outFilePath);

    console.log('File processing completed.');
}


start();
