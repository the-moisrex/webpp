import {getCanonicalDecompositions} from "./UnicodeData.mjs";
import {utf32To8} from "./utils.mjs";
import * as assert from "node:assert";

export class CanonicalComposition {

    #canonicalCompositions = {};
    #mergedMagicalValues = [];

    #magicalTable = {};

    #validateMagicMerge(merged) {
        if (this.#mergedMagicalValues.includes(merged)) {
            throw new Error("Magical Merging Formula does not produce unique values anymore: " + merged);
        }
        this.#mergedMagicalValues.push(merged);
    }

    /// Magical Formula
    /// Do NOT try to make sense of this algorithm, it's random with no meaning.
    /// Its purpose is only to generate a set of merged values that the merged values
    ///   are unique in the current Unicode composition database.
    magicMerge(codePoint1, codePoint2) {
        const merged = (codePoint1 + (codePoint1 >> 2)) * codePoint2;
        this.#validateMagicMerge(merged);
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

}


if (process.argv[1] === new URL(import.meta.url).pathname) {
    const comp = new CanonicalComposition();
    await comp.load();
    console.log('61: ', comp.needsModification(61));
    console.log('fb32: ', comp.needsModification(0xfb32));
}
