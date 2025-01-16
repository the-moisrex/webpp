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
    uint32,
    utf32To8All,
    runClangFormat,
    writePieces,
    renderTableValues,
    parseCodePointRangeExclusive,
    sizeOf,
    toHexString,
    recursiveLength,
    findSimilarSubRange,
    findSimilarRange,
    packBoolsIntoInts
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
const DISALLOWED = NOT_MAPPED | 0b010;
const VALID = DISALLOWED | 0b1;

// UTF-8 version:
// const MAPPED = 0b0 << 7;
// const NOT_MAPPED = 0b1 << 7;
// const VALID = NOT_MAPPED | 0b011;
// const DISALLOWED = NOT_MAPPED | 0b010;

const isMapped = (flags) => flags < NOT_MAPPED;
const isValid = (flags) => flags === VALID;
const isDisallowed = (flags) => flags === DISALLOWED;
const isNotMapped = (flags) => !isMapped(flags);
const flagsStatus = (flags) => {
    switch (flags) {
        case VALID: return "valid";
        case NOT_MAPPED: return "not_mapped";
        case DISALLOWED: return "disallowed";
        default:
            return isMapped(flags) ? `<Mapped:${flags}>` : `<invalid:${flags.toString(16)}>`;
    }
};
const flagsOr = (flags, pos) => flags | (isMapped(flags) ? pos : 0);


class MappingTable {

    #refs = []; // items in this table points to the #refBlocks
    #refBlocks = []; // items in this table stores flags and points to the #maps
    #refBools = []; // An optimization for #refBlocks for blocks that can contain only bools.
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

    // The mask that makes the difference on which table they should choose
    // between the refBlocks table and refBools table
    #tablePickMask;

    constructor() {
        this.#refs = [];
        this.#refBlocks = [];
        this.#refBools = [];
        this.#maps = [];

        this.#refs.type = uint16;
        this.#refBlocks.type = uint16;
        this.#refBools.type = uint32; // boolean
        this.#maps.type = char8_8;

        this.#refs.sizeof = sizeOf(this.#refs.type);
        this.#refBlocks.sizeof = sizeOf(this.#refBlocks.type);
        this.#refBools.sizeof = sizeOf(this.#refBools.type);
        this.#maps.sizeof = sizeOf(this.#maps.type);

        // this number affects the size of the tables, try changing it:
        this.#batchBitCount = 8n;
        this.#batchSize = 0b1 << Number(this.#batchBitCount);
        //     this.#bitLength = Number(this.#refs.sizeof);

        this.#refMax = (0b1 << Number(sizeOf(this.#refs.type))) - 1;
        this.#refBlocksMax = (0b1 << Number(sizeOf(this.#refBlocks.type))) - 1;

        this.#tablePickMask = 0b1 << (Number(this.#refs.sizeof) - 1);
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
            const localUtf8MappedTo = [...utf8MappedTo, 0]; // add the last EOF '\0'
            return findSimilarSubRange(localUtf8MappedTo, this.#maps);
        }
        return null;
    }

