/***
 * This file downloads UnicodeData.txt, and generates a C++ header file.
 *
 * More information about the syntax:
 *   https://www.unicode.org/reports/tr44/#Character_Decomposition_Mappings
 */

import * as readme from "./readme.mjs";
import {
    getReadme
} from "./readme.mjs";
import * as UnicodeData from "./UnicodeData.mjs";
import {
    chunked, realSizeOf,
    runClangFormat, symbolOf, uint32,
    writePieces
} from "./utils.mjs";
import * as path from "node:path";
import {
    CanonicalComposition
} from "./composition.mjs";
import {TablePairs} from "./table.mjs";
import {
    Addenda,
    genPositionAddendum,
    staticFields
} from "./modifiers.mjs";
import {isHangul} from "./hangul.mjs";

const outFile = `composition_tables.hpp`;
const enable2TableMode = false;
const defaultChunkSize = 6; // uintN_t
const defaultHardWrap = -1n;
const findTheBest = false;
const enableMagicCodeComments = true;
const invalidCodePoint = 0;

const start = async () => {
    await readme.download();

    // database file
    const compTables = new CompTable();
    if (enable2TableMode) {
        await compTables.load();
        compTables.fillTable();
        compTables.process();
    } else {
        await UnicodeData.parse(compTables, UnicodeData.properties.decompositionType);
        await compTables.load();
        compTables.optimize();
    }
    await createTableFile([compTables]);
    console.log('Done.');
};

class CompTable {

    // these numbers are educated guesses from other projects, they're not that important!
    #canonicalCompositions = null;
    #tables = new TablePairs();
    #indexAddenda = null;

    lastMapped = 0n;

    get lastMappedBucket () {
        return this.lastMapped >> this.#tables.chunkShift;
    }

