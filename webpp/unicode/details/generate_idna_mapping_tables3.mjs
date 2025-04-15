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
import {
    char8_8, findSimilarSubRange, realSizeOf, recursiveLength,
    renderTableValues,
    runClangFormat, splitInto, TableTraits,
    uint16,
    uint32, uint4,
    uint5, uint6, uint8,
    writePieces
} from "./utils.mjs";
import * as IDNAMappingTable from "./IdnaMappingTable.mjs";
import {DISALLOWED, flagsStatus, isDisallowed, isMapped, NOT_MAPPED, VALID} from "./IdnaMappingTable.mjs";

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
    mappingsTable = [];
    lastDisallowed = 0n;

    constructor() {
        this.tables.init({
            name: "idna",
            description: "IDNA Mapping Index table",
            ignoreErrors: false,
            disableComments: false,
            validateResults: true,

            indices: {
                tableName: "idna_mapping_ref",
                sizeof: uint16,

                // split the indices table
                splitInto: 5,
                // breakpointsTableLimit: 3, // limit it to first 3 uncommon tables for breakpoints table
                description: `IDNA Mappings`,
            },
            values: {
                tableName: "idna_mapping_blocks", // table name
                sizeof: uint16,
                description: "Block values of the IDNA Mappings; the values of this table points to the idna_mappings table if it's not VALID or DISALLOWED specifically specified.",

                // convert values of the values table into booleans if possible, and put them into a different table
                minSplittingLength: 10,
                splitInto(val) {
                    return val === VALID ? true : val === DISALLOWED ? false : null;
                },

                /// it runs on print
                map(vals, info) {
                    for (let i = 0; i !== vals.length; ++i) {
                        switch (vals[i]) {
                            case DISALLOWED:
                                vals[i] = "disallowed";
                                break;
                            case VALID:
                                vals[i] = "valid";
                                break;
                        }
                    }
                }
            },
            genIndexAddenda: () => genSimpleIndexAddenda("index", uint6),
        });
    }

    /// proxy the function
    process() {
        this.tables.process();
        const lastZeroBucket = this.lastDisallowed >> this.tables.chunkShift;
        console.log(
            "Trim indices table at: ",
            lastZeroBucket,
            `(${this.lastDisallowed} >> ${this.tables.chunkShift})`,
        );
        this.tables.indices.trimAt(lastZeroBucket);
    }

    add(codePoint, {flags, mappedTo, utf8MappedTo}) {
        codePoint = BigInt(codePoint);

        // calculating the last item that it's value is zero
        if (isDisallowed(flags)) {
            // find the end of the batch, not just the last item
            const lastZero =
                (((codePoint + 1n) >> this.tables.chunkShift) + 1n) <<
                this.tables.chunkShift;
            if (lastZero > this.lastDisallowed) {
                this.lastDisallowed = lastZero;
            }
        }

        if (isMapped(flags)) {
            const utf8Vals = [...utf8MappedTo, 0];
            utf8Vals.codePoints = [];
            let blockPtr = findSimilarSubRange(utf8Vals, this.mappingsTable);
            if (blockPtr === null) {
                blockPtr = this.mappingsTable.length;
                this.mappingsTable.push(utf8Vals);
            }
            this.mappingsTable[blockPtr].codePoints.push(codePoint);
            this.tables.add(codePoint, blockPtr);
        } else {
            this.tables.add(codePoint, flags);
        }
    }

    render() {
        return `
    /**
     * The last code point that has a mapping status:
     */
    static constexpr auto idna_mapping_trailing_zero = 0x${this.lastDisallowed.toString(16).toUpperCase()}UL;

${this.tables.render()}
        `;
    }

    totalTablesSizeInBits() {
        return this.tables.totalTablesSizeInBits() + this.mappingsTableSizeInBits();
    }

    mappingsTableSizeInBits() {
        return BigInt(recursiveLength(this.mappingsTable)) * realSizeOf(char8_8);
    }
}

const createTableFile = async (table) => {
    const tableContent = table.render();
    const mappingsBits = Number(table.mappingsTableSizeInBits());
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
 * IDNA Mapping Tables are derived from here:
 *   ${IDNAMappingTable.fileUrl}
 * UCD README file (used to check the version and creation date):
 *   ${readme.fileUrl}
 */

#ifndef WEBPP_UNICODE_IDNA_MAPPINGS_TABLES_HPP
#define WEBPP_UNICODE_IDNA_MAPPINGS_TABLES_HPP

#include <array>
#include <cstdint>

namespace webpp::unicode::details {

    static constexpr auto last_disallowed = static_cast<char32_t>(0x${table.lastDisallowed.toString(16).toUpperCase()});

    [[maybe_unused]] static constexpr ${table.tables.values.type.description} ${flagsStatus(NOT_MAPPED)} = 0b${NOT_MAPPED.toString(2)}U;
    static constexpr ${table.tables.values.type.description} ${flagsStatus(VALID)} = 0b${VALID.toString(2)}U;
    static constexpr ${table.tables.values.type.description} ${flagsStatus(DISALLOWED)} = 0b${DISALLOWED.toString(2)}U;
    

${tableContent}

    /**
     * IDNA Mappings
     *
     * Table size:
     *   - in bits:       ${mappingsBits}
     *   - in bytes:      ${mappingsBits / 8} B
     *   - in KibiBytes:  ${(mappingsBits / 8 / 1024).toFixed(2)} KiB
     */
    ${renderTableValues({
        name: "idna_mappings",
        type: char8_8,
        printableValues: table.mappingsTable,
        len: recursiveLength(table.mappingsTable),
        map: val => {
            val.inline_comment = val.codePoints.map(cp => cp.toString(16).toUpperCase()).join(", ")
            return val;
        }
    })}
} // namespace webpp::unicode::details

#endif // WEBPP_UNICODE_IDNA_MAPPINGS_TABLES_HPP
    `;
    await writePieces(IDNAMappingTable.outFilePath, [content]);
    await runClangFormat(IDNAMappingTable.outFilePath);
};

start();
