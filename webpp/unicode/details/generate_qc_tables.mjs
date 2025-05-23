import * as path from "node:path";
import { genSimpleIndexAddenda } from "./modifiers.mjs";
import * as readme from "./readme.mjs";
import { getReadme } from "./readme.mjs";
import { TablePairs } from "./table.mjs";
import {fillEmptyObject, runClangFormat, uint32, uint7, uint8, writePieces} from "./utils.mjs";
import { getQCs, getQuickChecks } from "./DerivedNormalizationProps.mjs";

const outFile = `qc_tables.hpp`;
const excludeDecompositionOnly = true; // Disables NFD
const excludeKompatibility = true; // Disables NFKC, NFKD

const start = async () => {
    await readme.download();

    const qcs = fillEmptyObject(await getQuickChecks(), 0b0) ;
    const qcTables = new QCTables();
    for (const codePointStr in qcs) {
        const codePoint = parseInt(codePointStr);
        if (isNaN(codePoint)) {
            continue;
        }
        const qcCode = getQCs(qcs[codePointStr], excludeKompatibility, excludeDecompositionOnly);
        // console.log(codePoint, qcCode, qcs[codePointStr]);
        qcTables.add(codePoint, qcCode);
    }
    qcTables?.process?.();
    await createTableFile(qcTables);
    console.log("File processing completed.");
};

class QCTables {
    tables = new TablePairs();
    name = "qc"; 
    description = "Quick Check";
    ignoreErrors = false;

    // these numbers are educated guesses from other projects, they're not that important!
    indices = {
        max: 4353 * 100,
        sizeof: uint32,
        // splitInto: 1, // split the table this many tables
        description: `QC: Quick Check Indices`,
    };
    values = {
        max: 65535 * 10,
        sizeof: uint8,
        description: `QC: Quick Check Values`,
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
            this.lastZero = (((codePoint + 1n) >> this.tables.chunkShift) + 1n) << this.tables.chunkShift;
        }
        // console.log(codePoint, value);
        return this.tables.add(codePoint, value);
    }

    render() {
        return this.tables.render();
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
    }
}

const createTableFile = async (table) => {
    const totalBits = Number(table.totalTablesSizeInBits());
    const readmeData = await getReadme();
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
 *   Some other implementations' total table size was 21 KiB;
 *   So I saved ${(21 - totalBits / 8 / 1024).toFixed(2)} KiB.
 *
 * Details about the contents of this file can be found here:
 *   UTS #15: https://www.unicode.org/reports/tr15/
 * 
 *   Detecting Normalization Forms:
 *       https://www.unicode.org/reports/tr15/tr15-54.html#Detecting_Normalization_Forms
 *
 *   UCD README file (used to check the version and creation date):
 *       ${readme.fileUrl}
 * 
 * ${!excludeDecompositionOnly ? '' : `NFD Quick Check values are excluded in these tables.`}
 * ${!excludeKompatibility ? '' : `Kompatibility values like NFKC and NFKD Quick Check values are excluded in these tables.`}
 */

#ifndef WEBPP_UNICODE_QC_TABLES_HPP
#define WEBPP_UNICODE_QC_TABLES_HPP

#include <array>
#include <cstdint>

namespace webpp::unicode::details {

    static constexpr auto trailing_zero_qcs = 0x${table.lastZero.toString(16).toUpperCase()}UL;

${table.render()}

} // namespace webpp::unicode::details

#endif // WEBPP_UNICODE_QC_TABLES_HPP
    `;

    await writePieces(outFile, [content]);
    await runClangFormat(outFile);
};

start();
