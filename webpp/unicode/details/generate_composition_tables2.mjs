/***
 * This file downloads UnicodeData.txt, and generates a C++ header file.
 *
 * More information about the syntax:
 *   https://www.unicode.org/reports/tr44/#Character_Decomposition_Mappings
 */

import * as path from "node:path";
import * as readme from "./readme.mjs";
import { getReadme } from "./readme.mjs";
import * as UnicodeData from "./UnicodeData.mjs";
import { fillEmpty, runClangFormat, writePieces } from "./utils.mjs";
import {getFullCompositionExclusions} from "./DerivedNormalizationProps.mjs";
import {getCanonicalDecompositions} from "./UnicodeData.mjs";

const outFile = `composition_tables.hpp`;

const start = async () => {
    await readme.download();

    // database file
    const compTables = new CompTable();
    await compTables.load();
    await createTableFile([compTables]);
    console.log("Done.");
};

class CP1 {
    #codePoint;
    #replacement;
    constructor(codePoint = 0, replacement = 0) {
        this.#codePoint = Number(codePoint) & 0xFF;
        this.#replacement = replacement;
    }

    static typeSize() {
        return 32;
    }

    static typeSizeBytes() {
        return this.typeSize() / 8;
    }

    static invalid() {
        return new CP1();
    }

    static renderStruct() {
        return `
            /**
             * First Code Point of the compositions
             */
            struct alignas(std::uint${this.typeSize()}_t) CP1 {
                std::uint8_t cp1_mask : 8 = 0; // a mask to identify if you have found the right code point
                char32_t replacement : 24 = 0U; // the composition code point
            };

            static_assert(sizeof(CP1) == ${this.typeSize() / 8}U, "Type size is not valid.");
        `;
    }

    render() {
        return `{0x${this.#codePoint.toString(16).toUpperCase()}U, 0x${this.#replacement.toString(16).toUpperCase()}U}`;
    }
}

class CP2 {
    #codePoint;
    #cp1Pos = 0;
    #rem = 1;
    constructor(codePoint = 0) {
        // this.#codePoint = Number(codePoint) & 0xff;
        this.#codePoint = codePoint;
    }

    setRem(cp1sLength) {
        // this.#rem = findSmallestMask(cp1sRaw);
        this.#rem = cp1sLength;
    }

    nextRem() {
        ++this.#rem;
    }

    setPosition(pos) {
        this.#cp1Pos = pos;
    }

    get position() {
        return this.#cp1Pos;
    }

    get rem() {
        return this.#rem;
    }

    static typeSize() {
        return 64;
    }

    static typeSizeBytes() {
        return this.typeSize() / 8;
    }

    static renderStruct() {
        return `
            /**
             * Second Code Point of the compositions
             */
            struct alignas(std::uint${this.typeSize()}_t) CP2 {
                char32_t cp2 = 0; // second code point, in order for you to check if you have found the right CP
                std::uint16_t cp1_pos = 0U; // start position
                std::uint16_t cp1_rem = 0U; // modulus value required to find your code CP1 of the composition
            };

            static_assert(sizeof(CP2) == ${this.typeSize() / 8}U, "Type size is not valid.");
        `;
    }

    static invalid() {
        return new CP2();
    }

    render() {
        if (this.#codePoint === 0) {
            return `{0, 0, 1}`;
        }
        return `{0x${this.#codePoint.toString(16).toUpperCase()}U, ${this.#cp1Pos}, ${this.#rem}}`;
    }
}

class CompTable {
    // these numbers are educated guesses from other projects, they're not that important!
    lastCP1 = 0n;
    lastCP2 = 0n;
    cp1s = [];
    cp2s = [];

    cp2sMask = 0n;
    cp2sRem = 0;

    constructor() {}

