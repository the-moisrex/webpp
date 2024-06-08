/***
 * This file downloads UnicodeData.txt, and generates a C++ header file.
 *
 * More information about the syntax:
 *   https://www.unicode.org/reports/tr44/#Character_Decomposition_Mappings
 */

import * as readme from "./readme.mjs";
import * as UnicodeData from "./UnicodeData.mjs";
import {
    uint8,
    uint32,
    writePieces, runClangFormat, uint6, uint7,
} from "./utils.mjs";
import * as path from "node:path";
import {getReadme} from "./readme.mjs";
import {TablePairs} from "./table.mjs";
import {genIndexAddenda, genMaskedIndexAddenda} from "./modifiers.mjs";

const cccOutFile = `decomposition_tables.hpp`;

const start = async () => {
    await readme.download();

    // database file
    const decompTables = new CCCTables();
    await UnicodeData.parse(decompTables, UnicodeData.properties.decompositionType);
    decompTables?.process?.();
    await createTableFile([decompTables]);
    console.log('File processing completed.');
};

class CCCTables {
    tables = new TablePairs();
    name = "decomposition";
    description = "Decomposition Code Points";
    ignoreErrors = false;

    // these numbers are educated guesses from other projects, they're not that important!
    indices = {
        max: 4353 * 10,
        sizeof: uint32,
        description: `Decomposition`
    };
    values = {
        max: 65535,
        sizeof: uint8,
        description: `Decomposition`
    };
    lastZero = 0;


    constructor() {
        this.tables.init({
            disableComments: false,
            name: this.name,
            description: this.description,
            ignoreErrors: this.ignoreErrors,
            indices: this.indices,
            values: this.values,
            validateResults: true,
            genIndexAddenda: () => genMaskedIndexAddenda("index", uint7),
        });
    }

    /// proxy the function
    process() {
        this.tables.process();
        const lastZeroBucket = this.lastZero >>> this.tables.chunkShift;
        console.log("Trim indices table at: ", lastZeroBucket);
        this.tables.indices.trimAt(lastZeroBucket);
    }

    add(codePoint, value) {
        // calculating the last item that it's value is zero
        if (value !== 0) {
            this.lastZero = codePoint + 1;
        }
        return this.tables.add(codePoint, value);
    }

    render() {
        return this.processRendered(this.tables.render());
    }

    totalTablesSizeInBits() {
        return this.tables.totalTablesSizeInBits();
    }

    tests() {
        /// Sanity check: see if we have skipped adding some code points to the table
        const undefinedIndex = this.tables.data.findIndex(codePoint => codePoint === undefined);
        if (undefinedIndex !== -1) {
            throw new Error(`Error: Undefined Code Point. Undefined Index: ${undefinedIndex}, ${this.tables.data.at(undefinedIndex)}, ${this.data}`);
        }

        if (this.tables.data[0x1CE8] !== 1) {
            throw new Error(`Invalid parsing; data[0x1CE8]: ${this.tables.data[0x1CE8]}; length: ${this.tables.data?.length}`);
        }
    }

    processRendered(renderedTables) {
        return `
    /**
     * In "decomposition_index" table, any code point bigger than this number will have "zero" as its value;
     * so it's designed this way to reduce the table size.
     */
    static constexpr auto trailing_zero_deomps = 0x${this.lastZero.toString(16).toUpperCase()}UL;
    
${renderedTables}
        `;
    }
}

const createTableFile = async (tables) => {
    const totalBits = tables.reduce((acc, cur) => acc + cur.totalTablesSizeInBits(), 0);
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
    await writePieces(cccOutFile, pieces);
    await runClangFormat(cccOutFile);
};

start();
