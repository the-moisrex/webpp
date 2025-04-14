/**
 * This file downloads IdnaMappingTable.txt, and generates a C++ header file.
 *
 * Details on parsing this file can be found here:
 * UTS #46: https://www.unicode.org/reports/tr46/#IDNA_Mapping_Table
 */

import {
    findVersion,
    findDate,
    uint16,
    char8_8,
    uint32,
    utf32To8All,
    runClangFormat,
    writePieces,
    renderTableValues,
    sizeOf,
    toHexString,
    recursiveLength,
    findSimilarSubRange,
    findSimilarRange,
    packBoolsIntoInts, removeOverlaps, downloadFile
} from "./utils.mjs";

import {
    fileUrl,
    isMapped,
    cacheFilePath,
    DISALLOWED,
    isDisallowed,
    MAPPED,
    outFilePath,
    flagsStatus,
    isNotMapped,
    flagsOr,
    VALID,
    NOT_MAPPED,
    isValid, parseIDNAMappingTable
} from "./IdnaMappingTable.mjs"

import * as path from "node:path";

const start = async () => {
    const fileContent = await downloadFile(fileUrl, cacheFilePath);
    const version = findVersion(fileContent);
    const creationDate = findDate(fileContent);
    if (version === undefined || creationDate === undefined) {
        console.error("Could not find the version from the file content.");
        return;
    }
    console.log(`Version: ${version}`);
    console.log(`Creation Date: ${creationDate}`);
    const table = new MappingTable();
    await parseIDNAMappingTable(table, fileContent);
    table.process();
    await writePieces(outFilePath, [table.render(version, creationDate)]);
    await runClangFormat(outFilePath);
    console.log('File processing completed.');
};

class MappingTable {

    #refs = []; // items in this table points to the #refBlocks
    #refsExtra = []; // same as refs table; it's heere to save space
    #refBlocks = []; // items in this table stores flags and points to the #maps
    #refBools = []; // An optimization for #refBlocks for blocks that can contain only bools.
    #maps = []; // the mapped code points that the other table point to

    // The last code point that after that everything is disallowed
    #lastDisallowed = 0n;

    // This is the point where which reference table will be chosen
    #refBreakPointStart = 0;
    #refBreakPointEnd = 0;

    #rawMaps = [];

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
        this.#refsExtra = [];
        this.#refBlocks = [];
        this.#refBools = [];
        this.#maps = [];

        this.#refs.type = uint16;
        this.#refsExtra.type = this.#refs.type;
        this.#refBlocks.type = uint16;
        this.#refBools.type = uint32; // boolean
        this.#maps.type = char8_8;

        this.#refs.sizeof = sizeOf(this.#refs.type);
        this.#refsExtra.sizeof = this.#refs.sizeof;
        this.#refBlocks.sizeof = sizeOf(this.#refBlocks.type);
        this.#refBools.sizeof = sizeOf(this.#refBools.type);
        this.#maps.sizeof = sizeOf(this.#maps.type);

        // this number affects the size of the tables, try changing it:
        this.#batchBitCount = 6n;
        this.#batchSize = 0b1 << Number(this.#batchBitCount);
        // this.#bitLength = Number(this.#refs.sizeof);

        this.#refMax = (0b1 << Number(sizeOf(this.#refs.type))) - 1;
        this.#refBlocksMax = (0b1 << Number(sizeOf(this.#refBlocks.type))) - 1;

