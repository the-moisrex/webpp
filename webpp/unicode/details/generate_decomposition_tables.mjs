/***
 * This file downloads UnicodeData.txt, and generates a C++ header file.
 *
 * More information about the syntax:
 *   https://www.unicode.org/reports/tr44/#Character_Decomposition_Mappings
 */

import * as readme from "./readme.mjs";
import {getReadme} from "./readme.mjs";
import * as UnicodeData from "./UnicodeData.mjs";
import {
    char8_6,
    char8_8,
    runClangFormat,
    uint32, uint4,
    utf32To8All,
    writePieces
} from "./utils.mjs";
import * as path from "node:path";
import {TablePairs} from "./table.mjs";
import {
    Addenda,
    genCompactMaskAddendum,
    genMaxLengthAddendum,
    genPositionAddendum,
    staticFields
} from "./modifiers.mjs";
import {isHangul} from "./hangul.mjs";
import {CanonicalComposition} from "./composition.mjs";

const outFile = `decomposition_tables.hpp`;
const embedCanonical = false; // a chance to disable hiding Canonical Compositions in between Decompositions
const enableMaksField = true;


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

    #cacheMaxLen = {};
    #dataViewCache = {};
    findMaxLengths({codePointStart, length, data}) {
        const key = `${codePointStart}-${length}`;
        if (key in this.#cacheMaxLen) {
            return {max_length: BigInt(this.#cacheMaxLen[key])};
        }
        let maxLen = 0n;
        const end = codePointStart + BigInt(length);
        for (let codePoint = codePointStart; codePoint < end; ++codePoint) {
            const {mapped, mappedTo} = data.at(Number(codePoint));
            if (!mapped && !embedCanonical) {
                continue;
            }
            if (!mapped && embedCanonical && !this.#canonicalCompositions.needsModification(codePoint)) {
                continue;
            }
            const len = BigInt(mappedTo.length);
            if (len > maxLen) {
                maxLen = len;
            }
            if (embedCanonical) {
                maxLen = this.#canonicalCompositions.maxLengthOfRange(data, codePoint, maxLen);
            }
        }

        // set the max_length addendum value
        this.#cacheMaxLen[key] = maxLen;
        return {max_length: maxLen};
    }

    constructor() {
        const self = this;
        if (embedCanonical) {
            this.#canonicalCompositions = new CanonicalComposition();
        }
        this.tables.init({
            disableComments: false,
            name: "decomp",
            description: "Decomposition Code Points",
            ignoreErrors: false,

            // first table
            indices: {
                max: 4353 * 100,
                sizeof: uint32,
                description: `Decomposition Index`,
            },

            // second table that holds the utf-8 encoded values
            values: {
                max: 65535 * 100, // 46452
                sizeof: char8_8,
                description: `UTF-8 Encoded Decomposition Code Points`,
            },
            validateResults: false,
            genIndexAddenda: () => this.genAddenda(),


            getModifierAddenda: this.findMaxLengths.bind(this),

            dataView(codePointStart, length) {
                const key = `${codePointStart}-${length}`;
                if (key in self.#dataViewCache) {
                    return self.#dataViewCache[key];
                }

                const maxLength = self.getMaxLength(codePointStart);
                let values = new Uint8Array(Number(maxLength * length));
                // const {
                //     inserts
                // } = self.#canonicalCompositions.modifyRange(this.data, codePointStart, length, maxLength);
                // assert.equal(inserts.length, length);
                const end = codePointStart + length;
                for (let codePoint = codePointStart; codePoint < end; ++codePoint) {
                    const vidx = codePoint - codePointStart;
                    const start = vidx * maxLength;
                    const {mapped, mappedTo} = this.data.at(Number(codePoint));


                    if (mapped) {
                        for (let ith = 0n; ith !== maxLength; ++ith) {
                            values[start + ith] = mappedTo[ith];
                        }
                    }


                    // adding the composition code point (it's a single code point, but in UTF-8):
                    if (embedCanonical) {
                        const canonicalCompositionCodePoint = self.#canonicalCompositions.utf8Composed(codePoint);
                        for (let index = 0; index !== canonicalCompositionCodePoint.length; ++index) {
                            const idx = start + maxLength - (index + 1);
                            if (values[idx] !== 0) {
                                debugger;
                                throw new Error(`Non-Zero value is being replaced; value: ${values[idx]}; ith: ${idx}; index: ${index}; values: ${values}`);
                            }
                            values[idx] = canonicalCompositionCodePoint[index];
                            if (idx < (start + mappedTo.length)) {
                                debugger;
                                throw new Error(`Invalid max length was calculated: ${idx} < ${mappedTo.length}`);
                            }
                        }
                        const idx = start + maxLength - (canonicalCompositionCodePoint.length + 1);
                        if (canonicalCompositionCodePoint.length > 0 && values[idx] !== 0) {
                            debugger;
                            throw new Error(`Invalid max length: ${idx} < ${mappedTo.length}; value: ${values[idx]}; max length: ${maxLength}; CC: ${canonicalCompositionCodePoint}; values: ${values}`);
                        }
                    }
                }

                self.#dataViewCache[key] = values;
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
        if (embedCanonical) {
            this.#canonicalCompositions.lastMapped = this.lastMapped;
            this.#canonicalCompositions.chunkShift = this.tables.chunkShift;
            this.#canonicalCompositions.chunkSize = this.tables.chunkSize;
            this.#canonicalCompositions.chunkMask = this.tables.chunkMask;
            await this.#canonicalCompositions.load();
        }
    }

    genAddenda = () => {
        const name = "index";
        const addendaPack = [
            genPositionAddendum(char8_6),
            // enableMaksField ? genMaskAddendum(uint10) : undefined,
            enableMaksField ? genCompactMaskAddendum(uint4) : undefined,

            // this will affect the chunkSize:
            genMaxLengthAddendum(char8_6),
        ].filter(item => item !== undefined);
        const addenda = new Addenda(name, addendaPack, {
            modify: function (table, modifier, range, pos) {
                let maskedPos = 0;
                if (enableMaksField) {
                    maskedPos = this.compact_mask.modify(modifier, {pos}).pos;
                } else {
                    maskedPos = pos;
                }
                const {pos: lenPos} = this.max_length.modify(modifier, {pos: maskedPos});
                const newPos = range + lenPos;
                if (newPos >= table.length) {
                    return null;
                    // throw new RangeError(`Invalid position calculated; range: ${range}, pos: ${pos}, faulty pos: ${newPos}, table length: ${table.length}, modifier: ${JSON.stringify(modifier)}`);
                }
                // return table.at(newPos);
                // return {mapped, mappedTo, length: mappedTo.length};
                return table.at(Number(newPos));
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
                if (embedCanonical) {
                    return self.#canonicalCompositions.render();
                }
                return "";
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
        const lastMappedBucket = this.lastMapped >> this.tables.chunkShift;

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
            debugger;
            throw new Error("Requesting something from us that shouldn't be requested.");
            // return 1; // default value for the max_length
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
            this.max16MaxLength = String.fromCodePoint(...value.mappedTo.map(val => Number(val))).length; // convert to UTF-16, then get the length
        }

        // calculating the last item that it's value is zero
        mappedTo = value.mappedTo = utf32To8All(mappedTo); // convert the code points to utf-8
        if (mapped) {
            // find the end of the batch, not just the last item
            this.lastMapped = (((codePoint + 1n) >> this.tables.chunkShift) + 1n) << this.tables.chunkShift;
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
    const totalBits = tables.reduce((acc, cur) => acc + Number(cur.totalTablesSizeInBits()), 0);
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
