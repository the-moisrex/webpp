/***
 * This file downloads UnicodeData.txt, and generates a C++ header file.
 *
 * Details on parsing this file can be found here:
 * UTS #44: https://www.unicode.org/reports/tr44/#UnicodeData.txt
 */
import * as path from "node:path";
import {genSimpleTwoTableIndexAddenda} from "./modifiers.mjs";
import * as readme from "./readme.mjs";
import {TablePairs} from "./table.mjs";
import {
    bool32,
    char8_8, findOrInsert, findSimilarSubRange, packBoolsIntoInts, realSizeOf, recursiveLength,
    renderTableValues,
    runClangFormat,
    sizeOf,
    toHexString,
    uint16,
    uint32,
    uint4,
    uint5,
    uint6,
    uint7,
    uint8,
    writePieces
} from "./utils.mjs";
import * as IDNAMappingTable from "./IdnaMappingTable.mjs";
import {DISALLOWED, flagsStatus, isDisallowed, isMapped, NOT_MAPPED, refPrinter, VALID} from "./IdnaMappingTable.mjs";

const verbose = process.argv.includes("--verbose");

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
    tablePickMask;
    boolsTable = [];
    boolsTablePacked;
    packedAmount = 0;

    #maxMappedLength = 0;
    #maxMappedDiff = 0;
    #maxMappedFactor = 0;
    #maxMappedCP = {};

    constructor() {
        const self = this;
        this.tables.init({
            name: "idna",
            description: "IDNA Mapping Index table",
            ignoreErrors: false,
            disableComments: false,
            validateResults: false, // the values table will not contain all the values

            // Put all the ranges that are not mapped into a different table
            modify({start, end, modifier, values}) {
                let areAllMapped = true;
                for (const val of values) {
                    areAllMapped &&= val === VALID || val === DISALLOWED;
                }
                if (areAllMapped) {
                    const pos = BigInt(findOrInsert(values.map(val => val !== DISALLOWED), self.boolsTable));
                    ++self.packedAmount;
                    if (verbose) {
                        console.log(' ', self.packedAmount, "Packed DISALLOWED/VALID into bools table: ", start, end, modifier);
                    }

                    // modifier = modifier.clone({pos: modifier.pos | BigInt(self.tablePickMask)});
                    // modifier.postModify = mod => mod.clone({pos: mod.pos | BigInt(self.tablePickMask)});
                    modifier.set({use_second_table: true, pos});
                    return {
                        inserts: [],
                        modifier,
                    }
                }
                return {};
            },

            indices: {
                tableName: "idna_mapping_ref",
                sizeof: uint16,

                // split the indices table
                splitInto: 4,
                splittingSingleValue: false,
                // breakpointsTableLimit: 3, // limit it to first 3 uncommon tables for breakpoints table
                description: `IDNA Mappings`,

                // add "iblt"
                map(vals, info) {

                    // vals may be Uint16Array and what not, and they can't hold strings
                    let res = Array.isArray(vals) ? vals : []; 
                    res.trailing_comment = vals.trailing_comment;

                    for (let i = 0; i !== vals.length; ++i) {
                        res[i] = refPrinter(vals[i], self.tablePickMask, 'iblt');
                    }

                    return res;
                }
            },
            values: {
                tableName: "idna_mapping_blocks", // table name
                sizeof: uint16,
                description: "Block values of the IDNA Mappings; the values of this table points to the idna_mappings table if it's not VALID or DISALLOWED specifically specified.",

                /// it runs on print
                map(vals, info) {
                    let res = Array.isArray(vals) ? vals : [];
                    for (let i = 0; i !== vals.length; ++i) {
                        switch (vals[i]) {
                            case DISALLOWED:
                                res[i] = "disallowed";
                                break;
                            case VALID:
                                res[i] = "valid";
                                break;
                            default:
                                res[i] = vals[i];
                        }
                    }
                    return res;
                }
            },
            genIndexAddenda: () => {
                const ret = genSimpleTwoTableIndexAddenda("index", uint5);
                ret.noStruct = true;
                return ret;
            },
        });

        this.tablePickMask = 0b1 << (Number(this.tables.indices.sizeof) - 1);
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

        this.boolsTablePacked = this.getBoolsTable();
    }

    add(codePoint, {flags, mappedTo, utf8MappedTo, utf8CodePoint}) {
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

            const strBlockPtr = recursiveLength(this.mappingsTable, blockPtr);
            this.tables.add(codePoint, strBlockPtr);

            const curLen = utf8MappedTo.length;
            const curCPLen = utf8CodePoint.length;
            const curDiff = curLen - curCPLen;
            const curFactor = curLen / curCPLen;
            if (curLen > this.#maxMappedLength) {
                this.#maxMappedLength = curLen;
            }
            if (curDiff > this.#maxMappedDiff) {
                this.#maxMappedDiff = curDiff;
                this.#maxMappedFactor = Math.ceil(curFactor);
                this.#maxMappedCP = {
                    codePoint,
                    mappedTo,
                    utf8MappedTo,
                    utf8CodePoint
                };
                // console.log(curDiff, curFactor, this.#maxMappedCP);
            }
        } else {
            this.tables.add(codePoint, flags);
        }
    }

    render() {
        return `
    /// Max UTF-8 IDNA mapping length change
    /// When Code Points are being mapped, this is the maximum length change.
    /// This can be used to calculate the necessary space required for mapping a string.
    /// ${this.#maxMappedCP.codePoint.toString(16).toUpperCase()} => ${this.#maxMappedCP.mappedTo.map(val => Number(val).toString(16).toUpperCase()).join(", ")}
    /// ${this.#maxMappedCP.utf8CodePoint.join(', ')} => ${this.#maxMappedCP.utf8MappedTo.join(", ")}
    static constexpr std::size_t max_mapping_factor = ${this.#maxMappedFactor}UL; // times of the original string
    static constexpr std::size_t max_mapping_diff = ${this.#maxMappedDiff}UL;
    static constexpr std::size_t max_mapping_length = ${this.#maxMappedLength}UL;

    /**
     * The last code point that has a mapping status:
     */
    static constexpr auto idna_mapping_trailing_zero = 0x${this.lastDisallowed.toString(16).toUpperCase()}UL;

    // Pick the table with this mask (between bools table and the block table)
    static constexpr ${this.tables.indices.type.description} table_pick_mask = 0b${this.tablePickMask.toString(2)}U;
    static constexpr auto iblt = table_pick_mask; // (IDNA Boolean Table) shortcut

${this.tables.render()}
        `;
    }

    totalTablesSizeInBits() {
        return this.tables.totalTablesSizeInBits() + this.mappingsTableSizeInBits() + this.boolsTableSizeInBits();
    }

    mappingsTableSizeInBits() {
        return BigInt(recursiveLength(this.mappingsTable)) * realSizeOf(char8_8);
    }

    boolsTableSizeInBits() {
        return BigInt(this.boolsTablePacked.length) * sizeOf(this.boolsTablePacked.type);
    }

    getBoolsTable() {
        const type = bool32;
        const tbl = packBoolsIntoInts(this.boolsTable, sizeOf(type));
        tbl.type = type;
        return tbl;
    }
}

