/***
 * This file downloads UnicodeData.txt, and generates a C++ header file.
 *
 * More information about the syntax:
 *   https://www.unicode.org/reports/tr44/#Character_Decomposition_Mappings
 */

import * as readme from "./readme.mjs";
import {
    getReadme
} from "./readme.mjs";
import * as UnicodeData from "./UnicodeData.mjs";
import {
    chunked, realSizeOf,
    runClangFormat, symbolOf,
    writePieces
} from "./utils.mjs";
import * as path from "node:path";
import {
    CanonicalComposition, genCompositionModifier
} from "./composition.mjs";

const outFile = `composition_tables.hpp`;

const start = async () => {
    await readme.download();

    // database file
    const compTables = new CompTable();
    await compTables.load();
    compTables.optimize();
    await createTableFile([compTables]);
    console.log('Done.');
};

class CompTable {

    // these numbers are educated guesses from other projects, they're not that important!
    #canonicalCompositions = null;

    constructor() {
        this.#canonicalCompositions = new CanonicalComposition();
    }

    async load() {
        this.#canonicalCompositions.embedCodePointCanonical = true;
        this.reset(8);
        await this.#canonicalCompositions.load();
    }

    reset (size) {
        const {chunkSize, chunkMask, chunkShift} = chunked(size);
        this.#canonicalCompositions.chunkShift = chunkShift;
        this.#canonicalCompositions.chunkSize = chunkSize;
        this.#canonicalCompositions.chunkMask = chunkMask;
        this.#canonicalCompositions.calculateMagicalTable();
    }

    optimize () {
        let history = [];
        for (let chunkSize = 1; chunkSize <= 15; chunkSize++) {
            try {
                this.reset(chunkSize);
                const table = {...this.#canonicalCompositions.magicTable};
                history.push({
                    chunkSize,
                    magicTable: table,
                    tableLength: table.length,
                    tableLastMagicCode: table.lastMagicCode,
                    lastMapped: this.#canonicalCompositions.lastMapped,
                    lastMappedBucket: this.#canonicalCompositions.lastMappedBucket,
                    lastShiftedMagicCodePoint: this.#canonicalCompositions.lastShiftedMagicCode,
                });
                console.log(`Valid chunk size: ${chunkSize}`);
            } catch (err) {
                console.log(`Invalid chunk size: ${chunkSize}`);
                history.push({
                    chunkSize,
                    error: err,
                })
            }
        }
        const best = history.filter(a => !a.error).toSorted((a, b) => Number(a.lastShiftedMagicCodePoint) - Number(b.lastShiftedMagicCodePoint));
        console.log(history.map(inp_info => {
            const info = {...inp_info};
            delete info.magicTable;
            return info;
        }));
        console.log("--------------------- <Best> --------------------------");
        console.log(best[0]);
        console.log("--------------------- </Best> --------------------------");
        this.reset(best[0].chunkSize);
        this.#renderTable(best[0]);
    }

    get typeString() {
        return this.#type.description;
    }

    #rendered = "";
    #type;
    #size;
    values = [];
    #renderTable(magicalTable) {
        this.#type = symbolOf(magicalTable.chunkSize);
        this.#size = realSizeOf(this.#type);
        this.values = this.#canonicalCompositions.getCodePointTable();
        if (this.values.length !== magicalTable.lastShiftedMagicCodePoint) {
            throw new Error(`Something went wrong with the table size.`);
        }
        this.#rendered = `
    static constexpr std::array<std::uint32_t, ${this.values.length}ULL> canonical_composition_magic_table {
        ${this.values.join(", ")}
    };
        `;
    }

    render () {
        return this.#rendered;
    }

    get totalValuesSize () {
        return this.values.length;
    }
}

const createTableFile = async (tables) => {
    const totalBits = tables.reduce(table => table.totalValuesSize, 0);
    const readmeData = await getReadme();
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
 *   Some other implementations' total table size was 73.4 KiB;
 *   So I saved ${Math.ceil(73.4 - totalBits / 8 / 1024)} KiB and a better a locality.
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
 
#ifndef WEBPP_UNICODE_DECOMPOSITION_TABLES_HPP
#define WEBPP_UNICODE_DECOMPOSITION_TABLES_HPP

#include <array>
#include <cstdint>
#include <string_view>
#include "../../utils/bits.hpp"

namespace webpp::unicode::details {

`;

    const endContent = `
    
} // namespace webpp::unicode::details

#endif // WEBPP_UNICODE_DECOMPOSITION_TABLES_HPP
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