    async load() {
        // const info = await UnicodeData.maxCPs();
        // this.lastCP1 = info.maxCP1;
        // this.lastCP2 = info.maxCP2;
        // const neededBitsCP1 = bitCeil(this.lastCP1);
        // const neededBitsCP2 = bitCeil(this.lastCP2);
        // console.log(neededBitsCP1, neededBitsCP2, this.lastCP1, this.lastCP2);
        const exclusions = await getFullCompositionExclusions();
        const maps = (await getCanonicalDecompositions(true, (codePoint, {mappedTo}) => {
            // return !(Number(codePoint) in exclusions /* || mappedTo[0] in exclusions || mappedTo[1] in exclusions */);
            if (exclusions[Number(codePoint)] !== undefined) {
                return false;
            }
            // for (const cp of mappedTo) {
            //     if (exclusions[Number(cp)] !== undefined) {
            //         return false;
            //     }
            // }
            return true;
        })).data;
        const cp2sRaw = await UnicodeData.groupedByCP2(maps);
        const cp2sArr = [];
        for (const cp2Str in cp2sRaw) {
            const cp2Raw = BigInt(parseInt(cp2Str));
            cp2sArr.push(cp2Raw);
        }
        // this.cp2sMask = Number(findSmallestMask(cp2sArr));

        console.log("Exclusions:", Object.values(exclusions));

        let maxCP1sRequired = 0;
        this.cp2sRem = cp2sArr.length;
        retry: for (; ; ++this.cp2sRem) {
            this.cp1s = [];
            this.cp2s = [];
            for (const cp2Str in cp2sRaw) {
                const cp2Raw = parseInt(cp2Str);
                const cp2 = new CP2(cp2Raw);
                const cp1Vals = cp2sRaw[cp2Str];
                const cp1sRaw = cp1Vals.map((val) => val.cp1);
                cp2.setPosition(this.cp1s.length);

                cp2.setRem(cp1sRaw.length);
                nextRem: for (; ; cp2.nextRem()) {
                    const cp1sTemp = {};
                    for (const { cp1: cp1Raw, codePoint: replacement } of cp1Vals) {
                        const cp1 = new CP1(cp1Raw, replacement);
                        const pos = cp2.position + (Number(cp1Raw) % cp2.rem);
                        if (pos in cp1sTemp) {
                            // throw new Error(`Replacing: ${pos}`);
                            // console.log(
                            //     `Bad Rem:`,
                            //     cp2.rem,
                            //     cp2.position,
                            //     cp1Raw,
                            // );
                            continue nextRem;
                        }
                        cp1sTemp[pos] = cp1;

                        const maxPossible = cp2.position + cp2.rem;
                        if (maxPossible > maxCP1sRequired) {
                            maxCP1sRequired = maxPossible;
                        }
                    }

                    for (const posStr in cp1sTemp) {
                        const pos = parseInt(posStr);
                        this.cp1s[pos] = cp1sTemp[pos];
                    }
                    break;
                }

                // const pos = cp2Raw & Number(this.cp2sMask);
                const pos = Number(cp2Raw) % this.cp2sRem;
                if (pos in this.cp2s) {
                    console.log("Restarting: ", this.cp2sRem);
                    continue retry;
                }
                this.cp2s[pos] = cp2;
            }
            break;
        }
        console.log("Max CP1 table position required:", maxCP1sRequired, this.cp1s.length);
        if (this.cp1s.length !== maxCP1sRequired) {
            this.cp1s[maxCP1sRequired] = CP1.invalid();
            console.log("Max CP1 table set.");
        } else {
            console.log("No need to set Max CP1 table.");
        }
        this.cp2s = fillEmpty(this.cp2s, CP2.invalid());
        this.cp1s = fillEmpty(this.cp1s, CP1.invalid());
    }

    render() {
        const cp2sStr = this.cp2s.map((cp2) => cp2.render()).join(", ");
        const cp1sStr = this.cp1s.map((cp1) => cp1.render()).join(", ");
        return `
            namespace composition {

                ${/*static constexpr auto cp2s_mask = 0x${this.cp2sMask.toString(16).toUpperCase()}U;*/ ""}
                static constexpr auto cp2s_rem = ${this.cp2sRem}U;

                ${CP1.renderStruct()}
                ${CP2.renderStruct()}

                /**
                 * Size: ${((this.cp2s.length * CP2.typeSizeBytes()) / 1024).toFixed(1)} KiB
                 */
                static constexpr std::array<CP2, ${this.cp2s.length}ULL> cp2s {{
                    ${cp2sStr}
                }};

                /**
                 * Size: ${((this.cp1s.length * CP1.typeSizeBytes()) / 1024).toFixed(1)} KiB
                 */
                static constexpr std::array<CP1, ${this.cp1s.length}ULL> cp1s {{
                    ${cp1sStr}
                }};
            };
        `;
    }

    get totalValuesSize() {
        let sum = (this.cp1s.length * CP1.typeSizeBytes());
        sum += (this.cp2s.length * CP2.typeSizeBytes());
        return sum;
    }
}

const createTableFile = async (tables) => {
    const totalBytes = tables.reduce(
        (sum, table) => sum + table.totalValuesSize,
        0,
    );
    const readmeData = await getReadme();
    const competitor =
        (4352 + 67 * 257 * 2 /* 16bit */ + 1883 * 4) /* 32bit */ / 1024;
    const begContent = `
/**
 * Attention:
 *   Auto-generated file, don't modify this file; use the mentioned file below
 *   to re-generate this file with different options.
 *
 *   Auto generated from:                ${path.basename(new URL(import.meta.url).pathname)}
 *   Unicode UCD Database Creation Date: ${readmeData.date}
 *   This file's generation date:        ${new Date().toUTCString()}
 *   Unicode Version:                    ${readmeData.version}
 *   Total Table sizes in this file:
 *       - in bytes:      ${totalBytes} B
 *       - in KibiBytes:  ${Math.ceil(totalBytes / 1024)} KiB
 *   Some other implementations' total table size was ${competitor.toFixed(1)} KiB;
 *   So I saved ${Math.ceil(competitor - totalBytes / 1024)} KiB.
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
 *   Decomposition Mapping syntax used in the UCD Database:
 *       https://www.unicode.org/reports/tr44/#Character_Decomposition_Mappings
 */

#ifndef WEBPP_UNICODE_COMPOSITION_TABLES_HPP
#define WEBPP_UNICODE_COMPOSITION_TABLES_HPP

#include <array>
#include <cstdint>

namespace webpp::unicode::details {

`;

    const endContent = `

} // namespace webpp::unicode::details

#endif // WEBPP_UNICODE_COMPOSITION_TABLES_HPP
    `;

    let pieces = [begContent];
    for (const table of tables) {
        pieces.push(table.render());
    }
    pieces.push(endContent);
    await writePieces(outFile, pieces);
    await runClangFormat(outFile);
};

start();