    #areAllNotMapped(start, length) {
        const end = start + length;
        for (let pos = start; pos !== end; ++pos) {
            const { flags } = this.#rawMaps.at(pos);
            if (isMapped(flags)) {
                return false;
            }
        }
        return true;
    }

    #findOrInsertBools(start, length) {
        length = length <= (this.#rawMaps.length - start) ? length : (this.#rawMaps.length - start);
        const end = start + length;
        const block = this.#rawMaps.slice(start, end).map(({ flags }) => isValid(flags));
        const found = findSimilarRange(block, this.#refBools);
        let targetIndex;
        if (found !== null) {
            targetIndex = found;
            console.log(`Bool Block Found: `, found, this.#refBools.length);
        } else {

            // didn't find it,
            // let's insert it then:
            targetIndex = this.#refBools.length;
            this.#refBools.push(...block);
            console.log(`Bool Block Not found: `, targetIndex, this.#refBools.length);
        }

        // const blockLen = Number(this.#refBools.sizeof);
        // if (targetIndex % blockLen !== 0) {
        //     throw new Error("Index is not aligned.");
        // }
        // const index = targetIndex / blockLen;
        // console.log(`Index: `, index, this.#refBools.length);
        return targetIndex;
    }

    /// Find the specified range from source table in target table
    #findOrInsertBlock(start, length) {
        length = length <= (this.#rawMaps.length - start) ? length : (this.#rawMaps.length - start);
        const end = start + length;
        let targetIndex = 0;
        let pos = start;
        nextTarget: for (; ; ++targetIndex) {
            for (pos = start; pos !== end; ++pos) {
                const index = pos - start;
                const block = this.#refBlocks.at(targetIndex);
                if (!block) {
                    pos = end;
                    break;
                }
                const { rawStart } = block;
                if (!this.#isSameMapping(rawStart + index, pos)) {
                    continue nextTarget;
                }
            }
            break;
        }
        if (pos === end) { // we didn't find it, let's insert it then
            const statuses = [];
            for (let i = start; i !== end; ++i) {
                const { codePoint, flags, utf8MappedTo, mappedTo } = this.#rawMaps.at(i);
                let mapsPosition = 0;

                // insert the utf-8 encoded stuff to the table
                if (isMapped(flags)) {
                    mapsPosition = this.#findSimilarMappedTo(i);
                    if (mapsPosition === null) {
                        mapsPosition = this.#maps.length;
                        let mapping = [...utf8MappedTo, 0]; // add the extra EOF '\0' at the end too
                        mapping.position = mapsPosition;
                        mapping.codePointSources = [codePoint];
                        mapping.utf32MappedTo = mappedTo;
                        this.#maps.push(mapping);
                    } else {
                        this.#maps[mapsPosition].codePointSources.push(codePoint);
                    }

                    // testing:
                    const mapping = this.#maps[mapsPosition];
                    if (mapping.position !== mapsPosition || !mapping.utf32MappedTo.every((cp, idx) => mappedTo[idx] === cp)) {
                        throw new Error(`Invalid mapping position "${mapsPosition}"`);
                    }
                }

                const mapsLength = recursiveLength(this.#maps, mapsPosition);
                const curStatus = flagsOr(flags, mapsLength);
                if (curStatus > this.#refBlocksMax) {
                    throw new Error(`Status ${curStatus} is greater than ${this.#refBlocksMax}, so we can't put it in the ref blocks table.`);
                }
                statuses.push(curStatus);
            }
            if (statuses.length !== this.#batchSize) {

                // at the end of the IDNA table, we get to this situation:
                if (statuses.length < this.#batchSize) {
                    for (let i = statuses.length; i !== this.#batchSize; ++i) {
                        statuses.push(DISALLOWED);
                    }
                } else {
                    throw new Error(`#${start}-${end} Invalid size: ${statuses.length}/${this.#batchSize}`);
                }
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
            let ref = {
                index: batchIndex >>> Number(this.#batchBitCount),
            };
            if (this.#areAllNotMapped(batchIndex, this.#batchSize)) {
                ref.blockPtr = this.#findOrInsertBools(batchIndex, this.#batchSize);
                if (ref.blockPtr >= this.#tablePickMask) {
                    throw new Error(`We ran out of room for bool blocks table; it now has a conflict with the bit mask!`);
                }
                ref.blockPtr |= this.#tablePickMask;
            } else {
                ref.blockPtr = this.#findOrInsertBlock(batchIndex, this.#batchSize);
                if (ref.blockPtr >= this.#tablePickMask) {
                    throw new Error(`We ran out of room for blocks table; it now has a conflict with the bit mask!`);
                }
            }
            if (ref.blockPtr > this.#refMax) {
                throw new Error(`Calculated value ${ref.blockPtr} is greater than ${this.#refMax}, so we can't put it inside the ref table; ${this.#tablePickMask}`);
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
        const boolsBitLength = Math.ceil(this.#refBools.length / Number(this.#refBools.sizeof)) * Number(this.#refBools.sizeof);
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
        console.log(`Ref Bools Blocks Table size:`);
        console.log(`  Count       : ${this.#refBools.length} * 1`);
        console.log(`  in bytes    : ${Math.ceil(boolsBitLength / 8)},`);
        console.log(`  in KibiBytes: ${(boolsBitLength / 8 / 1024).toFixed(2)} KiB\n`);
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
    static constexpr auto last_disallowed = static_cast<char32_t>(0x${this.#lastDisallowed.toString(16).toUpperCase()});
    static constexpr std::uint8_t batch_bit_count = ${this.#batchBitCount}U;
    static constexpr std::uint8_t batch_mask = 0x${((0b1 << Number(this.#batchBitCount)) - 1).toString(16).toUpperCase()}U;

    [[maybe_unused]] static constexpr ${this.#refBlocks.type.description} ${flagsStatus(NOT_MAPPED)} = 0b${NOT_MAPPED.toString(2)}U;
    static constexpr ${this.#refBlocks.type.description} ${flagsStatus(VALID)} = 0b${VALID.toString(2)}U;
    static constexpr ${this.#refBlocks.type.description} ${flagsStatus(DISALLOWED)} = 0b${DISALLOWED.toString(2)}U;

    // Pick the table with this mask (between bools table and the block table)
    static constexpr ${this.#refBlocks.type.description} table_pick_mask = 0b${this.#tablePickMask.toString(2)}U;
    static constexpr auto blt = table_pick_mask; // shortcut

    /**
     * IDNA Reference Table.
     * 
     * Table size: ${refsBitLength / 8} B or ${(refsBitLength / 8 / 1024).toFixed(2)} KiB
     */
    static constexpr std::array<${this.#refs.type.description}, ${this.#refs.length}ULL> idna_refs {
       ${this.#refs.map(({ blockPtr }) => {
            blockPtr = (blockPtr || 0);
            let ret = '';
            if ((blockPtr & this.#tablePickMask) === this.#tablePickMask) {
                ret += `blt | `;
                blockPtr &= ~this.#tablePickMask; // removing it
            }
            ret += `0x${blockPtr.toString(16).toUpperCase()}U`
            return ret;
        }).join(", ")}
    };
    

    /**
     * IDNA Reference Blocks Table (for valid or disallowed values only).
     * 
     *  - true:  ${flagsStatus(VALID)}
     *  - false: ${flagsStatus(DISALLOWED)}
     * 
     * Table size: ${boolsBitLength / 8} B or ${(boolsBitLength / 8 / 1024).toFixed(2)} KiB
     */
    static constexpr std::array<${this.#refBools.type.description}, ${this.#refBools.length / Number(this.#refBools.sizeof)}ULL> idna_ref_bools {
       ${packBoolsIntoInts(this.#refBools, this.#refBools.sizeof).map((block) => `0b${block.toString(2).padStart(Number(this.#refBools.sizeof), '0')}U`).join(", ")}
    };

    /**
     * IDNA Reference Blocks Table.
     * 
     * Table size: ${blockBitLength / 8} B or ${(blockBitLength / 8 / 1024).toFixed(2)} KiB
     */
    static constexpr std::array<std::array<${this.#refBlocks.type.description}, ${this.#refBlocks[0].length}ULL>, ${this.#refBlocks.length}ULL> idna_ref_blocks {{
       ${this.#refBlocks.map((block, blkIndex) => `
       
           // Block #${blkIndex}
           { ${block.statuses.map(flags => isNotMapped(flags) ? flagsStatus(flags) : `${(flags || 0).toString()}U`).join(", ")} }
       `).join(", ")}
    }};
    
    /**
     * IDNA Mapped Code Points Table.
     * Each mapping ends with EOF '\\0'.
     * 
     * Table size: ${mapsBitLength / 8} B or ${(mapsBitLength / 8 / 1024).toFixed(2)} KiB
     */
    ${renderTableValues({
            name: "idna_mappings",
            type: this.#maps.type,
            printableValues: this.#maps.map(block => {
                let blk = block.map(val => toHexString(val));
                let {position, codePointSources, utf32MappedTo} = block;
                codePointSources = codePointSources.map(curCP => curCP.toString(16).toUpperCase());
                utf32MappedTo = utf32MappedTo.map(curCP => curCP.toString(16).toUpperCase());
                blk.inline_comment = `#${position}: [${codePointSources.join(', ')}] ==> [${utf32MappedTo.join(', ')}]`;
                return blk;
            }),
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


await start();
