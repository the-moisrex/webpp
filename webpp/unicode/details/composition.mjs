import {extractedCanonicalDecompositions, getCanonicalDecompositions} from "./UnicodeData.mjs";
import {
    findSmallestComplement,
    findSmallestMask,
    findTopLongestZeroRanges,
    interleaveBits,
    utf32To8
} from "./utils.mjs";

export class CanonicalComposition {

    #canonicalCompositions = {};
    #mergedMagicalValues = [];
    // #shiftedMagicalValues = [];
    lastMapped = 0;
    chunkShift = 0;
    #magicalTable = {};
    #topEmptyRanges = [];

    #codePoint1Mask = 0;
    #codePoint2Mask = 0;

    #codePoint1Compl = 0;
    #codePoint2Compl = 0;

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

        const shifted = magicCode >>> this.chunkShift;
        // console.log(shifted, magicCode, cp1, cp2);
        if (shifted >= this.lastMappedBucket) {
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
        // return (bitFloor(this.lastMapped) << this.chunkShift) - 1;

        // the 65536 is `unsigned short` length so the interleave_bits algorithm would work correctly.
        return (65536 - 1) >>> 1;
    }

    get lastMappedBucket() {
        return this.lastMapped >>> this.chunkShift;
    }

    get lastMagic() {
        // return ((this.lastMapped << this.chunkShift) * this.lastMapped);
        return undefined;
    }

    get magicBucket() {
        return this.lastMappedBucket * 2000;
        // const lastCP = (bitCeil(this.lastMapped) - 1) & this.magicMask;
        // console.log(this.lastMappedBucket * 2000, this.lastMapped, lastCP, interleaveBits(lastCP, lastCP) >>> this.chunkShift);
        // return interleaveBits(lastCP, lastCP) >>> this.chunkShift;
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


        // const cp1 = codePoint1 & this.magicMask;
        // const cp2 = (codePoint2 << 2) & this.magicMask;
        // const lastMagic = this.lastMapped & this.magicMask;
        // const maxInterleaves = interleaveBits(lastMagic, lastMagic);
        // const magic = interleaveBits(cp1, cp2);
        // const k = 0.00005; //  scaling factor that determines how much emphasis to put around z (typically between 0 and 1).
        // const z = lastMagic / 2; // The central value around which you want to emphasize scaling.
        // const p = 2; // A power factor that controls the curvature of the scaling (higher values will create a sharper emphasis around z).
        // const x = Math.floor(k * Math.pow(magic - z, p));
        // const scaled = Math.floor(x * lastMagic / maxInterleaves);
        // // console.log(cp1, cp2, x)
        // // console.log(this.magicBucket)
        // const merged = scaled;
        // console.log(merged, codePoint1, codePoint2, cp1, cp2, 'scaled:', scaled, x, lastMagic, maxInterleaves, this.magicBucket);


        const cp1 = (codePoint1 & this.#codePoint1Mask) - this.#codePoint1Compl;
        const cp2 = (codePoint2 & this.#codePoint2Mask) - this.#codePoint2Compl;
        // const x = (cp1 + (cp1 >>> 2)) * cp2;
        // const x = (cp1 * (cp1 >>> 2)) * cp2;
        // const x = (cp1 + (cp2 >>> 2)) * cp2;
        // const x = cp1 * cp2;
        const x = interleaveBits(cp1, cp2);
        const merged = x % this.lastMappedBucket;
        console.log(codePoint1, codePoint2, cp1, cp2, x, merged, `(${this.#codePoint1Mask}, ${this.#codePoint2Mask})`);

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

    #calculateCodePointMasks(firstArray, secondArray) {
        this.#codePoint1Mask = findSmallestMask(firstArray);
        this.#codePoint2Mask = findSmallestMask(secondArray);
        this.#codePoint1Compl = findSmallestComplement(firstArray.map(item => item & this.#codePoint1Mask));
        this.#codePoint2Compl = findSmallestComplement(secondArray.map(item => item & this.#codePoint2Mask));
    }

    async load() {
        this.#canonicalCompositions = await getCanonicalDecompositions();
        const maps = await extractedCanonicalDecompositions();
        this.#calculateCodePointMasks(maps.mappedToFirst, maps.mappedToSecond);
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
        
        static constexpr std::size_t magic_bucket = 0x${this.magicBucket.toString(16).toUpperCase()}U;
        
        /// This is a magical formula that absolutely does not make sense, but it works because math is magical.
        /// This will merge the 2 code points into one single value that then can be used to get the position of the
        /// values in the values table.
        template <typename CharT = char32_t>
        [[nodiscard]] static constexpr std::size_t magic_merge(CharT cp1, CharT cp2) noexcept {
            cp1 &= magic_mask;
            cp2 &= magic_mask;
            return webpp::interleave_bits(cp1, cp2) % magic_bucket;
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