const createTableFile = async (table) => {
    const tableContent = table.render();
    const mappingsBits = Number(table.mappingsTableSizeInBits());
    const boolsBits = Number(table.boolsTableSizeInBits());
    const totalBits = Number(table.totalTablesSizeInBits());
    const readmeData = await readme.getReadme();
    const competition = 84.47;
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
#include <string_view>

namespace webpp::unicode::idna::details {

    static constexpr auto last_disallowed = static_cast<char32_t>(0x${table.lastDisallowed.toString(16).toUpperCase()});

    [[maybe_unused]] static constexpr ${table.tables.values.type.description} ${flagsStatus(NOT_MAPPED)} = 0b${NOT_MAPPED.toString(2)}U;
    static constexpr ${table.tables.values.type.description} ${flagsStatus(VALID)} = 0b${VALID.toString(2)}U;
    static constexpr ${table.tables.values.type.description} ${flagsStatus(DISALLOWED)} = 0b${DISALLOWED.toString(2)}U;
    

${tableContent}

    /**
     * IDNA Mapping Status Table
     *
     * Table size:
     *   - in bits:       ${boolsBits}
     *   - in bytes:      ${boolsBits / 8} B
     *   - in KibiBytes:  ${(boolsBits / 8 / 1024).toFixed(2)} KiB
     */
    ${renderTableValues({
        name: "idna_mappings_bools",
        type: table.boolsTablePacked.type,
        printableValues: table.boolsTablePacked,
    })}
    
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
            const codePoints = val.codePoints;
            val = val.map(toHexString);
            val.inline_comment = codePoints.map(cp => cp.toString(16).toUpperCase()).join(", ");
            return val;
        }
    })}
} // namespace webpp::unicode::idna::details

#endif // WEBPP_UNICODE_IDNA_MAPPINGS_TABLES_HPP
    `;
    await writePieces(IDNAMappingTable.outFilePath, [content]);
    await runClangFormat(IDNAMappingTable.outFilePath);
};

start();
