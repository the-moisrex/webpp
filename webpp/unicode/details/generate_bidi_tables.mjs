/***
 * This file downloads UnicodeData.txt, and generates a C++ header file.
 *
 * Details on parsing this file can be found here:
 * UTS #44: https://www.unicode.org/reports/tr44/#UnicodeData.txt
 */
import * as path from "node:path";
import {genSimpleIndexAddenda} from "./modifiers.mjs";
import * as readme from "./readme.mjs";
import {getReadme} from "./readme.mjs";
import {TablePairs} from "./table.mjs";
import * as DerivedBidiClass from "./DerivedBidiClass.mjs";
import {runClangFormat, uint32, uint5, uint8, writePieces} from "./utils.mjs";
import * as UnicodeData from "./UnicodeData.mjs";

const bidiOutFile = `bidi_tables.hpp`;

const start = async () => {
    await readme.download();

    // database file
    const bidisTables = new BidiTables();
    await DerivedBidiClass.parse(bidisTables);
    bidisTables?.process?.();
    await createTableFile(bidisTables);
    console.log("File processing completed.");
};

class BidiTables {
    tables = new TablePairs();
    name = "bidi"; // Bidirectional Class
    description = "Bidirectional Class";
    ignoreErrors = false;

    // these numbers are educated guesses from other projects, they're not that important!
    indices = {
        max: 4353 * 100,
        sizeof: uint32,

        // split the indices table
        splitInto: 20,
        // breakpointsTableLimit: 3, // limit it to first 3 uncommon tables for breakpoints table

        description: `Bidi: Bidirectional Class`,
    };
    values = {
        max: 65535 * 100,
        sizeof: uint8,
        description: `Bidi: Bidirectional Class Values`,
    };
    lastZero = 0n;

    constructor() {
        this.tables.init({
            disableComments: false,
            name: this.name,
            description: this.description,
            ignoreErrors: this.ignoreErrors,
            indices: this.indices,
            values: this.values,
            validateResults: true,
            genIndexAddenda: () => genSimpleIndexAddenda("index", uint5),
        });
    }

    /// proxy the function
    process() {
        this.tables.process();
        const lastZeroBucket = this.lastZero >> this.tables.chunkShift;
        console.log(
            "Trim indices table at: ",
            lastZeroBucket,
            `(${this.lastZero} >> ${this.tables.chunkShift})`,
        );
        this.tables.indices.trimAt(lastZeroBucket);
    }

    add(codePoint, value) {
        codePoint = BigInt(codePoint);
        value = Number(value);

        // calculating the last item that it's value is zero
        if (value !== 0) {
            // this.lastZero = codePoint + 1;
            // find the end of the batch, not just the last item
            const lastZero =
                (((codePoint + 1n) >> this.tables.chunkShift) + 1n) <<
                this.tables.chunkShift;
            if (lastZero > this.lastZero) {
                this.lastZero = lastZero;
            }
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
        // todo
    }

    processRendered(renderedTables) {
        return `
    /**
     * In "bidi_index" table, any code point bigger than this number will have "None" as its Bidi value;
     * so it's designed this way to reduce the table size.
     */
    static constexpr auto trailing_zero_bidis = 0x${this.lastZero.toString(16).toUpperCase()}UL;

${renderedTables}
        `;
    }
}

const createTableFile = async (table) => {
    const tableContent = table.render();
    const totalBits = Number(table.totalTablesSizeInBits());
    const readmeData = await readme.getReadme();
    const competition = 16.98;
    const saved = competition - totalBits / 8 / 1024;
    const content = `
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
 *       - in KibiBytes:  ${(totalBits / 8 / 1024).toFixed(2)} KiB
 *   Some other implementations' total table size was ${competition.toFixed(2)} KiB;
 *   So I have ${saved > 0 ? `saved` : `wasted`} ${Math.abs(saved).toFixed(2)} KiB.
 *   Some other implementations use binary search, which is not be the fastest solution.
 *
 * Details about the contents of this file can be found here:
 *   UTS #15: https://www.unicode.org/reports/tr15/
 *   UTS #44: https://www.unicode.org/reports/tr44/#UnicodeData.txt
 *   IDN FAQ: https://www.unicode.org/faq/idn.html
 *
 *   Derived Bidi Classes from UCD Database Code Points:
 *       ${DerivedBidiClass.fileUrl}
 *       ${UnicodeData.fileUrl}
 *   UCD README file (used to check the version and creation date):
 *       ${readme.fileUrl}
 */

#ifndef WEBPP_UNICODE_BIDI_TABLES_HPP
#define WEBPP_UNICODE_BIDI_TABLES_HPP

#include <array>
#include <cstdint>

namespace webpp::unicode::details {

${tableContent}

} // namespace webpp::unicode::details

#endif // WEBPP_UNICODE_BIDI_TABLES_HPP
    `;
    await writePieces(bidiOutFile, [content]);
    await runClangFormat(bidiOutFile);
};

start();
