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
    parseCodePointRangeExclusive,
    uint8
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


class MappingTable {

    #refs; // the flags and what not
    #maps; // the mapped code points that the other table point to

    #rawMaps = [];

    // the modulus value
    #magicRem = 1n;

    constructor(max) {
        this.#refs = new TableTraits(max, uint8);
        this.#maps = new TableTraits(max, char8_8);
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
            // console.log(raw)
            this.#rawMaps.push(raw);
        }
    }

    process() {
        const cutSize = 8n;
        const batchSize = 0b1 << Number(cutSize);
        const bitLength = Number(this.#refs.sizeof);

        // calculate the position value for the specified range (it's for the refs table)
        const calcBatch = (start, end) => {
            let pos = (0b1n << BigInt(bitLength)) - 1n;
            let utf8Values = new Array(1000000).fill(0); // todo
            for (let i = start; i < end; ++i) {
                const { flags, utf8MappedTo } = this.#rawMaps[i];
                const index = i - start;
                const bitIndex = BigInt(Math.floor(index % bitLength));
                if (bitIndex > bitLength) {
                    console.log(bitIndex, index, bitLength, index / bitLength)
                    throw new Error(`Can't hold ${bitIndex} in ${bitLength} length`);
                }
                // const bit = (pos >>> (bitIndex - 1)) & 0b1;
                const bitMask = 0b1n << bitIndex;
                if (!isValid(flags)) {
                    pos &= ~bitMask;
                    for (let j = 0; j !== utf8MappedTo.length; ++j) {
                        utf8Values[j] = utf8MappedTo[j]; // todo
                    }
                }
            }

            if (pos >= (0b1 << bitLength)) {
                throw new Error(`We don't have the enough bits to store ${pos}; (${start}-${end})`);
            }
            return { pos, utf8Values };
        }

        for (let i = 0; i < this.#rawMaps.length; i += batchSize) {
            const { codePoint } = this.#rawMaps[i];
            const tbl1Loc = Number(codePoint) >>> Number(cutSize);
            const { pos, utf8Values } = calcBatch(i, i + batchSize);

            if (!this.#maps.isAll(Number(pos), utf8Values.length, 0)) {
                throw new Error(`Replacing is happening: ${pos}+${utf8Values.length}`);
            }

            this.#refs.set(tbl1Loc, Number(pos));
            if (pos !== ((0b1n << BigInt(bitLength)) - 1n)) { // contains mapped or disallowed values
                console.log(pos, utf8Values);
                this.#maps.setAt(Number(pos), utf8Values);
            }
        }
    }

    calculate() {
        const invalidFlag = 0xFFFF;
        let tryNum = 0;
        this.#magicRem = BigInt(Math.floor(this.#rawMaps.length / 1000));
        const nextAttempt = (...info) => {
            console.log(`Attempt #${tryNum} with magic rem of ${this.#magicRem} failed.`, ...info);
            // this.#refs.clear(invalidFlag);
            // this.#maps.clear();
            ++this.#magicRem;
            ++tryNum;
        };

        retry: for (; ;) {
            let refs = [];
            let maps = [];

            let pos = 0;
            for (let i = 0; i !== this.#rawMaps.length; ++i) {
                const { codePoint, flags, utf8MappedTo } = this.#rawMaps[i];
                const loc = codePoint % this.#magicRem;
                const val = isMapped(flags) ? flags | pos : flags;
                const mag = refs?.[loc] || invalidFlag;
                // console.log(codePoint, loc, this.#magicRem, mag, invalidFlag, flagsStatus(mag), this.#refs)
                if (mag !== val && mag !== invalidFlag) {
                    nextAttempt(codePoint, loc, val, flagsStatus(mag));
                    continue retry;
                }

                refs[loc] = val;

                pos += utf8MappedTo.length + 1;

                // Add the UTF-8 encoded mapped to code points:
                // for (const cu of utf8MappedTo) {
                //     this.#maps.push(cu);
                // }
                // this.#maps.push(0);
            }

            for (let pos = 0; pos !== refs.length; ++pos) {
                const value = refs.at(pos) || invalidFlag;
                this.#refs.set(pos, value);
            }
            for (let pos = 0; pos !== refs.length; ++pos) {
                const value = maps.at(pos) || invalidFlag;
                this.#maps.set(pos, value);
            }

            break;
        }
        console.log(`Success on #${tryNum}th try with magic rem of ${this.#magicRem}`);
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
        const refsBitLength = this.#refs.length * Number(this.#refs.sizeof);
        const mapsBitLength = this.#maps.length * Number(this.#maps.sizeof);
        console.log(`Reference Table size:`);
        console.log(`  Count       : ${this.#refs.length} * ${this.#refs.sizeof}`);
        console.log(`  in bytes    : ${refsBitLength / 8},`);
        console.log(`  in KibiBytes: ${Math.ceil(refsBitLength / 8 / 1024)} KiB\n`);
        console.log(`Map Table size:`);
        console.log(`  Count       : ${this.#maps.length} * ${this.#maps.sizeof}`);
        console.log(`  in bytes    : ${mapsBitLength / 8},`);
        console.log(`  in KibiBytes: ${Math.ceil(mapsBitLength / 8 / 1024)} KiB\n`);

        return `
/**
 * Attention: Auto-generated file, don't modify.
 * 
 *   Auto generated from:          ${path.basename(new URL(import.meta.url).pathname)}
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
    static constexpr std::array<${this.#refs.typeString}, ${this.#refs.length}ULL> idna_mapping_flags {
       ${this.serializeTable(this.#refs)}
    };
    
    
    /**
     * IDNA Mapped Code Points Table
     * 
     * Table size:
     *   - in bytes:      ${mapsBitLength / 8} B
     *   - in KibiBytes:  ${Math.ceil(mapsBitLength / 8 / 1024)} KiB
     */
    static constexpr std::array<${this.#maps.typeString}, ${this.#maps.length}ULL> idna_mappings {
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
