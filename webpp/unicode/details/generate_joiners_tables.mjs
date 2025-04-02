/***
 * This file downloads DerivedJoiningType.txt, and generates a C++ header file.
 */
import * as path from "node:path";
import * as readme from "./readme.mjs";
import * as DerivedJoiningType from "./DerivedJoiningType.mjs";
import {genSimpleIndexAddenda} from "./modifiers.mjs";
import {TablePairs} from "./table.mjs";
import {runClangFormat, uint10, uint3, uint32, uint4, uint5, uint6, uint8, uint9, writePieces} from "./utils.mjs";
import {JoiningTypes} from "./DerivedJoiningType.mjs";

const joinersOutFile = `joiners_tables.hpp`;

const config = {
    // exclude these join types from the generated tables (will be replaced by non-joiner)
    excludeJoinTypes: [
        JoiningTypes["Transparent"],
        JoiningTypes["Join_Causing"],
    ]
}

const start = async () => {
    await readme.download();

    // database file
    const joinersTables = new JoinersTables();
    await DerivedJoiningType.parse(joinersTables);
    joinersTables?.process?.();
    await createTableFile(joinersTables);
    console.log("File processing completed.");
};

class JoinersTables {
    tables = new TablePairs();
    name = "joiners"; // Joining_Type
    description = "Joining_Type";
    ignoreErrors = false;

    // these numbers are educated guesses from other projects, they're not that important!
    indices = {
        max: 4353 * 10,
        sizeof: uint32,

        // split the indices table
        splitInto: 6,
        // breakpointsTableLimit: 3, // limit it to first 3 uncommon tables for breakpoints table

        description: `Joiners: Joining_Type`,
    };
    values = {
        max: 65535,
        sizeof: uint8,
        description: `Joiners: Joining_Type Values`,
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
            genIndexAddenda: () => genSimpleIndexAddenda("index", uint4),
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

    #lastCodePoint = -1n;
    add(codePoint, value) {
        codePoint = BigInt(codePoint);
        value = Number(value);

        if (codePoint - 1n !== this.#lastCodePoint) {
            throw new Error(`CodePoint ${codePoint} missed a step ${this.#lastCodePoint}`);
        }
        this.#lastCodePoint = codePoint;

        if (config.excludeJoinTypes.includes(value)) {
            value = JoiningTypes["Non_Joining"];
        }

        if (codePoint === 0x10F33n && value !== JoiningTypes["Right_Joining"]) {
            throw new Error(`CodePoint ${codePoint} does not match joining ${value}`);
        }

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
     * In "joiners_index" table, any code point bigger than this number will have "Non_Joining" as its Joiners value;
     * so it's designed this way to reduce the table size.
     */
    static constexpr auto trailing_zero_joiners = 0x${this.lastZero.toString(16).toUpperCase()}UL;

${renderedTables}
        `;
    }
}

const createTableFile = async (table) => {
    const tableContent = table.render();
    const totalBits = Number(table.totalTablesSizeInBits());
    const readmeData = await readme.getReadme();
    const competition = 1.55;
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
 *   Arabic Shaping code point lists:
 *       ${DerivedJoiningType.fileUrl}
 *   UCD README file (used to check the version and creation date):
 *       ${readme.fileUrl}
 *${config.excludeJoinTypes.length === 0 ? `` : `
 *
 * Attention:
 *   These 'Joining_Type's are excluded from these tables and have been
 *   replaced with 'Non_Joining':
 *       ${config.excludeJoinTypes.map(type => JoiningTypes[type]).join(', ')}
 *  `}
 */

#ifndef WEBPP_UNICODE_JOINERS_TABLES_HPP
#define WEBPP_UNICODE_JOINERS_TABLES_HPP

#include <array>
#include <cstdint>

namespace webpp::unicode::details {

${tableContent}

} // namespace webpp::unicode::details

#endif // WEBPP_UNICODE_JOINERS_TABLES_HPP
    `;
    await writePieces(joinersOutFile, [content]);
    await runClangFormat(joinersOutFile);
};

start();
