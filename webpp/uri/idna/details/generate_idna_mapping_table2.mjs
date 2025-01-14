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
    uint16,
    char8_8,
    uint8,
    utf32To8All,
    runClangFormat,
    writePieces,
    renderTableValues,
    parseCodePointRangeExclusive,
    sizeOf,
    toHexString,
    recursiveLength,
    findSimilarSubRange
} from "../../../unicode/details/utils.mjs";

import * as path from "node:path";

const fileUrl = 'https://www.unicode.org/Public/idna/latest/IdnaMappingTable.txt';
const cacheFilePath = 'IdnaMappingTable.txt';
const outFilePath = `idna_mapping_table.hpp`;


const start = async () => {
    await downloadFile(fileUrl, cacheFilePath, processCachedFile);
};

const parseMappedCodePoints = codePoints => codePoints.split(" ").map(codePoint => parseInt(codePoint, 16));

// UTF-16 version:
const MAPPED = 0b000 << 13;
const NOT_MAPPED = 0b100 << 13;
const VALID = NOT_MAPPED | 0b001;
const DISALLOWED = NOT_MAPPED | 0b010;

// UTF-8 version:
// const MAPPED = 0b0 << 7;
// const NOT_MAPPED = 0b1 << 7;
// const VALID = NOT_MAPPED | 0b001;
// const DISALLOWED = NOT_MAPPED | 0b010;

