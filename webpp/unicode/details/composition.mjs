import {getCanonicalDecompositions} from "./UnicodeData.mjs";
import {bitFloor, utf32To8} from "./utils.mjs";

export class CanonicalComposition {

    #canonicalCompositions = {};
    #mergedMagicalValues = [];
    #shiftedMagicalValues = [];
    lastMapped = 0;
    chunkShift = 0;
    #magicalTable = {};

    #validateMagicMerge(magicCode, cp1, cp2) {
        if (this.#mergedMagicalValues.includes(magicCode)) {
            throw new Error("Magical Merging Formula does not produce unique values anymore: " + magicCode);
        }

        const lastMappedBucket = this.lastMapped >>> this.chunkShift;
        const mask = this.magicMask;
        const shifted = magicCode & mask;
        if (shifted > lastMappedBucket) {
            debugger;
            throw new Error(`Out of range magic code generated: ${magicCode} > ${this.lastMapped} (code points: ${cp1}, ${cp2}) (shifted: ${shifted}) (last bucket: ${lastMappedBucket}) (chunk shift: ${this.chunkShift}) (mask: ${mask})`);
        }
        // if (this.#shiftedMagicalValues.includes(shifted)) {
        //     debugger;
        //     throw new Error(`Invalid shift algorithm: (magic code: ${magicCode}) (last mapped: ${this.lastMapped}) (code points: ${cp1}, ${cp2}) (shifted: ${shifted}) (last bucket: ${lastMappedBucket}) (chunk shift: ${this.chunkShift}) (mask: ${mask})`);
        // }

        this.#mergedMagicalValues.push(magicCode);
        // this.#shiftedMagicalValues.push(shifted);
    }

    get magicMask() {
        return bitFloor(this.lastMapped) - 1;
    }

    /// Magical Formula
    /// Do NOT try to make sense of this algorithm, it's random with no meaning.
    /// Its purpose is only to generate a set of merged values that the merged values
    ///   are unique in the current Unicode composition database.
    ///
    /// If you change this, you need to change it inside "../normalization.hpp" file as well.
    magicMerge(codePoint1, codePoint2) {
        const merged = (codePoint1 + (codePoint1 >>> 2)) * codePoint2;
        this.#validateMagicMerge(merged, codePoint1, codePoint2);
        return merged;
    }

    codePoint(magicCode) {
        return this.#magicalTable[magicCode];
    }

    #calculateMagicTable() {
        for (let codePoint in this.#canonicalCompositions) {
            const [cp1, cp2] = this.#canonicalCompositions[codePoint];
            const magicVal = this.magicMerge(cp1, cp2);
            this.#magicalTable[magicVal] = codePoint;
        }
    }

    async load() {
        this.#canonicalCompositions = await getCanonicalDecompositions();
        this.#calculateMagicTable();
    }

    needsModification(magicCode) {
        return magicCode in this.#magicalTable;
    }

    utf8Composed(magicCode) {
        if (!this.needsModification(magicCode)) {
            return [];
        }
        return utf32To8(this.#magicalTable[magicCode]);
    }

    needsModificationRange(codePointStart, codePointEnd) {
        for (let codePoint = codePointStart; codePoint !== codePointEnd; ++codePoint) {
            if (this.needsModification(codePoint)) {
                return true;
            }
        }
        return false;
    }

    maxLengthOfRange(table, codePoint, curMaxLength) {
        if (!this.needsModification(codePoint)) {
            return curMaxLength;
        }

        const utf8 = this.utf8Composed(codePoint);

        // calculate the perfect maxLength for the current codePoint:
        const {mappedTo} = table.at(codePoint);
        const requiredLength = mappedTo.length + utf8.length + 1; // +1 for the additional '0' that used as a separator
        if (requiredLength > curMaxLength) {
            return requiredLength;
        }
        return curMaxLength;
    }

    /// check if the specified range will require a new max_length to fit in the new value
    // modifyRange(table, codePointStart, length, curMaxLength) {
    //     assert.ok(Number.isSafeInteger(codePointStart) && !isNaN(codePointStart));
    //
    //     const codePointEnd = codePointStart + length;
    //     let needsModification = false;
    //     let inserts = [];
    //     let maxLength = curMaxLength;
    //     for (let codePoint = codePointStart; codePoint !== codePointEnd; ++codePoint) {
    //         if (!this.needsModification(codePoint)) {
    //             inserts.push([]); // no insert for this code point
    //             continue;
    //         }
    //
    //         needsModification = true;
    //         const utf8 = this.utf8Composed(codePoint);
    //         inserts.push(utf8);
    //
    //
    //         // calculate the perfect maxLength for the current codePoint:
    //         const {mappedTo} = table.at(codePoint);
    //         const requiredLength = mappedTo.length + utf8.length + 1; // +1 for the additional '0' that used as a separator
    //         if (requiredLength > maxLength) {
    //             maxLength = requiredLength;
    //             console.log("  Maximum length expansion is required: ", curMaxLength, '-->', requiredLength, `${codePointStart.toString(16)}-${codePointEnd.toString(16)}`);
    //         }
    //
    //     }
    //     return {
    //         requiresExpanding: maxLength !== curMaxLength,
    //         maxLength,
    //         needsModification,
    //         inserts,
    //     };
    // }

    render() {
        return `
        /// Magical mask to be used on magic_code to get its values' position in the index table
        static constexpr std::size_t magic_mask = 0x${this.magicMask.toString(16).toUpperCase()}U;
        
        /// This is a magical formula that absolutely does not make sense, but it works because math is magical.
        /// This will merge the 2 code points into one single value that then can be used to get the position of the
        /// values in the values table.
        template <typename CharT = char32_t>
        [[nodiscard]] static constexpr std::size_t magic_merge(CharT const cp1, CharT const cp2) noexcept {
            return (cp1 + (cp1 >> 2)) * cp2;
        }
        
        /// This function will get you the Canonical Composition's values' position
        [[nodiscard]] static constexpr std::size_t composition_position(std::size_t const magic_code) noexcept {
            return magic_code & magic_mask;
        }
        
        /// This function will get you the Canonical Composition's values' position
        template <typename CharT = char32_t>
        [[nodiscard]] static constexpr std::size_t composition_position(CharT const cp1, CharT const cp2) noexcept {
            return composition_position(magic_merge(cp1, cp2));
        }
        `;
    }

}


if (process.argv[1] === new URL(import.meta.url).pathname) {
    const comp = new CanonicalComposition();
    await comp.load();
    console.log('61: ', comp.needsModification(61));
    console.log('fb32: ', comp.needsModification(0xfb32));
}
