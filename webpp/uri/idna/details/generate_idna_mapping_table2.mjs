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
    parseCodePointRange,
    parseCodePointRangeExclusive
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

const isMapped = (flags) => flags & NOT_MAPPED === 0;
const isIgnored = (flags) => isMapped(flags);
const isValid = (flags) => flags & VALID === VALID;
const isDisallowed = (flags) => flags & DISALLOWED === DISALLOWED;


class MappingTable {

    #refs; // the flags and what not
    #maps; // the mapped code points that the other table point to

    #rawMaps = [];

    // the modulus value
    #magicRem = 1n;

    constructor(max) {
        this.#refs = new TableTraits(max, uint16);
        this.#maps = new TableTraits(max, char8_8);
    }

    append(start, end, flags, mappedTo = []) {
        console.assert((mappedTo?.length || 1) > 0 && !isMapped(flags), "Flags don't match the other inputs.");

        for (; start <= end; ++start) {
            const raw = {
                codePoint: BigInt(start),
                flags,
                mappedTo,
                utf8MappedTo: utf32To8All(mappedTo)
            };
            // console.log(raw)
            this.#rawMaps.push(raw);
        }
    }

    calculate() {
        const invalidFlag = 0b11111111;
        let tryNum = 0;
        const nextAttempt = (...info) => {
            console.log(`Attempt #${tryNum} with magic rem of ${this.#magicRem} failed.`, ...info);
            this.#refs.clear(invalidFlag);
            this.#maps.clear();
            ++this.#magicRem;
            ++tryNum;
        };

        retry: for (; ;) {

            let pos = 0;
            for (let i = 0; i !== this.#rawMaps.length; ++i) {
                const {codePoint, flags, mappedTo, utf8MappedTo} = this.#rawMaps[i];
                const loc = codePoint % this.#magicRem;
                const val = isMapped(flags) ? flags | pos : flags;
                const mag = this.#refs.atOr(loc, invalidFlag);
                if (mag !== val && mag !== invalidFlag) {
                    nextAttempt(codePoint, loc);
                    continue retry;
                }

                this.#refs.setOrFill(loc, val, invalidFlag);

                pos += utf8MappedTo.length + 1;

                // Add the UTF-8 encoded mapped to code points:
                // for (const cu of utf8MappedTo) {
                //     this.#maps.push(cu);
                // }
                // this.#maps.push(0);
            }

            break;
        }
        console.log(`Success on #${tryNum}th try with magic rem of ${this.#magicRem}`);
    }

    serializeTable(table) {
        let res = "";
        for (let pos = 0; pos !== table.length;) {
            res += `${table[pos]}${table.postfix}, `;
            ++pos;
            if (pos % 20 === 0) {
                res += '\n';
            }
        }
        return res;
    }

    render(version, creationDate) {
        const refsBitLength = this.#refs.length * this.#refs.sizeof;
        const mapsBitLength = this.#maps.length * this.#maps.sizeof;
        console.log(`Reference Table size:`);
        console.log(`  in bytes: ${refsBitLength / 8},`);
        console.log(`  in KibiBytes: ${Math.ceil(refsBitLength / 8 / 1024)} KiB\n`);
        console.log(`Map Table size:`);
        console.log(`  in bytes: ${mapsBitLength / 8},`);
        console.log(`  in KibiBytes: ${Math.ceil(mapsBitLength / 8 / 1024)} KiB\n`);

        return `
/**
 * Attention: Auto-generated file, don't modify.
 * 
 *   Auto generated from:          ${path.basename(__filename)}
 *   IDNA Creation Date:           ${creationDate}
 *   This file's generation date:  ${new Date().toUTCString()}
 *   IDNA Mapping Table Version:   ${version}
 *
 * Details about the contents of this file can be found here:
 *   UTS #46: https://www.unicode.org/reports/tr46/#IDNA_Mapping_Table
 *   IDN FAQ: https://www.unicode.org/faq/idn.html
 */
 
#ifndef WEBPP_URI_IDNA_MAPPING_TABLE_HPP
#define WEBPP_URI_IDNA_MAPPING_TABLE_HPP

#include <array>
#include <cstdint>

namespace webpp::uri::idna::details {

    static constexpr std::uint16_t magic_rem = ${this.#magicRem}U;

    /**
     * IDNA Reference Table
     * 
     * This table will let us see if each Unicode code point is mapped/disallowed/ignored/... .
     * 
     * Table size:
     *   - in bytes:      ${refsBitLength / 8} B
     *   - in KibiBytes:  ${Math.ceil(refsBitLength / 8 / 1024)} KiB
     */
    static constexpr std::array<std::${this.#refs.typeString}_t, ${this.#refs.length}ULL> idna_mapping_flags {
       ${this.serializeTable(this.#refs)}
    };
    
    
    /**
     * IDNA Mapped Code Points Table
     * 
     * Table size:
     *   - in bytes:      ${mapsBitLength / 8} B
     *   - in KibiBytes:  ${Math.ceil(mapsBitLength / 8 / 1024)} KiB
     */
    static constexpr std::array<std::${this.#maps.typeString}_t, ${this.#maps.length}ULL> idna_mappings {
       ${this.serializeTable(this.#maps)}
    };
    
    
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

    const tables = new MappingTable(200000);
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
                return;
        }
        tables.append(rangeStart, rangeEnd, flags, mappedValues);

        if (mappedValues?.length > maxMappedCount) {
            maxMappedCount = mappedValues.length;
        }
        cpSum += rangeEnd - rangeStart + 1n;

        console.log(`${index}/${cpSum}:`, rangeStart, rangeEnd, status, mappedValues || "", IDNA2008Status || "");
    });


    console.log("Max Mapped Count: ", maxMappedCount);
    tables.calculate();

    await writePieces(outFilePath, [tables.render(version, creationDate)]);

    // Reformat the file
    await runClangFormat(outFilePath);

    console.log('File processing completed.');
}


start();