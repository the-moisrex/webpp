import {extractedCanonicalDecompositions, getCanonicalDecompositions} from "./UnicodeData.mjs";
import {
    findSmallestComplement,
    findSmallestMask,
    findTopLongestZeroRanges,
    interleaveBits, uint32,
    utf32To8
} from "./utils.mjs";
import {Addendum} from "./modifiers.mjs";

export const genCompositionModifier = () => new Addendum({
    name: "composition_code_point",
    description: "This is the canonical composition code point (merge 2 code points together with a magical algorithm," +
        " look it up in the table, and this value is its canonical composition of them);" +
        " this field has nothing to do with other fields; they are for decomposition," +
        " while this value is for composition.",
    sizeof: uint32,
    affectsChunkSize: true,
    defaultValue: 0n,
    isCategorizable: true,
});

/**
 * The important parts of this class is the merge algorithm and the shift algorithm which needs to be reimplemented in C++ as well.
 */
export class CanonicalComposition {


    embedCodePointCanonical = false;

    #canonicalCompositions = {};
    #mergedMagicalValues = [];
    #shiftedMagicalValues = {};
    lastMapped = 0n;
    chunkShift = 0n;
    chunkSize = 0n;
    chunkMask = 0n;
    #magicalTable = {};
    #topEmptyRanges = [];

    #codePoint1Mask = 0n;
    #codePoint2Mask = 0n;

    #codePoint1Compl = 0n;
    #codePoint2Compl = 0n;

    #magicFinalMask = 0n;
    #magicFinalCompl = 0n;

    // if specified, this value will be used instead of lastMappedBucket
    hardWrap = -1n;

    #lastShiftedMagicCodePoint = 0n;

    #validateMagicMerge(magicCode, cp1, cp2) {
        if (isNaN(Number(magicCode)) || magicCode === undefined || magicCode === null || !isFinite(Number(magicCode)) || !Number.isSafeInteger(Number(magicCode))) {
            throw new Error(`magic code is not a safe integer: ${magicCode} (${cp1}, ${cp2})`);
        }
        if (magicCode < 0n) {
            throw new Error(`magic code is negative: ${magicCode} (${cp1}, ${cp2})`);
        }
        if (this.#mergedMagicalValues.includes(magicCode)) {
            debugger;
            throw new Error(`Magical Merging Formula does not produce unique values anymore: (${cp1}, ${cp2}) (chunk shift: ${this.chunkShift}) (chunk mask: ${this.chunkMask}) (chunk size: ${this.chunkSize}) (magic code: ${magicCode}) (magic count: ${this.#mergedMagicalValues.length})\n${this.#mergedMagicalValues.join(', ')}`);
        }

        const shifted = this.shiftCodePoint(magicCode);
        // console.log(shifted, magicCode, cp1, cp2);
        if (shifted >= this.lastMappedBucket) {
            debugger;
            throw new Error(`Out of range magic code generated: ${magicCode} > ${this.lastMapped} (code points: ${cp1}, ${cp2}) (shifted: ${shifted}) (last bucket: ${this.lastMappedBucket}) (chunk shift: ${this.chunkShift})`);
        }
        if (!(shifted in this.#shiftedMagicalValues)) {
            this.#shiftedMagicalValues[shifted] = [];
        }
        if (this.#shiftedMagicalValues[shifted].length >= this.chunkSize) {
            debugger;
            throw new Error(`Invalid shift algorithm: (magic code: ${magicCode}) (last mapped: ${this.lastMapped}) (code points: ${cp1}, ${cp2}) (shifted: ${shifted}) (last bucket: ${this.lastMappedBucket}) (chunk shift: ${this.chunkShift})`);
        }
        if (this.#shiftedMagicalValues[shifted].includes(this.remaining(magicCode))) {
            debugger;
            throw new Error(`Invalid shift algorithm: duplicate remaining magic code created after shifting.`);
        }

        this.#mergedMagicalValues.push(magicCode);
        this.#shiftedMagicalValues[shifted].push(this.remaining(magicCode));
    }

    /// The shifting algorithm:
    shiftCodePoint(codePoint) {
        // return (codePoint >>> this.chunkShift) % this.lastMappedBucket;

        // this should work, but somehow it creates a bigger magical table:
        if (this.embedCodePointCanonical) {
            return codePoint % this.lastMappedBucket;
        }

        // this should not work, but it does; I don't know why, since this algorithm is magical, I don't even care!
        return codePoint >> this.chunkShift;
    }

    remaining(codePoint) {
        if (this.embedCodePointCanonical) {
            return codePoint;
        }
        return codePoint & this.chunkMask;
    }

