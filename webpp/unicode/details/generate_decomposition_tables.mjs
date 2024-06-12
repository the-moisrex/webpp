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
    writePieces, runClangFormat, uint6, uint7, utf32To8All,
} from "./utils.mjs";
import * as path from "node:path";
import {getReadme} from "./readme.mjs";
import {TablePairs} from "./table.mjs";
import {
    Addenda, Addendum, genMaskAddendum, genPositionAddendum
} from "./modifiers.mjs";

const outFile = `decomposition_tables.hpp`;


// From https://www.unicode.org/versions/Unicode15.1.0/ch03.pdf#G56669

/// SBase in the standard:
const hangul_syllable_base = 0xAC00;
/// LBase in the standard:
const hangul_leading_base = 0x1100;
/// VBase in the standard:
const hangul_vowel_base = 0x1161;
/// TBase in the standard:
const hangul_trailing_base = 0x11A7;
/// LCount in the standard:
const hangul_leading_count = 19;
/// VCount in the standard:
const hangul_vowel_count = 21;
/// TCount in the standard:
const hangul_trailing_count = 28;
/// Total count of Hangul blocks and syllables
/// NCount in the standard:
const hangul_block_count = hangul_vowel_count * hangul_trailing_count;
/// SCount in the standard:
const hangul_syllable_count = hangul_leading_count * hangul_block_count;

const isHangul = (codePoint) => {
    return codePoint >= hangul_syllable_base && codePoint < hangul_syllable_base + hangul_syllable_count;
}


const start = async () => {
    await readme.download();

    // database file
    const decompTables = new DecompTable();
    await UnicodeData.parse(decompTables, UnicodeData.properties.decompositionType);
    decompTables?.process?.();
    await createTableFile([decompTables]);
    console.log('File processing completed.');
};

class DecompTable {
    tables = new TablePairs();

    // these numbers are educated guesses from other projects, they're not that important!
    lastMapped = 0;
    maxMappedLength = 0;
    hangulIgnored = 0;

    constructor() {
        this.tables.init({
            disableComments: false,
            name: "decomp",
            description: "Decomposition Code Points",
            ignoreErrors: false,

            // first table
            indices: {
                max: 4353 * 10,
                sizeof: uint32,
                description: `Decomposition Index`,
            },

            // second table that holds the utf-8 encoded values
            values: {
                max: 65535,
                sizeof: uint8,
                description: `UTF-8 Encoded Decomposition Code Points`,
            },
            validateResults: true,
            genIndexAddenda: () => this.genAddenda(),

            // this gets run just before we add the modifier to the indices table
            modify: ({modifier, inserts}) => {

                // flattening the inserts to include only the utf-8 bytes:
                inserts = inserts.reduce((acc, cur) => [...acc, ...cur.mappedTo], []);

                // add length to the modifier:
                modifier.set({length: inserts.length});

                return {modifier, inserts};
            },
        });
    }

    genAddenda = () => {
        const name = "index";
        const addendaPack = [
            genPositionAddendum(),
            genMaskAddendum(uint8),
            new Addendum({
                name: "length",
                description: `Length of the UTF-8 Encoded Decomposition Code Points`,
                affectsChunkSize: true,
                sizeof: uint8,
            }),
        ];
        const addenda = new Addenda(name, addendaPack, function (table, modifier, range, pos) {
            const {pos: maskedPos} = this.mask.modify(modifier, {pos});
            const newPos = range + maskedPos;
            if (newPos >= table.length) {
                throw new RangeError(`Invalid position calculated; range: ${range}, pos: ${pos}, faulty pos: ${newPos}, table length: ${table.length}, modifier: ${JSON.stringify(modifier)}`);
            }
            const {mapped, mappedTo} = table.at(newPos);
            return {mapped, mappedTo, length: mappedTo.length};
        });
        addenda.modifierFunctions = {
            applyMask: function (pos) {
                return this.addenda.mask.modify(this, {pos});
            }
        };
        addenda.renderFunctions = [
            // todo
        ];
        return addenda;
    };

    /// proxy the function
    process() {
        this.tables.process();
        const lastMappedBucket = this.lastMapped >>> this.tables.chunkShift;

        console.log("Trim indices table at: ", lastMappedBucket);
        this.tables.indices.trimAt(lastMappedBucket);

        console.log("Hangul code points ignored: ", this.hangulIgnored);
        console.log("Max Mapped Length: ", this.maxMappedLength);

        // let flattened = [];
        // for (const index of this.tables.indices) {
        //     flattened.push(index)
        // }
    }

    add(codePoint, value) {

        // ignore Hangul code points, they're handled algorithmically
        if (isHangul(codePoint)) {
            ++this.hangulIgnored;
            return;
        }

        // calculating the last item that it's value is zero
        const {mapped, mappedTo} = value;
        value.mappedTo = utf32To8All(mappedTo); // convert the code points to utf-8
        if (mapped) {
            this.lastMapped = codePoint + 1;
        }
        if (mappedTo.length > this.maxMappedLength) {
            this.maxMappedLength = mappedTo.length;
        }
        this.tables.add(codePoint, value);
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
    }

    processRendered(renderedTables) {
        return `
    /**
     * In "decomposition_index" table, any code point bigger than this number will be "non-mapped" (it's mapped to the input code point by standard);
     * so it's designed this way to reduce the table size.
     */
    static constexpr auto trailing_mapped_deomps = 0x${this.lastMapped.toString(16).toUpperCase()}UL;
    
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
    await writePieces(outFile, pieces);
    await runClangFormat(outFile);
};

start();
