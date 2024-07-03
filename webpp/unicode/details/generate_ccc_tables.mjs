/***
 * This file downloads UnicodeData.txt, and generates a C++ header file.
 *
 * Details on parsing this file can be found here:
 * UTS #44: https://www.unicode.org/reports/tr44/#UnicodeData.txt
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
import {genMaskedIndexAddenda} from "./modifiers.mjs";

const cccOutFile = `ccc_tables.hpp`;

const start = async () => {
    await readme.download();

    // database file
    const cccsTables = new CCCTables();
    await UnicodeData.parse(cccsTables, UnicodeData.properties.ccc);
    cccsTables?.process?.();
    await createTableFile([cccsTables]);
    console.log('File processing completed.');
};

class CCCTables {
    tables = new TablePairs();
    name = "ccc"; // Canonical Combining Class
    description = "Canonical Combining Class";
    ignoreErrors = false;

    // these numbers are educated guesses from other projects, they're not that important!
    indices = {
        max: 4353 * 10,
        sizeof: uint32,
        description: `CCC: Canonical Combining Class
These are the indices that are used to find which values from "ccc_values" table correspond to a Unicode Code Point.`
    };
    values = {
        max: 65535,
        sizeof: uint8,
        description: `CCC: Canonical Combining Class
These values are calculated and individually represent actual CCC values, but they have no
valid order by themselves, and they only make sense if they're being used in conjunction with
the "ccc_indices" table.
        `
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
            // this.lastZero = codePoint + 1;
            // find the end of the batch, not just the last item
            this.lastZero = (((codePoint + 1) >>> this.tables.chunkShift) + 1) << this.tables.chunkShift;
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
     * In "ccc_index" table, any code point bigger than this number will have "zero" as its CCC value;
     * so it's designed this way to reduce the table size.
     */
    static constexpr auto trailing_zero_cccs = 0x${this.lastZero.toString(16).toUpperCase()}UL;
    
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
 *   Some other implementations' total table size was 21 KiB;
 *   So I saved ${Math.ceil(21 - totalBits / 8 / 1024)} KiB.
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
 */
 
#ifndef WEBPP_UNICODE_CCC_TABLES_HPP
#define WEBPP_UNICODE_CCC_TABLES_HPP

#include <array>
#include <cstdint>

namespace webpp::unicode::details {

`;

    const endContent = `
} // namespace webpp::unicode::details

#endif // WEBPP_UNICODE_CCC_TABLES_HPP
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
