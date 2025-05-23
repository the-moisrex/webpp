/***
 * This file downloads UnicodeData.txt, and generates a C++ header file.
 *
 * Details on parsing this file can be found here:
 * UTS #44: https://www.unicode.org/reports/tr44/#UnicodeData.txt
 */
import * as path from "node:path";
import { genSimpleIndexAddenda } from "./modifiers.mjs";
import * as readme from "./readme.mjs";
import { getReadme } from "./readme.mjs";
import { TablePairs } from "./table.mjs";
import * as UnicodeData from "./UnicodeData.mjs";
import {fillEmptyObject, runClangFormat, runCmd, uint32, uint7, uint16, uint8, writePieces} from "./utils.mjs";
import { getQCs, getQuickChecks } from "./DerivedNormalizationProps.mjs";

const cccOutFile = `ccc_tables.hpp`;
const embedQuickCheckTables = true;
const excludeDecompositionOnly = true;
const excludeKompatibility = true;

const start = async () => {
    await readme.download();

    const cccsTables = new CCCTables();
    if (embedQuickCheckTables) {
        const qcs = fillEmptyObject(await getQuickChecks(), 0b0) ;
        let data = [];
        for (const codePointStr in qcs) {
            const codePoint = parseInt(codePointStr);
            if (isNaN(codePoint)) {
                continue;
            }
            const qcCode = getQCs(qcs[codePointStr], excludeKompatibility, excludeDecompositionOnly);
            // console.log(codePoint, qcCode, qcs[codePointStr]);
            data[codePoint] = qcCode << 8;
        }
        await UnicodeData.parse({
            add(codePoint, value) {
                data[codePoint] |= Number(value);
            }
        }, UnicodeData.properties.ccc);
        for (const codePoint in data) {
            cccsTables.add(codePoint, data[codePoint]);
        }
    } else {
        await UnicodeData.parse(cccsTables, UnicodeData.properties.ccc);
    }
    cccsTables?.process?.();
    await createTableFile(cccsTables);
    console.log("File processing completed.");
};

class CCCTables {
    tables = new TablePairs();
    name = "ccc"; // Canonical Combining Class
    description = "Canonical Combining Class";
    ignoreErrors = false;

    // these numbers are educated guesses from other projects, they're not that important!
    indices = {
        max: 4353 * 100,
        sizeof: uint32,
        // splitInto: 1, // split the table this many tables
        description: `CCC: Canonical Combining Class
These are the indices that are used to find which values from "ccc_values" table correspond to a Unicode Code Point.`,
    };
    values = {
        max: 65535 * 10,
        sizeof: embedQuickCheckTables ? uint16 : uint8,
        description: `CCC: Canonical Combining Class ${embedQuickCheckTables ? 'and Quick Check' : ''}
These values are calculated and individually represent actual CCC values, but they have no
valid order by themselves, and they only make sense if they're being used in conjunction with
the "ccc_indices" table.
        `,
        map: !embedQuickCheckTables ? undefined : (vals, info) => {
            let res = Array.isArray(vals) ? vals : [];
            for (let i = 0; i !== vals.length; ++i) {
                const code = vals[i];
                const qcCode = code >> 8;
                const ccc = code & 0xFF;
                if (code === 0) {
                    res[i] = vals[i];
                    continue;
                }
                if (qcCode === 0) {
                    res[i] = ccc;
                    continue;
                }
                res[i] = `${ccc} | 0x${qcCode.toString(16).toUpperCase()}U << 8U`;
            }
            return res;
        }
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
            genIndexAddenda: () => genSimpleIndexAddenda("index", uint7),
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

        // calculating the last item that it's value is zero
        if (value !== 0) {
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
${renderedTables}
        `;
    }
}

const createTableFile = async (table) => {
    const totalBits = Number(table.totalTablesSizeInBits());
    const readmeData = await getReadme();
    const competition = 21;
    const saved = (competition - totalBits / 8 / 1024).toFixed(2);
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
 *   Some other implementations' total table size (excluding the Quick Check tables) was ${competition} KiB;
 *   So I have ${saved > 0 ? `saved` : `wasted`} ${Math.abs(saved).toFixed(2)} KiB.
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
 *   Known Properties' Values are taken from:
 *       https://www.unicode.org/Public/UCD/latest/ucd/PropertyValueAliases.txt
 * 
 * ${!embedQuickCheckTables ? '' : `Quick Check values are embedded in CCC values in these tables, which grows the tables.`}
 * ${!embedQuickCheckTables || !excludeDecompositionOnly ? '' : `NFD Quick Check values are excluded in these tables.`}
 * ${!embedQuickCheckTables || !excludeKompatibility ? '' : `Kompatibility values like NFKC and NFKD Quick Check values are excluded in these tables.`}
 */

#ifndef WEBPP_UNICODE_CCC_TABLES_HPP
#define WEBPP_UNICODE_CCC_TABLES_HPP

#include <array>
#include <cstdint>

namespace webpp::unicode {

    struct ccc_props {
        // Giving aliases to known values:
        ${runCmd("curl --silent https://www.unicode.org/Public/UCD/latest/ucd/PropertyValueAliases.txt | grep -oE '[^#]+' | awk 'BEGIN{FS=\";\"} {if ($1 == \"ccc\") print \"static constexpr std::uint8_t\" $4 \" =\" $2 \"U; // \" $3}'")}
    };

}

namespace webpp::unicode::details {

    /**
     * In "ccc_index" table, any code point bigger than this number will have "zero" as its CCC value;
     * so it's designed this way to reduce the table size.
     */
    static constexpr auto trailing_zero_cccs = 0x${table.lastZero.toString(16).toUpperCase()}UL;

${table.render()}

} // namespace webpp::unicode::details

#endif // WEBPP_UNICODE_CCC_TABLES_HPP
    `;

    await writePieces(cccOutFile, [content]);
    await runClangFormat(cccOutFile);
};

start();