const isMapped = (flags) => flags < NOT_MAPPED;
const isValid = (flags) => flags === VALID;
const isDisallowed = (flags) => flags === DISALLOWED;
const isNotMapped = (flags) => !isMapped(flags);
const flagsStatus = (flags) => {
    switch (flags) {
        case VALID: return "valid";
        case NOT_MAPPED: return "NOT_MAPPED";
        case DISALLOWED: return "disallowed";
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

    // Maximum possible value that can be put in the #refBlocks table
    #refBlocksMax;
    #refMax;

    constructor() {
        this.#refs = [];
        this.#refBlocks = [];
        this.#maps = [];

        this.#refs.type = uint8;
        this.#refBlocks.type = uint16;
        this.#maps.type = char8_8;

        this.#refs.sizeof = sizeOf(this.#refs.type);
        this.#refBlocks.sizeof = sizeOf(this.#refBlocks.type);
        this.#maps.sizeof = sizeOf(this.#maps.type);

        this.#batchBitCount = 7n;
        this.#batchSize = 0b1 << Number(this.#batchBitCount);
        //     this.#bitLength = Number(this.#refs.sizeof);
        
        this.#refMax = (0b1 << Number(sizeOf(this.#refs.type))) - 1;
        this.#refBlocksMax = (0b1 << Number(sizeOf(this.#refBlocks.type))) - 1;
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
            return findSimilarSubRange(localUtf8MappedTo, this.#maps);
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
                const { rawStart } = block;
                if (!this.#isSameMapping(rawStart + index, pos)) { // todo
                    continue nextTarget;
                }
            }
            break;
        }
        if (pos === end) { // we didn't find it, let's insert it then
            const statuses = [];
            for (let i = start; i !== end; ++i) {
                const { codePoint, flags, utf8MappedTo } = this.#rawMaps.at(i);
                let mapsPosition = 0;

                // insert the utf-8 encoded stuff to the table
                if (isMapped(flags)) {
                    mapsPosition = this.#findSimilarMappedTo(i);
                    if (mapsPosition === null) {
                        mapsPosition = this.#maps.length;
                        let mapping = [];
                        mapping.comment = `#${mapsPosition}: ${toHexString(codePoint)}`;
                        for (const codeUnit of utf8MappedTo) {
                            mapping.push(codeUnit);
                        }
                        mapping.push(0); // the last EOF '\0' character
                        this.#maps.push(mapping);
                    }
                }

                const curStatus = flagsOr(flags, mapsPosition);
                if (curStatus > this.#refBlocksMax) {
                    throw new Error(`Status ${curStatus} is greater than ${this.#refBlocksMax}, so we can't put it in the ref blocks table.`);
                }
                statuses.push(curStatus);
            }
            this.#refBlocks[targetIndex] = {
                rawStart: start,
                rawEnd: end,
                statuses,
                length: statuses.length
            };
        }
        return targetIndex;
    }

    process() {
        // Removing everything that's disallowed after the last one:
        const startLen = this.#rawMaps.length;
        this.#rawMaps = this.#rawMaps.filter(({ codePoint }) => codePoint < this.#lastDisallowed);
        const endLen = this.#rawMaps.length;
        console.log("Removed trailing disallowed mappings:", startLen - endLen);

        for (let batchIndex = 0; batchIndex < this.#rawMaps.length; batchIndex += this.#batchSize) {
            const ref = {
                index: batchIndex >>> Number(this.#batchBitCount),
                blockPtr: this.#findOrInsertBlock(batchIndex, this.#batchSize)
            };
            if (ref.blockPtr > this.#refMax) {
                throw new Error(`Calculated value ${ref.blockPtr} is greater than ${this.#refMax}, so we can't put it inside the ref table.`);
            }
            this.#refs.push(ref);
        }
    }

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
        const mapsLength = recursiveLength(this.#maps);
        const blocksLength = recursiveLength(this.#refBlocks);
        if (mapsLength === this.#maps.length || blocksLength === this.#refBlocks.length) {
            throw new Error(`Recursive length calculation is wrong; maps len: ${this.#maps.length}, blocks len: ${this.#refBlocks.length} * ${this.#refBlocks[0].length}`);
        }
        const refsBitLength = this.#refs.length * Number(this.#refs.sizeof);
        const blockBitLength = blocksLength * Number(this.#refBlocks.sizeof);
        const mapsBitLength = mapsLength * Number(this.#maps.sizeof);
        const sumBitLength = refsBitLength + blockBitLength + mapsBitLength;
        console.log(`Reference Table size:`);
        console.log(`  Count       : ${this.#refs.length} * ${this.#refs.sizeof}`);
        console.log(`  in bytes    : ${Math.ceil(refsBitLength / 8)},`);
        console.log(`  in KibiBytes: ${(refsBitLength / 8 / 1024).toFixed(2)} KiB\n`);
        console.log(`Ref Blocks Table size:`);
        console.log(`  Count       : ${blocksLength} * ${this.#refBlocks.sizeof}`);
        console.log(`  in bytes    : ${Math.ceil(blockBitLength / 8)},`);
        console.log(`  in KibiBytes: ${(blockBitLength / 8 / 1024).toFixed(2)} KiB\n`);
        console.log(`Map Table size:`);
        console.log(`  Count       : ${mapsLength} * ${this.#maps.sizeof}`);
        console.log(`  in bytes    : ${Math.ceil(mapsBitLength / 8)},`);
        console.log(`  in KibiBytes: ${(mapsBitLength / 8 / 1024).toFixed(2)} KiB\n`);
        console.log(`Total Table size:`);
        console.log(`  in bytes    : ${Math.ceil(sumBitLength / 8)},`);
        console.log(`  in KibiBytes: ${(sumBitLength / 8 / 1024).toFixed(2)} KiB\n`);
        console.log(`Last Disallowed Code Point: ${this.#lastDisallowed}\n`);

        return `
/**
 * Attention: Auto-generated file, don't modify.
 * 
 *   Auto generated from:          ${path.basename(new URL(import.meta.url).pathname)}
 *   IDNA Creation Date:           ${creationDate}
 *   This file's generation date:  ${new Date().toUTCString()}
 *   IDNA Mapping Table Version:   ${version}
 *   Size:                         ${Math.ceil(sumBitLength / 8)} B
 *                                 ${(sumBitLength / 8 / 1024).toFixed(2)} KiB
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
    static constexpr std::uint8_t batch_bit_count = ${this.#batchBitCount};

    static constexpr ${this.#refBlocks.type.description} ${flagsStatus(NOT_MAPPED)} = 0b${NOT_MAPPED.toString(2)}U;
    static constexpr ${this.#refBlocks.type.description} ${flagsStatus(VALID)} = 0b${VALID.toString(2)}U;
    static constexpr ${this.#refBlocks.type.description} ${flagsStatus(DISALLOWED)} = 0b${DISALLOWED.toString(2)}U;

    /**
     * IDNA Reference Table
     * 
     * Table size: ${refsBitLength / 8} B or ${(refsBitLength / 8 / 1024).toFixed(2)} KiB
     */
    static constexpr std::array<${this.#refs.type.description}, ${this.#refs.length}ULL> idna_refs {
       ${this.#refs.map(({ blockPtr }) => `0x${(blockPtr || 0).toString(16)}`).join(", ")}
    };
    

    /**
     * IDNA Reference Blocks Table
     * 
     * Table size: ${blockBitLength / 8} B or ${(blockBitLength / 8 / 1024).toFixed(2)} KiB
     */
    static constexpr std::array<std::array<${this.#refBlocks.type.description}, ${this.#refBlocks[0].length}ULL>, ${this.#refBlocks.length}ULL> idna_ref_blocks {
       ${this.#refBlocks.map((block, blkIndex) => `
           // Block #${blkIndex}
           { ${block.statuses.map(flags => isNotMapped(flags) ? flagsStatus(flags) : `0x${(flags || 0).toString(16)}`).join(", ")} }
       `).join(", ")}
    };
    
    /**
     * IDNA Mapped Code Points Table
     * Each mapping ends with EOF '\\0'.
     * 
     * Table size: ${mapsBitLength / 8} B or ${(mapsBitLength / 8 / 1024).toFixed(2)} KiB
     */
    ${renderTableValues({
            name: "idna_mappings",
            type: this.#maps.type,
            printableValues: this.#maps.map(block => block.map(val => toHexString(val))),
            len: mapsLength,
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

    const tables = new MappingTable();
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
    tables.process();

    await writePieces(outFilePath, [tables.render(version, creationDate)]);

    // Reformat the file
    await runClangFormat(outFilePath);

    console.log('File processing completed.');
}


start();