    /// generate a table of mappedTo code points where the magic code points say they should be
    getCodePointTable() {
        const values = new Uint32Array(Number(this.lastShiftedMagicCode));
        for (const magicCodePointStr in this.#magicalTable) {
            const mappedTo = Number(this.#magicalTable[magicCodePointStr]);
            const magicInt = parseInt(magicCodePointStr);
            if (!Number.isSafeInteger(magicInt)) {
                continue; // it could be ".length" or something else
            }
            const magicCodePoint = BigInt(magicInt);
            const shiftedMagicCode = Number(this.shiftCodePoint(magicCodePoint));
            if (shiftedMagicCode > values.length) {
                debugger;
                throw new Error(`Invalid table size calculated: ${shiftedMagicCode} > ${values.length}; (magicCodePointStr: ${magicCodePointStr})`);
            }
            values[shiftedMagicCode] = mappedTo;
        }
        return values;
    }

    // get magicMask() {
    //     // return (bitFloor(this.lastMapped) << this.chunkShift) - 1;
    //
    //     // the 65536 is `unsigned short` length so the interleave_bits algorithm would work correctly.
    //     return (65536n - 1n) >> 1n;
    // }

    get lastMappedBucket() {
        if (this.hardWrap > 0n) {
            return this.hardWrap;
        }
        return this.lastMapped >> this.chunkShift;
    }

    get lastMagic() {
        // return ((this.lastMapped << this.chunkShift) * this.lastMapped);
        // return undefined;

        // const cp1 = (this.lastMapped & this.#codePoint1Mask);
        // const cp2 = (this.lastMapped & this.#codePoint2Mask);
        // return (interleaveBits(cp1, cp2) & this.#magicFinalMask) - this.#magicFinalCompl;
        return this.magicMerge(this.lastMapped, this.lastMapped);
    }

    // get magicBucket() {
    //     return this.lastMappedBucket * 2000;
    //     // const lastCP = (bitCeil(this.lastMapped) - 1) & this.magicMask;
    //     // console.log(this.lastMappedBucket * 2000, this.lastMapped, lastCP, interleaveBits(lastCP, lastCP) >>> this.chunkShift);
    //     // return interleaveBits(lastCP, lastCP) >>> this.chunkShift;
    // }

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

        const cp1 = (codePoint1 & this.#codePoint1Mask);
        const cp2 = (codePoint2 & this.#codePoint2Mask);
        // const x = (cp1 + (cp1 >>> 2)) * cp2;
        // const x = (cp1 * (cp1 >>> 2)) * cp2;
        // const x = (cp1 + (cp2 >>> 2)) * cp2;
        // const x = cp1 * cp2;
        const x = (interleaveBits(cp1, cp2) & this.#magicFinalMask) - this.#magicFinalCompl;
        // const merged = x;
        const merged = (((x >> this.chunkShift) % this.lastMappedBucket) << this.chunkShift) | (x & this.chunkMask);
        // console.log(codePoint1, codePoint2, cp1, cp2, x, 'merged:', merged, `(${this.#codePoint1Mask}, ${this.#codePoint2Mask})`);

        this.#validateMagicMerge(merged, codePoint1, codePoint2);
        return merged;
    }

    // codePoint(magicCode) {
    //     return this.#magicalTable[magicCode];
    // }

    #resetCache () {
        this.#magicalTable = {
            length: 0,
            lastMagicCode: 0n
        };
        this.#mergedMagicalValues = [];
        this.#shiftedMagicalValues = {};
        this.#lastShiftedMagicCodePoint = -1n;
    }

    #calculateMagicTable() {
        this.#resetCache();
        for (let codePoint in this.#canonicalCompositions) {
            const [cp1, cp2] = this.#canonicalCompositions[codePoint];
            const magicVal = this.magicMerge(cp1, cp2);
            if (cp1 !== cp2 && magicVal === this.magicMerge(cp2, cp1)) {
                throw new Error(`Magic merge algorithm error; cp1: ${cp1}, cp2: ${cp2}, merge1: ${magicVal}, merge2: ${this.magicMerge(cp2, cp1)}`);
            }
            this.#magicalTable[magicVal] = parseInt(codePoint);
            ++this.#magicalTable.length;
            if (magicVal > this.#magicalTable.lastMagicCode) {
                this.#magicalTable.lastMagicCode = magicVal;
            }
            const shifted = this.shiftCodePoint(magicVal);
            if (shifted > this.#lastShiftedMagicCodePoint) {
                this.#lastShiftedMagicCodePoint = shifted;
            }
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

        let maps = [];
        for (const codePoint in this.#canonicalCompositions) {
            let [cp1, cp2] = this.#canonicalCompositions[codePoint];
            cp1 &= this.#codePoint1Mask;
            // cp1 -= this.#codePoint1Compl;
            cp2 &= this.#codePoint2Mask;
            // cp2 -= this.#codePoint2Compl;
            maps.push(interleaveBits(cp1, cp2));
        }
        this.#magicFinalMask = findSmallestMask(maps);
        this.#magicFinalCompl = findSmallestComplement(maps.map(cp => cp & this.#magicFinalMask));

        console.log("Code Point 1 Mask:", this.#codePoint1Mask);
        console.log("Code Point 1 Complement:", this.#codePoint1Compl);
        console.log("Code Point 2 Mask:", this.#codePoint2Mask);
        console.log("Code Point 2 Complement:", this.#codePoint2Compl);
        console.log("Magic Mask:", this.#magicFinalMask);
        console.log("Magic Complement:", this.#magicFinalCompl);
        console.log("Last Mapped:", this.lastMapped);
    }

    async load() {
        const {data, lastMapped} = await getCanonicalDecompositions();
        this.#canonicalCompositions = data;
        this.lastMapped = lastMapped;
        const maps = await extractedCanonicalDecompositions(this.#canonicalCompositions);
        this.#calculateCodePointMasks(maps.mappedToFirst, maps.mappedToSecond);
        if (this.shiftCodePoint(this.lastMagic) >= this.lastMappedBucket) {
            throw new Error(`Out of range last magic code point: ${this.lastMagic} (shifted: ${this.shiftCodePoint(this.lastMagic)}) / ${this.lastMappedBucket}`);
        }
    }

    calculateMagicalTable () {
        this.#calculateMagicTable();
        this.#calculateTopEmptyRanges();
    }

    needsModification(magicCode) {
        return magicCode in this.#magicalTable;
    }

    utf8Composed(magicCode) {
        if (!this.needsModification(magicCode)) {
            return new Uint8Array(0);
        }
        return utf32To8(this.#magicalTable[magicCode]);
    }

    codePoint(codePoint, invalidCodePoint = 0) {
        if (this.#magicalTable?.[codePoint] === invalidCodePoint) {
            throw new Error(`Invalid "invalid code point" specified: table[${codePoint}] == ${this.#magicalTable[codePoint]} == ${invalidCodePoint}`);
        }
        return this.#magicalTable?.[codePoint] || invalidCodePoint;
    }

    table(codePointSTart, length, invalidCodePoint = 0) {
        const end = codePointSTart + length;
        let tbl = [];
        for (let codePoint = codePointSTart; codePoint !== end; ++codePoint) {
            tbl.push(this.codePoint(codePoint, invalidCodePoint));
        }
        return tbl;
    }

    get magicTable() {
        return this.#magicalTable;
    }

    get lastShiftedMagicCode() {
        return this.#lastShiftedMagicCodePoint;
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
        const {mappedTo} = table.at(Number(codePoint));
        const requiredLength = BigInt(mappedTo.length) + BigInt(utf8.length) + 1n; // +1 for the additional '0' that used as a separator
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
        static constexpr std::size_t cp1_mask = 0x${this.#codePoint1Mask.toString(16).toUpperCase()}U;
        static constexpr std::size_t cp2_mask = 0x${this.#codePoint2Mask.toString(16).toUpperCase()}U;
        static constexpr std::size_t interleaved_magic_mask = 0x${this.#magicFinalMask.toString(16).toUpperCase()}U;
        static constexpr std::size_t interleaved_magic_compl = 0x${this.#magicFinalCompl.toString(16).toUpperCase()}U;
        static constexpr std::size_t last_mapped_bucket = 0x${this.lastMappedBucket.toString(16).toUpperCase()}U;
        
        /// This is a magical formula that absolutely does not make sense, but it works because math is magical.
        /// This will merge the 2 code points into one single value that then can be used to get the position of the
        /// values in the values table.
        template <typename CharT = char32_t>
        [[nodiscard]] static constexpr std::size_t magic_merge(CharT cp1, CharT cp2) noexcept {
            cp1 &= cp1_mask;
            cp2 &= cp2_mask;
            const auto intlvd = webpp::interleave_bits(static_cast<std::uint16_t>(cp1), static_cast<std::uint16_t>(cp2));
            const auto merged = (intlvd & interleaved_magic_mask) - interleaved_magic_compl;
            return (((merged >> chunk_shift) % last_mapped_bucket) << chunk_shift) | (merged & chunk_mask);
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
