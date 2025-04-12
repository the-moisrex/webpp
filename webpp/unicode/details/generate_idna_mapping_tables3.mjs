/***
 * This file downloads UnicodeData.txt, and generates a C++ header file.
 *
 * Details on parsing this file can be found here:
 * UTS #44: https://www.unicode.org/reports/tr44/#UnicodeData.txt
 */
import * as path from "node:path";
import {genSimpleIndexAddenda} from "./modifiers.mjs";
import * as readme from "./readme.mjs";
import {TablePairs} from "./table.mjs";
import {char8, char8_8, runClangFormat, uint16, uint32, uint5, uint8, writePieces} from "./utils.mjs";
import * as IDNAMappingTable from "./IdnaMappingTable.mjs";

const start = async () => {
    await readme.download();

    // database file
    const table = new IDNAMappings();
    await IDNAMappingTable.parseIDNAMappingTable(table);
    table?.process?.();
    await createTableFile(table);
    console.log("File processing completed.");
};

class IDNAMappings {
    tables = new TablePairs();
    lastZero = 0n;

    constructor() {
        this.tables.init({
            name: "IDNA Mapping",
            description: "IDNA Mapping Index table",
            ignoreErrors: false,
            disableComments: false,
            validateResults: true,

            indices: {
                tableName: "idna_mapping_ref",
                sizeof: uint32,

                // split the indices table
                splitInto: 20,
                // breakpointsTableLimit: 3, // limit it to first 3 uncommon tables for breakpoints table

                description: `IDNA Mappings`,
            },
            blocks: {
                sizeof: uint16,
                description: "Block values of the IDNA Mappings"
            },
            values: {
                tableName: "idna_mappings", // table name
                sizeof: char8_8, // turning it into a string
                description: `IDNA Mapping Values`,
                map(val, {name}) {
                    // todo: add comments
                }
            },
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
        return `
    /**
     * The last code point that has a mapping status:
     */
    static constexpr auto idna_mapping_trailing_zero = 0x${this.lastZero.toString(16).toUpperCase()}UL;

${this.tables.render()}
        `;
    }

    totalTablesSizeInBits() {
        return this.tables.totalTablesSizeInBits();
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
 *   UTS #46: https://www.unicode.org/reports/tr46/#IDNA_Mapping_Table
 *   IDN FAQ: https://www.unicode.org/faq/idn.html
 *
 * IDNA Mapping Tables are derrived from here:
 *   ${IDNAMappingTable.fileUrl}
 * UCD README file (used to check the version and creation date):
 *   ${readme.fileUrl}
 */

#ifndef WEBPP_UNICODE_IDNA_MAPPINGS_TABLES_HPP
#define WEBPP_UNICODE_IDNA_MAPPINGS_TABLES_HPP

#include <array>
#include <cstdint>

namespace webpp::unicode::details {

${tableContent}

} // namespace webpp::unicode::details

#endif // WEBPP_UNICODE_IDNA_MAPPINGS_TABLES_HPP
    `;
    await writePieces(IDNAMappingTable.outFilePath, [content]);
    await runClangFormat(IDNAMappingTable.outFilePath);
};

start();
