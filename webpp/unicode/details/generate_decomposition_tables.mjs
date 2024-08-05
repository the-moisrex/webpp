/***
 * This file downloads UnicodeData.txt, and generates a C++ header file.
 *
 * More information about the syntax:
 *   https://www.unicode.org/reports/tr44/#Character_Decomposition_Mappings
 */

import * as readme from "./readme.mjs";
import {getReadme} from "./readme.mjs";
import * as UnicodeData from "./UnicodeData.mjs";
import {char8_6, char8_8, runClangFormat, uint32, utf32To8All, writePieces} from "./utils.mjs";
import * as path from "node:path";
import {TablePairs} from "./table.mjs";
import {Addenda, genMaxLengthAddendum, genPositionAddendum, staticFields} from "./modifiers.mjs";
import {isHangul} from "./hangul.mjs";
import {CanonicalComposition} from "./composition.mjs";

const outFile = `decomposition_tables.hpp`;


const start = async () => {
    await readme.download();

    // database file
    const decompTables = new DecompTable();
    await UnicodeData.parse(decompTables, UnicodeData.properties.decompositionType);
    await decompTables.load();
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
    // flattedDataView = [];
    maxMaxLength = 0;
    max32MaxLength = 0;
    max16MaxLength = 0;
    #canonicalCompositions = null;

    findMaxLengths({codePointStart, length, data}) {
        let maxLen = 0;
        for (let index = codePointStart; index < (codePointStart + length); index++) {
            const datum = data.at(index);
            const {mapped, mappedTo} = datum;
            if (!mapped && !this.#canonicalCompositions.needsModification(index)) {
                continue;
            }
            if (mappedTo.length > maxLen) {
                maxLen = mappedTo.length;
            }
            maxLen = this.#canonicalCompositions.maxLengthOfRange(data, index, maxLen);
        }

        // set the max_length addendum value
        return {max_length: maxLen};
    }

    constructor() {
        const self = this;
        this.#canonicalCompositions = new CanonicalComposition();
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
                sizeof: char8_8,
                description: `UTF-8 Encoded Decomposition Code Points`,
            },
            validateResults: false,
            genIndexAddenda: () => this.genAddenda(),


            getModifierAddenda: this.findMaxLengths.bind(this),

            dataView(codePointStart, length) {

                let values = [];
                const maxLength = self.getMaxLength(codePointStart);
                // const {
                //     inserts
                // } = self.#canonicalCompositions.modifyRange(this.data, codePointStart, length, maxLength);
                // assert.equal(inserts.length, length);
                for (let index = codePointStart; index < (codePointStart + length); index++) {
                    const canonicalCompositionCodePoint = self.#canonicalCompositions.utf8Composed(index - codePointStart);
                    const {mapped, mappedTo} = this.data.at(index);

                    let ith = 0;
                    for (; ith !== maxLength; ++ith) {
                        if (mapped && ith < mappedTo.length) {
                            values.push(mappedTo[ith]);
                        } else {
                            values.push(0);
                        }
                    }

                    if (ith !== maxLength) {
                        debugger;
                        throw new Error("Max length is violated.");
                    }

                    // adding the composition code point (it's a single code point, but in UTF-8):
                    --ith;
                    for (let index = canonicalCompositionCodePoint.length - 1; ith > -1 && index > -1; --index, --ith) {
                        values[ith] = canonicalCompositionCodePoint[index];
                        if (ith < mappedTo.length) {
                            debugger;
                            throw new Error(`Invalid max length was calculated: ${ith} < ${mappedTo.length}`);
                        }
                    }
                }
                return values;

                // const endPos = codePointStart + length;
                // // if (this.data.length < endPos) {
                // //     return new Span(this.data, codePointStart, 0);
                // // } else if (this.data.length === endPos) {
                // //     return new Span(this.data, codePointStart, this.data[this.data.length - 1].mappedTo.length);
                // // }
                // const {flatStart} = this.data.at(codePointStart);
                // const {flatStart: flatEnd} = this.data.at(endPos) || {flatStart: self.flattedDataView.length};
                // const flatLength = flatEnd - flatStart;
                // if (!Number.isSafeInteger(flatLength)) {
                //     debugger;
                //     throw new Error(`Unexpected length: ${flatLength}`);
                // }
                // return new Span(self.flattedDataView, flatStart, flatLength);
            },

            // insertsDataView(codePointStart, length) {
            //     let values = [];
            //     const maxLength = self.getMaxLength(codePointStart);
            //     for (let index = codePointStart; index < (codePointStart + length); index++) {
            //         const {mapped, mappedTo} = this.data.at(index);
            //         for (let ith = 0; ith !== maxLength; ith++) {
            //             if (mapped && ith < mappedTo.length) {
            //                 values.push(mappedTo[ith]);
            //             } else {
            //                 values.push(0);
            //             }
            //         }
            //     }
            //     return values;
            // },

            // this gets run just before we add the modifier to the indices table
            modify: ({modifier, inserts}) => {

                // flattening the inserts to include only the utf-8 bytes:
                // inserts = Array.from(inserts).reduce((acc, cur) => [...acc, ...cur.mappedTo], []);

                // add length to the modifier:
                // modifier.set({length: inserts.length});

                // Add empty fields to the values table:
                // let newInserts = [];
                // for (const item of inserts) {
                //     newInserts.push(item);
                //     for (let index = 1; index < modifier.max_length; index++) {
                //         newInserts.push(0);
                //     }
                // }

                return {modifier, inserts};
            },
        });
    }

    async load() {
        this.#canonicalCompositions.lastMapped = this.lastMapped;
        this.#canonicalCompositions.chunkShift = this.tables.chunkShift;
        await this.#canonicalCompositions.load();
    }

    genAddenda = () => {
        const name = "index";
        const addendaPack = [
            genPositionAddendum(char8_6),
            // genMaskAddendum(uint8),

            // this will affect the chunkSize:
            genMaxLengthAddendum(char8_6),
        ];
        const addenda = new Addenda(name, addendaPack, {
            modify: function (table, modifier, range, pos) {
                const {pos: maskedPos} = this.max_length.modify(modifier, {pos});
                const newPos = range + maskedPos;
                if (newPos >= table.length) {
                    return null;
                    // throw new RangeError(`Invalid position calculated; range: ${range}, pos: ${pos}, faulty pos: ${newPos}, table length: ${table.length}, modifier: ${JSON.stringify(modifier)}`);
                }
                // return table.at(newPos);
                // return {mapped, mappedTo, length: mappedTo.length};
                return table.at(newPos);
            }
        });
        addenda.modifierFunctions = {
            // applyMask: function (pos) {
            //     return this.addenda.mask.modify(this, {pos});
            // }
        };
        const self = this;
        addenda.renderFunctions = [
            staticFields,
            function notMappedFunction() {
                const addenda = this.addenda.toSorted((a, b) => a.placement - b.placement);
                return `
        /// Get an invalid mapping (that shows the code point is not being mapped at all)
        /// This means the code point is mapped to itself
        [[nodiscard]] static consteval ${this.name} not_mapped() noexcept {
            // it can be identified by ${this.max_length.name} == 0
            return ${this.name}{${addenda.map(addendum => addendum.name === "max_length" ? "0" : `${addendum.defaultValue}`).join(", ")}};
        }
                `;
            },
            function maxMaxLengthFunction() {
                return `
        /// Maximum value of "max_length" in the whole values table.
        /// It's the amount of mapped UTF-8 "bytes" (not code points).
        /// Hope this can enable some optimizations.
        static constexpr auto max_utf8_mapped_length = ${self.maxMaxLength}UL;
        
        /// Maximum values of UTF-16 code points mapped
        static constexpr auto max_utf16_mapped_length = ${self.max16MaxLength}UL;
        
        /// Maximum values of code points mapped (UTF-32)
        static constexpr auto max_utf32_mapped_length = ${self.max32MaxLength}UL;
                `;
            },
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
            //     function isMapped() {
            //         return `
            // /// See if this code point
            // [[nodiscard]] constexpr bool is_mapped(${self.tables.values.STLTypeString} const value) const noexcept {
            //     return max_length == 0;
            // }
            //         `;
            //     }
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

    getMaxLength(codePoint) {
        const codePointStart = codePoint - (codePoint % this.tables.chunkSize);
        if (codePointStart >= this.tables.data.length) {
            return 1; // default value for the max_length
        }
        return this.findMaxLengths({
            codePointStart,
            length: this.tables.rangeLengthStarting(codePointStart),
            data: this.tables.data,
            // dataView: this.tables.dataView(codePointStart)
        }).max_length;
    }

    add(codePoint, value) {
        let {mapped, mappedTo} = value;

        // ignore Hangul code points, they're handled algorithmically
        if (isHangul(codePoint)) {
            ++this.hangulIgnored;
            if (mapped) {
                throw new Error("Hangul is mapped manually.");
            }
            // return;
        }

        if (value.mappedTo.length > this.max32MaxLength) {
            this.max32MaxLength = value.mappedTo.length;
            this.max16MaxLength = String.fromCodePoint(...value.mappedTo).length; // convert to UTF-16, then get the length
        }

        // calculating the last item that it's value is zero
        mappedTo = value.mappedTo = utf32To8All(mappedTo); // convert the code points to utf-8
        if (mapped) {
            // find the end of the batch, not just the last item
            this.lastMapped = (((codePoint + 1) >>> this.tables.chunkShift) + 1) << this.tables.chunkShift;
        }
        if (mappedTo.length > this.maxMappedLength) {
            this.maxMappedLength = mappedTo.length;
        }


        // // expand the "data" into its "values" table equivalent
        // value.flatStart = this.flattedDataView.length;
        // // value.flatLength = mappedTo.length;
        // if (mapped) {
        //
        if (mappedTo.length > this.maxMaxLength) {
            this.maxMaxLength = mappedTo.length;
        }
        //
        //     // these don't get to be in the "values" table, so they should not be in this table either
        //     for (const curCodePoint of mappedTo) {
        //         // curCodePoint is already UTF-8 encoded, no need for re-encoding
        //         this.flattedDataView.push(curCodePoint);
        //         --maxLength;
        //     }
        //     for (; maxLength > 0; --maxLength) {
        //         this.flattedDataView.push(0);
        //     }
        //
        // }

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
 *   Some other implementations' total table size was 73.4 KiB;
 *   So I saved ${Math.ceil(73.4 - totalBits / 8 / 1024)} KiB and a better a locality.
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
#include <string_view>
#include "../../utils/bits.hpp"

namespace webpp::unicode::details {

`;

    const endContent = `
 
    template <typename CharT = char8_t, typename CPType>
        requires (sizeof(CharT) == sizeof(char8_t))
    [[nodiscard]] static constexpr CharT const* decomp_ptr(decomp_index const code, CPType const code_point) noexcept {
        if constexpr (stl::same_as<CharT, char8_t>) {
            return decomp_values.data() + code.get_position(code_point);
        } else {
            // Legally we can't cast a "char const*" to "char8_t const*", 
            // but we can cast a "char8_t const*" to "char const*"; this is a very weird C++ behavior, that's why
            // we chose u8-based strings in the values table above instead of traditional values.
            // NOLINTNEXTLINE(*-pro-type-reinterpret-cast)
            return reinterpret_cast<CharT const*>(decomp_values.data()) + code.get_position(code_point);
        }
    }
    
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
