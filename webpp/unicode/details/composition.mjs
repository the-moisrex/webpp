import {getCanonicalDecompositions} from "./UnicodeData.mjs";

class Composition {

    #canonicalCompositions = {};
    #mergedMagicalValues = [];

    async load() {
        this.#canonicalCompositions = await getCanonicalDecompositions();
    }

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

    needsModification(codePoint) {
        return codePoint in this.#canonicalCompositions;
    }

    needsModificationRange(codePointStart, codePointEnd) {
        for (let codePoint = codePointStart; codePoint !== codePointEnd; ++codePoint) {
            if (this.needsModification(codePoint)) {
                return true;
            }
        }
        return false;
    }

    modify({codePointStart, codePointEnd, modifier, inserts}) {

    }
}


if (process.argv[1] === new URL(import.meta.url).pathname) {
    const comp = new Composition();
    await comp.load();
    console.log('61: ', comp.needsModification(61));
    console.log('fb32: ', comp.needsModification(0xfb32));
}
