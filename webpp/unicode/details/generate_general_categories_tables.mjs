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
import * as UnicodeData from "./UnicodeData.mjs";
import {runClangFormat, uint32, uint4, uint5, uint6, uint7, uint8, writePieces} from "./utils.mjs";
import {getGeneralCategories, makeGCEnum, renderEnum} from "./PropertyValueAliases.mjs";

const gcOutFile = `gc_tables.hpp`;

const generalCategories = makeGCEnum(await getGeneralCategories());
const excludeCategories = [
    "Other",
    "Control",
    "Format",
    // "Unassigned",
    "Private_Use",
    "Surrogate",
    "Letter",
    "Cased_Letter",
    "Lowercase_Letter",
    "Modifier_Letter",
    "Other_Letter",
    "Titlecase_Letter",
    "Uppercase_Letter",
    // "Mark",
    // "Spacing_Mark",
    // "Enclosing_Mark",
    // "Nonspacing_Mark",
    "Number",
    "Decimal_Number",
    "Letter_Number",
    "Other_Number",
    "Punctuation",
    "Connector_Punctuation",
    "Dash_Punctuation",
    "Close_Punctuation",
    "Final_Punctuation",
    "Initial_Punctuation",
    "Other_Punctuation",
    "Open_Punctuation",
    "Symbol",
    "Currency_Symbol",
    "Modifier_Symbol",
    "Math_Symbol",
    "Other_Symbol",
    "Separator",
    "Line_Separator",
    "Paragraph_Separator",
    "Space_Separator",
];

const replaceCategories = {
    // "Spacing_Mark": "Mark",
    // "Enclosing_Mark": "Mark",
    // "Nonspacing_Mark": "Mark",
};

const start = async () => {
    await readme.download();

    // database file
    const gcsTables = new GCTables();
    await UnicodeData.parse(gcsTables, UnicodeData.properties.gc);
    gcsTables?.process?.();
    await createTableFile(gcsTables);
    console.log("File processing completed.");
};

class GCTables {
    tables = new TablePairs();
    name = "gc"; // General Category
    description = "General Category";
    ignoreErrors = false;

    // these numbers are educated guesses from other projects, they're not that important!
    indices = {
        max: 4353 * 1000,
        sizeof: uint32,
        // splitInto: 6,
        splitInto: 5, // split the table this many tables
        description: `GC: General Category
These are the indices that are used to find which values from "gc_values" table correspond to a Unicode Code Point.`,
    };
    values = {
        max: 65535 * 1000,
        sizeof: uint8,
        description: `GC: General Category
These values are calculated and individually represent actual GC values, but they have no
valid order by themselves, and they only make sense if they're being used in conjunction with
the "gc_indices" table.
        `,
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
        this.tables.splitTables();
    }

    add(codePoint, value) {
        codePoint = BigInt(codePoint);
        value = Number(value);

        if (excludeCategories.includes(generalCategories[value])) {
            value = generalCategories['Unassigned'];
        }

        if (generalCategories[value] in replaceCategories) {
            // console.log(codePoint.toString(16), value, generalCategories[replaceCategories[generalCategories[value]]]);
            value = generalCategories[replaceCategories[generalCategories[value]]];
        }

        // calculating the last item that it's value is zero
        if (value !== generalCategories['Unassigned']) {
            // this.lastZero = codePoint + 1;
            // find the end of the batch, not just the last item
            this.lastZero =
                (((codePoint + 1n) >> this.tables.chunkShift) + 1n) <<
                this.tables.chunkShift;
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
        const undefinedIndex = this.tables.data.findIndex(
            (codePoint) => codePoint === undefined,
        );
        if (undefinedIndex !== -1) {
            throw new Error(
                `Error: Undefined Code Point. Undefined Index: ${undefinedIndex}, ${this.tables.data.at(undefinedIndex)}, ${this.data}`,
            );
        }

        if (this.tables.data[0x1ce8] !== 1) {
            throw new Error(
                `Invalid parsing; data[0x1CE8]: ${this.tables.data[0x1ce8]}; length: ${this.tables.data?.length}`,
            );
        }
    }

    processRendered(renderedTables) {
        return `
    /**
     * In "gc_index" table, any code point bigger than this number will have "zero" as its GC value;
     * so it's designed this way to reduce the table size.
     */
    static constexpr auto trailing_unassigned_gc = 0x${this.lastZero.toString(16).toUpperCase()}UL;

${renderedTables}
        `;
    }
}

const createTableFile = async (table) => {
    const tableContent = table.render();
    const totalBits = Number(table.totalTablesSizeInBits());
    const readmeData = await readme.getReadme();
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
 *
 * Details about the contents of this file can be found here:
 *   UTS #15: https://www.unicode.org/reports/tr15/
 *   UTS #44: https://www.unicode.org/reports/tr44/#UnicodeData.txt
 *   IDN FAQ: https://www.unicode.org/faq/idn.html
 *
 *   UCD Database Code Points (used the get the GC values and what not):
 *       ${UnicodeData.fileUrl}
 *   UCD README file (used to check the version and creation date):
 *       ${readme.fileUrl}
 *   Known Properties' Values are taken from:
 *       https://www.unicode.org/Public/UCD/latest/ucd/PropertyValueAliases.txt
 *${excludeCategories.length === 0 ? '' : `
 * 
 * Attention:
 *   These General Categories have been excluded from this file because we've decided
 *   these we don't need them for now:
 *       ${excludeCategories.join(",\n *       ")}
 *`}
 *${Object.keys(replaceCategories).length === 0 ? '' : `
 * 
 * Attention:
 *   These General Categories are being replaced because we've decided we only need them
 *   this way mostly due to performance and table size reasons:
 *       ${Object.keys(replaceCategories).map((lhs) => `${lhs} => ${replaceCategories[lhs]}`).join(",\n *       ")}
 *`}
 */

#ifndef WEBPP_UNICODE_GC_TABLES_HPP
#define WEBPP_UNICODE_GC_TABLES_HPP

#include <array>
#include <cstdint>

namespace webpp::unicode {

    /// Giving aliases to known values:
    /// Can be re-generating using:
    /// curl --silent https://www.unicode.org/Public/UCD/latest/ucd/PropertyValueAliases.txt | grep -oE '[^#]+' | awk 'BEGIN{FS=";"; i=0; print "enum struct general_category : std::uint8_t {"; } {gsub(/ */, "", $1); gsub(/ */, "", $3); gsub(/ */, "", $2); if ($1 == "gc") {print "  " $3 " = " i ",\\n  " $2 " = " i ",\\n"; i=i+1; }} END { print "};"; }'
    ${renderEnum("general_category", makeGCEnum(await getGeneralCategories()), "std::uint8_t")}

}

namespace webpp::unicode::details {
${tableContent}
} // namespace webpp::unicode::details

#endif // WEBPP_UNICODE_GC_TABLES_HPP
    `;

    await writePieces(gcOutFile, [content]);
    await runClangFormat(gcOutFile);
};

start();