    constructor() {
        this.#canonicalCompositions = new CanonicalComposition();
        this.#indexAddenda = this.genAddenda();
        this.#tables.init({
            name: "composition_table",
            description: "Canonical Compositions",

            // first table
            indices: {
                max: 4353 * 100,
                sizeof: uint32,
                description: `Composition Index`,
            },

            // second table that holds the utf-8 encoded values
            values: {
                max: 65535 * 100, // 46452
                sizeof: uint32,
                description: `Composition Code Points`,
            },

            genIndexAddenda: () => this.#indexAddenda,
        });
    }
    genAddenda = () => {
        const name = "index";
        const addendaPack = [
            genPositionAddendum(uint32),
        ].filter(item => item !== undefined);
        const addenda = new Addenda(name, addendaPack,{
            modify: function (table, modifier, range, pos) {
                const newPos = Number(range + pos);
                if (newPos >= Number(table.length)) {
                    return null;
                }
                return table.at(newPos);
            }
        }, defaultChunkSize);
        const self = this;
        addenda.renderFunctions = [
            staticFields,
            function getPositionFunction() {
                return `
        /**
         * Get the final position of the second table.
         * This does not apply the shift or get the value of the second table for you; this only applies tha mask.
         */
        [[nodiscard]] constexpr ${this.pos.STLTypeString} get_position(auto const request_position) const noexcept {
#if __cplusplus >= 202302L // C++23
            [[assume(max_length <= max_utf8_mapped_length)]];
#endif
            ${this.pos.STLTypeString} const remaining_pos = static_cast<${this.pos.STLTypeString}>(request_position) & chunk_mask;
            return pos + static_cast<${this.pos.STLTypeString}>(remaining_pos * max_length);
        }
        `;
            },
            function magicalRender() {
                return self.#canonicalCompositions.render();
            }
        ];
        return addenda;
    };

    async load() {
        this.#canonicalCompositions.embedCodePointCanonical = !enable2TableMode;
        this.reset(defaultChunkSize);
        await this.#canonicalCompositions.load();
    }

    reset (size, hardWrap = -1n) {
        const {chunkSize, chunkMask, chunkShift} = chunked(size);
        this.#canonicalCompositions.chunkShift = chunkShift;
        this.#canonicalCompositions.chunkSize = chunkSize;
        this.#canonicalCompositions.chunkMask = chunkMask;
        this.#canonicalCompositions.hardWrap = hardWrap;
        this.#canonicalCompositions.calculateMagicalTable();
    }

    fillTable () {
        this.reset(defaultChunkSize);
        if (enable2TableMode) {
            for (let magicCodeStr in this.#canonicalCompositions.magicTable) {
                const magicCode = Number(magicCodeStr);
                if (isNaN(magicCode)) {
                    continue;
                }
                const mappedTo = this.#canonicalCompositions.magicTable[magicCode];
                this.add(magicCode, mappedTo);
            }
        }
    }

    optimize () {
        let history = [];
        let errors = [];
        const tryIt = (chunkSize, hardWrap = -1n) => {
            try {
                this.reset(chunkSize, hardWrap);
                const table = {...this.#canonicalCompositions.magicTable};
                const values = this.#canonicalCompositions.getCodePointTable(invalidCodePoint);
                history.push({
                    chunkSize,
                    hardWrap,
                    magicTable: table,
                    magicValues: values,
                    tableLength: table.length,
                    tableLastMagicCode: table.lastMagicCode,
                    lastMapped: this.#canonicalCompositions.lastMapped,
                    lastMappedBucket: this.#canonicalCompositions.lastMappedBucket,
                    lastShiftedMagicCodePoint: this.#canonicalCompositions.lastShiftedMagicCode,
                });
                console.log(`Valid chunk size: ${chunkSize}-${hardWrap}`);
            } catch (error) {
                console.log(`Invalid chunk size: ${chunkSize}-${hardWrap} ${error.message.substring(0, 100)}`);
                errors.push({
                    chunkSize,
                    hardWrap,
                    error,
                });
            }
        }
        if (findTheBest) {
            for (let chunkSize = 1; chunkSize <= 15; chunkSize++) {
                for (let hardWrap = -1n; hardWrap <= 1000n; hardWrap += 1n) {
                    tryIt(chunkSize, hardWrap);
                }
            }
        } else {
            tryIt(defaultChunkSize, defaultHardWrap);
        }
        const best = history.toSorted((a, b) => Number(a.lastShiftedMagicCodePoint) - Number(b.lastShiftedMagicCodePoint))[0];
        const prettyPrinter = inp_info => {
            const info = {...inp_info};
            delete info.magicTable;
            return info;
        };
        console.log(errors.map(prettyPrinter));
        console.log("--------------------- <Best> --------------------------");
        console.log(prettyPrinter(best));
        console.log("--------------------- </Best> --------------------------");
        if (best === undefined) {
            console.log("Nothing found.");
            return;
        }
        this.reset(best.chunkSize, best.hardWrap);
        this.#renderTable(best);
    }

    get typeString() {
        return this.#type.description;
    }

    #rendered = "";
    #type;
    #size;
    values = [];
    #renderTable(magicalTable) {
        this.#type = symbolOf(magicalTable.chunkSize);
        this.#size = realSizeOf(this.#type);
        this.values = this.#canonicalCompositions.getCodePointTable(invalidCodePoint);
        if (this.values.length !== Number(magicalTable.lastShiftedMagicCodePoint)) {
            debugger;
            throw new Error(`Something went wrong with the table size; (values len: ${this.values.length}) (last shifted: ${magicalTable.lastShiftedMagicCodePoint}) table: ${this.values}`);
        }
        const valuesString = Array.from(this.values).map(magicCode => {
            if (enableMagicCodeComments && magicCode !== invalidCodePoint) {
                const [cp1, cp2] = this.#canonicalCompositions.getCodePointsOf(magicCode, invalidCodePoint);
                if (cp1 === cp2 && cp1 === invalidCodePoint) {
                    return magicCode;
                }
                return `/* ${cp1} + ${cp2} = */ ${magicCode}`;
            }
            return magicCode;
        }).join(", ");
        this.#rendered = `
    static constexpr std::array<std::uint32_t, ${this.values.length}ULL> canonical_composition_magic_table {
        ${valuesString}
    };
        `;
    }

    render () {
        if (!enable2TableMode) {
            return this.processRendered(this.#rendered);
        } else {
            return this.#tables.render();
        }
    }

    processRendered(renderedTables) {
        return `
        ${renderedTables}
        struct composition {
                ${staticFields.call(this.#indexAddenda)}
                ${this.#canonicalCompositions.render()}
        };
        `;
    }

    get totalValuesSize () {
        if (enable2TableMode) {
            return this.#tables.totalTablesSizeInBits() / 8;
        } else {
            return this.values.length;
        }
    }


    /// proxy the function
    process() {
        this.#tables.process();
        console.log("Trim indices table at: ", this.lastMappedBucket);
        this.#tables.indices.trimAt(this.lastMappedBucket);
    }

    add(codePoint, value) {
        let {
            mapped,
        } = value;

        // ignore Hangul code points, they're handled algorithmically
        if (isHangul(codePoint)) {
            if (mapped) {
                throw new Error("Hangul is mapped manually.");
            }
        }

        // calculating the last item that it's value is zero
        if (mapped) {
            // find the end of the batch, not just the last item
            this.lastMapped = (((codePoint + 1n) >> this.#tables.chunkShift) + 1n) << this.#tables.chunkShift;
        }

        this.#tables.add(codePoint, value);
    }

}

const createTableFile = async (tables) => {
    const totalBits = tables.reduce((sum, table) => sum + table.totalValuesSize * 8, 0);
    const readmeData = await getReadme();
    const competitor = (4352 + (67 * 257 * 2 /* 16bit */) + (1883 * 4 /* 32bit */)) / 1024;
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
 *   Some other implementations' total table size was ${competitor.toFixed(1)} KiB;
 *   So I saved ${Math.ceil(competitor - totalBits / 8 / 1024)} KiB and a better a locality.
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
 
#ifndef WEBPP_UNICODE_COMPOSITION_TABLES_HPP
#define WEBPP_UNICODE_COMPOSITION_TABLES_HPP

#include <array>
#include <cstdint>
#include <string_view>
#include "../../utils/bits.hpp"

namespace webpp::unicode::details {

`;

    const endContent = `
    
} // namespace webpp::unicode::details

#endif // WEBPP_UNICODE_COMPOSITION_TABLES_HPP
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