        this.#tablePickMask = 0b1 << (Number(this.#refs.sizeof) - 1);
    }

    add(codePoint, value) {
        // console.assert((mappedTo?.length || 1) > 0 && !isMapped(flags), `Flags don't match the other inputs.`, start, end, flags, mappedTo);

        // calculating the last disallowed code point
        if (!isDisallowed(value.flags)) {
            this.#lastDisallowed = codePoint + 1n;
        }

        this.#rawMaps.push(value);
    }

    findBreakPoint() {
        let tolerance = 3;
        let lastDisabled = true;
        let i = this.#rawMaps.length - 1;
        let length = 0;
        let lastLength = 0;
        for (; i >= 0 && tolerance !== 0; --i) {
            const cur = this.#rawMaps[i];
            const isDis = isDisallowed(cur.flags);
            if (lastDisabled !== isDis) {
                --tolerance;
                lastLength = length;
                length = 0;
            }
            lastDisabled = isDis;
            ++length;
        }
        return {start: i, length: lastLength};
    }

    insertRef(ref) {
        if (ref.pos < this.#refBreakPointStart) {
            this.#refs.push(ref);
        } else if (ref.pos >= this.#refBreakPointEnd) {
            // if (917944 >= ref.pos && 917944 < (ref.pos + this.#batchSize)) {
            //     console.log(ref);
            // }
            this.#refsExtra.push(ref);
        } else {
            console.log(`Omitted inserting ref:`, ref);
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
        const {utf8MappedTo, flags} = this.#rawMaps.at(index);
        if (isMapped(flags)) {
            const localUtf8MappedTo = [...utf8MappedTo, 0]; // add the last EOF '\0'
            return findSimilarSubRange(localUtf8MappedTo, this.#maps);
        }
        return null;
    }

    #areAllNotMapped(start, length) {
        const end = start + length;
        for (let pos = start; pos !== end; ++pos) {
            const {flags} = this.#rawMaps.at(pos);
            if (isMapped(flags)) {
                return false;
            }
        }
        return true;
    }

    #findOrInsertBools(start, length) {
        length = length <= (this.#rawMaps.length - start) ? length : (this.#rawMaps.length - start);
        const end = start + length;
        const block = this.#rawMaps.slice(start, end).map(({flags}) => isValid(flags));
        const found = findSimilarRange(block, this.#refBools);
        let targetIndex;
        if (found !== null) {
            targetIndex = found;
            // console.log(`Bool Block Found: `, found, this.#refBools.length, block.map(val => val ? '1' : '0').join(""));
        } else {
            const bools = packBoolsIntoInts(block, this.#refBools.sizeof).map(intVal => intVal.toString(2).padStart(Number(this.#refBools.sizeof), '0')).join("|");

            // didn't find it,
            // let's insert it then:
            targetIndex = this.#refBools.length;
            this.#refBools.push(...block);
            console.log(`Bool Block Not found: `, bools, targetIndex, this.#refBools.length, `${start}+${length}`);
        }
        return targetIndex;
    }

    /// Find the specified range from source table in target table
    #findOrInsertBlock(start, origLength) {
        const length = origLength <= (this.#rawMaps.length - start) ? origLength : (this.#rawMaps.length - start);
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
                const {rawStart} = block;
                if (!this.#isSameMapping(rawStart + index, pos)) {
                    continue nextTarget;
                }
            }
            break;
        }
        if (pos === end) { // we didn't find it, let's insert it then
            const statuses = [];
            for (let i = start; i !== end; ++i) {
                const {codePoint, flags, utf8MappedTo, mappedTo} = this.#rawMaps.at(i);
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
                        mapping.start = recursiveLength(this.#maps);
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
            if (statuses.length !== origLength) {

                // at the end of the IDNA table, we get to this situation:
                if (statuses.length < origLength) {
                    for (let i = statuses.length; i !== origLength; ++i) {
                        statuses.push(DISALLOWED);
                    }
                    console.log("Fixing end of the table:", start, end, statuses.length, length, origLength, statuses);
                } else {
                    throw new Error(`#${start}-${end} Invalid size: ${statuses.length}/${this.#batchSize}`);
                }
            }
            this.#refBlocks[targetIndex] = {
                rawStart: start,
                rawEnd: start + origLength,
                statuses,
                length: statuses.length
            };
        }
        return targetIndex;
    }

    process() {
        // Removing everything that's disallowed after the last one:
        const startLen = this.#rawMaps.length;
        this.#rawMaps = this.#rawMaps.filter(({codePoint}) => codePoint < this.#lastDisallowed);
        const endLen = this.#rawMaps.length;
        console.log("Removed trailing disallowed mappings:", startLen - endLen);

        // split the reference tables:
        const {start: splitStart, length: splitLength} = this.findBreakPoint();
        this.#refBreakPointStart = splitStart;
        this.#refBreakPointEnd = splitStart + splitLength;

        for (let batchIndex = 0; batchIndex < this.#rawMaps.length; batchIndex += this.#batchSize) {
            let ref = {
                index: batchIndex >>> Number(this.#batchBitCount),
                pos: batchIndex
            };
            if (this.#areAllNotMapped(batchIndex, this.#batchSize)) {
                ref.blockPtr = this.#findOrInsertBools(batchIndex, this.#batchSize);
                if (ref.blockPtr >= this.#tablePickMask) {
                    throw new Error(`We ran out of room for bool blocks table; it now has a conflict with the bit mask!`);
                }
                ref.blockPtr |= this.#tablePickMask;
            } else {
                ref.blockPtr = this.#findOrInsertBlock(batchIndex, this.#batchSize);
                ref.blockPtr = recursiveLength(this.#refBlocks, ref.blockPtr);
                ref = this.optimizeBlocks(ref);
                if (ref.blockPtr >= this.#tablePickMask) {
                    throw new Error(`We ran out of room for blocks table; it now has a conflict with the bit mask!`);
                }
            }
            if (ref.blockPtr > this.#refMax) {
                throw new Error(`Calculated value ${ref.blockPtr} is greater than ${this.#refMax}, so we can't put it inside the ref table; ${this.#tablePickMask}`);
            }

            this.insertRef(ref);
        }
    }

    optimizeBlocks(ref) {
        ref = {...ref};
        if (this.#refBlocks.length > 1) {
            const curBlock = this.#refBlocks.length - 1;
            const {
                rhs,
                overlapLen
            } = removeOverlaps(this.#refBlocks[curBlock - 1].statuses, this.#refBlocks[curBlock].statuses);
            if (overlapLen !== 0) {
                this.#refBlocks[curBlock].statuses = rhs;
                // this.#refBlocks[curBlock].rawStart -= overlapLen;
                // this.#refBlocks[curBlock].rawEnd -= overlapLen;
                this.#refBlocks[curBlock].length = rhs.length;
                ref.blockPtr -= overlapLen;
                console.log("Smashed blocks:", overlapLen, ref, this.#refBlocks[curBlock - 1].statuses.slice(this.#refBlocks[curBlock - 1].statuses.length - overlapLen - 2), this.#refBlocks[curBlock].statuses.slice(0, overlapLen + 2)/*, rhs*/);
            }
        }

        // todo: the bools table can be optimize as well
        // todo: the findOrInsertBlock algorithm can be optimized as well
        return ref;
    }

    render(version, creationDate) {
        const packedBools = packBoolsIntoInts(this.#refBools, this.#refBools.sizeof);
        // const packedLength = Math.ceil(this.#refBools.length / Number(this.#refBools.sizeof));
        const packedLength = packedBools.length;

        const mapsLength = recursiveLength(this.#maps);
        const blocksLength = recursiveLength(this.#refBlocks);
        if (mapsLength === this.#maps.length || blocksLength === this.#refBlocks.length) {
            throw new Error(`Recursive length calculation is wrong; maps len: ${this.#maps.length}, blocks len: ${this.#refBlocks.length} * ${this.#refBlocks[0].length}`);
        }
        const refsBitLength = this.#refs.length * Number(this.#refs.sizeof);
        const refsExtraBitLength = this.#refsExtra.length * Number(this.#refsExtra.sizeof);
        const blockBitLength = blocksLength * Number(this.#refBlocks.sizeof);
        const boolsBitLength = packedLength * Number(this.#refBools.sizeof);
        const mapsBitLength = mapsLength * Number(this.#maps.sizeof);
        const sumBitLength = refsBitLength + refsExtraBitLength + blockBitLength + boolsBitLength + mapsBitLength;
        console.log(`Reference Table size:`);
        console.log(`  Count       : ${this.#refs.length} * ${this.#refs.sizeof}`);
        console.log(`  in bytes    : ${Math.ceil(refsBitLength / 8)},`);
        console.log(`  in KibiBytes: ${(refsBitLength / 8 / 1024).toFixed(2)} KiB\n`);
        console.log(`Reference Extra Table size:`);
        console.log(`  Count       : ${this.#refsExtra.length} * ${this.#refsExtra.sizeof}`);
        console.log(`  in bytes    : ${Math.ceil(refsExtraBitLength / 8)},`);
        console.log(`  in KibiBytes: ${(refsExtraBitLength / 8 / 1024).toFixed(2)} KiB\n`);
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
        console.log("Last Disallowed Code Point:", this.#lastDisallowed);
        console.log("Batch Size:", this.#batchSize);
        console.log("Batch Bit Count:", this.#batchBitCount);
        console.log("Ref Sizeof:", this.#refs.sizeof);
        console.log("Ref Extra Sizeof:", this.#refsExtra.sizeof);
        console.log("Blocks Sizeof:", this.#refBlocks.sizeof);
        console.log("Maps Sizeof:", this.#maps.sizeof);
        console.log("Ref Max:", this.#refMax);
        console.log("Ref Block Max:", this.#refBlocksMax);
        console.log("Table Pick Mask:", this.#tablePickMask);
        console.log();


        const refPrinter = ({blockPtr}) => {
            blockPtr = (blockPtr || 0);
            let ret = '';
            if ((blockPtr & this.#tablePickMask) === this.#tablePickMask) {
                ret += `blt | `;
                blockPtr &= ~this.#tablePickMask; // removing it
            }
            ret += `0x${blockPtr.toString(16).toUpperCase()}U`
            return ret;
        };

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
 
#ifndef WEBPP_UNICODE_IDNA_MAPPING_TABLES_HPP
#define WEBPP_UNICODE_IDNA_MAPPING_TABLES_HPP

#include <array>
#include <cstdint>
#include <string_view>

namespace webpp::unicode::idna::details {

    static constexpr auto last_disallowed = static_cast<char32_t>(0x${this.#lastDisallowed.toString(16).toUpperCase()});
    static constexpr std::uint8_t batch_bit_count = ${this.#batchBitCount}U;
    static constexpr std::uint8_t batch_mask = 0x${((0b1 << Number(this.#batchBitCount)) - 1).toString(16).toUpperCase()}U;

    [[maybe_unused]] static constexpr ${this.#refBlocks.type.description} ${flagsStatus(NOT_MAPPED)} = 0b${NOT_MAPPED.toString(2)}U;
    static constexpr ${this.#refBlocks.type.description} ${flagsStatus(VALID)} = 0b${VALID.toString(2)}U;
    static constexpr ${this.#refBlocks.type.description} ${flagsStatus(DISALLOWED)} = 0b${DISALLOWED.toString(2)}U;
    
    // Reference table is split into two tables, these are the where they start and end:
    static constexpr auto breakpoint_start = static_cast<char32_t>(${(this.#refBreakPointStart)});
    static constexpr auto breakpoint_end = static_cast<char32_t>(${(this.#refBreakPointEnd)});

    // Pick the table with this mask (between bools table and the block table)
    static constexpr ${this.#refBlocks.type.description} table_pick_mask = 0b${this.#tablePickMask.toString(2)}U;
    static constexpr auto blt = table_pick_mask; // shortcut

    /**
     * IDNA Reference Table.
     * 
     * Table size: ${refsBitLength / 8} B or ${(refsBitLength / 8 / 1024).toFixed(2)} KiB
     */
    static constexpr std::array<${this.#refs.type.description}, ${this.#refs.length}ULL> idna_refs {
       ${this.#refs.map(refPrinter).join(", ")}
    };
    

    /**
     * IDNA Reference Table part 2 (Extra).
     * 
     * Table size: ${refsExtraBitLength / 8} B or ${(refsExtraBitLength / 8 / 1024).toFixed(2)} KiB
     */
    static constexpr std::array<${this.#refsExtra.type.description}, ${this.#refsExtra.length}ULL> idna_refs_extra {
       ${this.#refsExtra.map(refPrinter).join(", ")}
    };
    

    /**
     * IDNA Reference Blocks Table (for valid or disallowed values only).
     *
     * The bits in the integer are stored in reverse order, so we don't have to
     * do additional calculations to get the bit that we need.
     * 
     *  - true:  ${flagsStatus(VALID)}
     *  - false: ${flagsStatus(DISALLOWED)}
     * 
     * Table size: ${boolsBitLength / 8} B or ${(boolsBitLength / 8 / 1024).toFixed(2)} KiB
     */
    static constexpr std::array<${this.#refBools.type.description}, ${packedLength}ULL> idna_ref_bools {
       ${packedBools.map((block) => `0b${block.toString(2).padStart(Number(this.#refBools.sizeof), '0')}U`).join(", ")}
    };

    /**
     * IDNA Reference Blocks Table.
     * 
     * Table size: ${blockBitLength / 8} B or ${(blockBitLength / 8 / 1024).toFixed(2)} KiB
     */
    static constexpr std::array<${this.#refBlocks.type.description}, ${blocksLength}ULL> idna_ref_blocks {
       ${this.#refBlocks.map((block, blkIndex) => `
           ${block.statuses.map(flags => isNotMapped(flags) ? flagsStatus(flags) : `${(flags || 0).toString()}U`).join(", ")}
       `).join(", ")}
    };
    
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
            let {position, start, codePointSources, utf32MappedTo} = block;
            codePointSources = codePointSources.map(curCP => curCP.toString(16).toUpperCase());
            utf32MappedTo = utf32MappedTo.map(curCP => curCP.toString(16).toUpperCase());
            blk.inline_comment = `#${position}/${start}: [${codePointSources.join(', ')}] ==> [${utf32MappedTo.join(', ')}]`;
            return blk;
        }),
        len: mapsLength,
    })}
    
    
} // webpp::unicode::idna::details

#endif // WEBPP_UNICODE_IDNA_MAPPING_TABLES_HPP
  `;
    }
}


await start();
