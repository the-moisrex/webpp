import {getCanonicalDecompositions} from "./UnicodeData.mjs";
import {bitFloor, findTopLongestZeroRanges, interleaveBits, utf32To8} from "./utils.mjs";

export class CanonicalComposition {

    #canonicalCompositions = {};
    #mergedMagicalValues = [];
    // #shiftedMagicalValues = [];
    lastMapped = 0;
    chunkShift = 0;
    #magicalTable = {};
    #topEmptyRanges = [];

    #validateMagicMerge(magicCode, cp1, cp2) {
        if (isNaN(magicCode) || magicCode === undefined || magicCode === null || !isFinite(magicCode) || !Number.isSafeInteger(magicCode)) {
            throw new Error(`magic code is not a safe integer: ${magicCode} (${cp1}, ${cp2})`);
        }
        if (magicCode < 0) {
            throw new Error(`magic code is negative: ${magicCode} (${cp1}, ${cp2})`);
        }
        if (this.#mergedMagicalValues.includes(magicCode)) {
            throw new Error(`Magical Merging Formula does not produce unique values anymore: (${cp1}, ${cp2}) (magic code: ${magicCode}) (magic mask: ${this.magicMask}) (magic count: ${this.#mergedMagicalValues.length})\n${this.#mergedMagicalValues.join(', ')}`);
        }

        const shifted = magicCode >>> (this.chunkShift * 2);
        if (shifted > this.lastMappedBucket) {
            debugger;
            throw new Error(`Out of range magic code generated: ${magicCode} > ${this.lastMapped} (code points: ${cp1}, ${cp2}) (shifted: ${shifted}) (last bucket: ${this.lastMappedBucket}) (chunk shift: ${this.chunkShift}) (last magic: ${this.lastMagic})`);
        }
        // if (this.#shiftedMagicalValues.includes(shifted)) {
        //     debugger;
        //     throw new Error(`Invalid shift algorithm: (magic code: ${magicCode}) (last mapped: ${this.lastMapped}) (code points: ${cp1}, ${cp2}) (shifted: ${shifted}) (last bucket: ${lastMappedBucket}) (chunk shift: ${this.chunkShift}) (mask: ${mask})`);
        // }

        this.#mergedMagicalValues.push(magicCode);
        // this.#shiftedMagicalValues.push(shifted);
    }

    get magicMask() {
        return (bitFloor(this.lastMapped) << this.chunkShift) - 1;
    }

    get lastMappedBucket() {
        return this.lastMapped >>> this.chunkShift;
    }

    get lastMagic() {
        return ((this.lastMapped << this.chunkShift) * this.lastMapped);
    }


    /// Magical Formula
    /// Do NOT try to make sense of this algorithm, it's random with no meaning.
    /// Its purpose is only to generate a set of merged values that the merged values
    ///   are unique in the current Unicode composition database.
    magicMerge(codePoint1, codePoint2) {
        // const merged = ((codePoint1 - (codePoint2 >>> 3)) * codePoint2) & this.magicMask;
        // const merged = Math.floor(((codePoint1 << this.chunkShift) * codePoint2) * this.magicMask / this.lastMagic);
        // const merged = ((codePoint1 * codePoint1) - (codePoint2 * codePoint2)) & this.magicMask;

        // Cantor Pairing Function: This function uniquely combines two non-negative integers into a single non-negative integer.
        // const merged = (codePoint1 + codePoint2) * (codePoint1 + codePoint2 + 1) / 2 + codePoint2;

        // Scaling formula:
        // const x = (codePoint1 * (codePoint1 >>> 2)) * codePoint2;
        // const maxMagic = (this.lastMapped * (this.lastMapped >>> 2)) * this.lastMapped;
        // const merged = Math.floor(this.lastMappedBucket * x / maxMagic);

        // Scaling function that scales more on specific point:
        // const x = (codePoint1 + (codePoint1 >>> 2)) * codePoint2; // The original number you want to scale.
        // const y = this.lastMappedBucket; // The target upper limit for scaling.
        // const maxMagic = (this.lastMapped * (this.lastMapped >>> 2)) * this.lastMapped;
        // const k = 1; //  scaling factor that determines how much emphasis to put around z (typically between 0 and 1).
        // const z = maxMagic / 2; // The central value around which you want to emphasize scaling.
        // const p = 2; // A power factor that controls the curvature of the scaling (higher values will create a sharper emphasis around z).
        // const merged = Math.floor(y * (1 - k * Math.pow(Math.abs((x - z) / y), p)));

        // const x = (codePoint1 + (codePoint1 >>> 2)) * codePoint2; // The original number you want to scale.
        // const maxMagic = (this.lastMapped * (this.lastMapped >>> 2)) * this.lastMapped;
        // const y = this.lastMappedBucket; // The target upper limit for scaling.
        // const z = 2/3 * maxMagic; // The central value around which you want to emphasize scaling.
        // const k = x * y / maxMagic;
        // const distance = Math.abs(x - z);
        // console.log(k)
        // const merged = Math.floor(k * distance);


        // const x = (codePoint1 + (codePoint1 >>> 2))  * codePoint2; // The original number you want to scale.
        // const merged = Math.floor(x * 0.09);

        // const x = (codePoint1 + (codePoint1 >>> 2)) ^ (codePoint2 + (codePoint2 << 2)); // The original number you want to scale.
        // const merged = x % this.lastMappedBucket;


        const cp1 = codePoint1 & (65536 - 1 >> 1);
        const cp2 = codePoint2 & (65536 - 1 >> 1);
        const x = interleaveBits(cp1, cp2);
        // console.log(cp1, cp2, x)
        const merged = x % (this.lastMappedBucket * 2000);

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

    #calculateTopEmptyRanges() {
        this.#topEmptyRanges = findTopLongestZeroRanges(this.#magicalTable);
    }

    async load() {
        this.#canonicalCompositions = await getCanonicalDecompositions();
        this.#calculateMagicTable();
        this.#calculateTopEmptyRanges();
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
            return ((cp1 + (cp1 >> 2)) * cp2) & magic_mask;
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
