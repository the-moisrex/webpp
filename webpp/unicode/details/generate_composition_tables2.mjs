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
import {
    fillEmpty,
    findSmallestMask,
    runClangFormat,
    writePieces,
} from "./utils.mjs";

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
    #value;
    constructor(codePoint = 0, value = 0) {
        this.#codePoint = codePoint;
        this.#value = value;
    }

    static typeSize() {
        return 64;
    }

    static invalid() {
        return new CP1();
    }

    static renderStruct() {
        return `
            struct CP1 {
                char32_t cp1 = 0;
                char32_t value = 0U;
            };
        `;
    }

    render() {
        if (this.#codePoint == 0) {
            return `{}`;
        }
        return `{0x${this.#codePoint.toString(16).toUpperCase()}U, 0x${this.#value.toString(16).toUpperCase()}U}`;
    }
}

class CP2 {
    #codePoint;
    #cp1Pos = 0;
    #mask = 0;
    constructor(codePoint = 0) {
        this.#codePoint = codePoint;
    }

    setMask(cp1sRaw) {
        this.#mask = findSmallestMask(cp1sRaw);
    }

    setPosition(pos) {
        this.#cp1Pos = pos;
    }

    get position() {
        return this.#cp1Pos;
    }

    get mask() {
        return this.#mask;
    }

    static typeSize() {
        return 64;
    }

    static renderStruct() {
        return `
            struct CP2 {
                char32_t cp2 = 0;
                std::uint16_t cp1_pos = 0U;
                std::uint16_t cp1_mask = 0U;
            };
        `;
    }

    static invalid() {
        return new CP2();
    }

    render() {
        if (this.#codePoint == 0) {
            return `{}`;
        }
        return `{0x${this.#codePoint.toString(16).toUpperCase()}U, ${this.#cp1Pos}, 0x${this.#mask.toString(16).toUpperCase()}}`;
    }
}

class CompTable {
    // these numbers are educated guesses from other projects, they're not that important!
    lastMapped = 0n;
    cp1s = [];
    cp2s = [];

    cp2sMask = 0n;
    cp2sRem = 0;

    constructor() {}

    async load() {
        const cp2sRaw = await UnicodeData.groupedByCP2();
        const cp2sArr = [];
        for (const cp2Str in cp2sRaw) {
            const cp2Raw = BigInt(parseInt(cp2Str));
            cp2sArr.push(cp2Raw);
        }
        this.cp2sMask = Number(findSmallestMask(cp2sArr));
        this.cp2sRem = cp2sArr.length;
        retry: for (;;) {
            this.cp1s = [];
            this.cp2s = [];
            for (const cp2Str in cp2sRaw) {
                const cp2Raw = parseInt(cp2Str);
                const cp2 = new CP2(cp2Raw);
                const cp1Vals = cp2sRaw[cp2Str];
                const cp1sRaw = cp1Vals.map((val) => val.cp1);
                cp2.setMask(cp1sRaw);
                cp2.setPosition(this.cp1s.length);

                for (const { cp1: cp1Raw, codePoint: value } of cp1Vals) {
                    const cp1 = new CP1(cp1Raw, value);
                    const pos = cp2.position + Number(cp1Raw & cp2.mask);
                    if (pos in this.cp1s) {
                        throw new Error(`Replacing: ${pos}`);
                    }
                    this.cp1s[pos] = cp1;
                }

                // const pos = cp2Raw & Number(this.cp2sMask);
                const pos = (Number(cp2Raw) & this.cp2sMask) % this.cp2sRem;
                if (this.cp2s[pos] !== undefined) {
                    console.log("Restarting: ", this.cp2sRem);
                    ++this.cp2sRem;
                    continue retry;
                }
                this.cp2s[pos] = cp2;
            }
            break;
        }
        this.cp2s = fillEmpty(this.cp2s, CP2.invalid());
        this.cp1s = fillEmpty(this.cp1s, CP1.invalid());
    }

    render() {
        const cp2sStr = this.cp2s.map((cp2) => cp2.render()).join(", ");
        const cp1sStr = this.cp1s.map((cp1) => cp1.render()).join(", ");
        return `
            namespace composition {

                static constexpr auto cp2s_mask = 0x${this.cp2sMask.toString(16).toUpperCase()}U;
                static constexpr auto cp2s_rem = ${this.cp2sRem}U;

                ${CP1.renderStruct()}
                ${CP2.renderStruct()}

                /**
                 * Size: ${((this.cp2s.length * CP2.typeSize()) / 1024).toFixed(1)} KiB
                 */
                static constexpr std::array<CP2, ${this.cp2s.length}ULL> cp2s {{
                    ${cp2sStr}
                }};

                /**
                 * Size: ${((this.cp1s.length * CP2.typeSize()) / 1024).toFixed(1)} KiB
                 */
                static constexpr std::array<CP1, ${this.cp1s.length}ULL> cp1s {{
                    ${cp1sStr}
                }};
            };
        `;
    }

    get totalValuesSize() {
        let sum = this.cp1s.length * CP1.typeSize();
        sum += this.cp2s.length * CP2.typeSize();
        return sum;
    }
}

const createTableFile = async (tables) => {
    const totalBits = tables.reduce(
        (sum, table) => sum + table.totalValuesSize * 8,
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
 *       - in bits:       ${totalBits}
 *       - in bytes:      ${totalBits / 8} B
 *       - in KibiBytes:  ${Math.ceil(totalBits / 8 / 1024)} KiB
 *   Some other implementations' total table size was ${competitor.toFixed(1)} KiB;
 *   So I saved ${Math.ceil(competitor - totalBits / 8 / 1024)} KiB and a better a locality.
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
