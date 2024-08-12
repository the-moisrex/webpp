import {
    cleanComments,
    downloadFile,
    parseCodePoints,
    splitLine,
    updateProgressBar,
    noop,
    findSmallestMask,
    findSmallestComplement,
    interleaveBits,
    findSmallestDivision,
    findSmallestXor,
    hasDuplicates,
    maskFinder
} from "./utils.mjs";
import {getCompositionExclusions} from "./DerivedNormalizationProps.mjs";
import * as assert from "node:assert";

export const fileUrl = 'https://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt';
export const cacheFilePath = 'UnicodeData.txt';

export const properties = {
    codePoints: Symbol("Explicitly Mentioned Code Points"),
    ccc: Symbol("Canonical Combining Class"),
    decompositionType: Symbol("Decomposition Tables"),
    canonicalDecompositionType: Symbol("Canonical-only Decomposition Tables"),
};

export const download = async (callback = noop) => {
    // database file
    return await downloadFile(fileUrl, cacheFilePath, async (fileContent) => {
        await callback(fileContent);
    });
};

const parseDecompositions = (codePoint, str = "") => {
    str = str.trim();
    if (str === "") {
        // The Standard says:
        // The default value of the Decomposition_Mapping property is the code point itself.
        // From: https://www.unicode.org/reports/tr44/#Character_Decomposition_Mappings
        return {mappedTo: [codePoint], mapped: false, formattingTag: null};
    }
    let parts = str.split(" ").map(item => item.trim());

    // Parsing Compatibility Formatting Tag
    // https://www.unicode.org/reports/tr44/#Formatting_Tags_Table
    let formattingTag = null;

    // Checking for the unthinkable:
    parts = parts.filter(part => {
        if (part.startsWith("<") && part.endsWith(">")) {
            formattingTag = part.substring(1, part.length - 1);
            return false;
        }
        return true;
    });

    parts = parts.map(codePoint => parseCodePoints(codePoint));

    return {mappedTo: parts, mapped: true, formattingTag};
};

export const parse = async (table, property, fileContent = undefined) => {
    if (fileContent === undefined) {
        fileContent = await download();
        if (fileContent === undefined) {
            throw new Error(`We were unable to get the ${cacheFilePath} file.`);
        }
    }

    const lines = fileContent.split('\n');

    let action = noop;

    switch (property) {

        /// simple code points
        case properties.codePoints:
            action = ({codePointStr}) => {
                const codePoint = parseCodePoints(codePointStr);
                table.add(codePoint);
            };
            break;

        /// Canonical Combining Class:
        case properties.ccc: {
            let lastCodePoint = 0n;
            action = ({codePointStr, CanonicalCombiningClass}) => {
                const ccc = parseInt(CanonicalCombiningClass);
                const codePoint = parseCodePoints(codePointStr);
                for (let curCodePoint = lastCodePoint; curCodePoint <= codePoint; ++curCodePoint) {
                    const curCCC = curCodePoint === codePoint ? ccc : 0n;

                    table.add(curCodePoint, curCCC);
                }
                lastCodePoint = codePoint + 1n;
            };
            break;
        }

        case properties.decompositionType: {
            let lastCodePoint = 0n;
            action = ({codePointStr, DecompositionStr}) => {
                const codePoint = parseCodePoints(codePointStr);
                const decomposition = parseDecompositions(codePoint, DecompositionStr);

                for (let curCodePoint = lastCodePoint; curCodePoint <= codePoint; ++curCodePoint) {
                    const curDecompositionMapping = curCodePoint === codePoint ? decomposition : parseDecompositions(curCodePoint);
                    table.add(curCodePoint, curDecompositionMapping);
                }
                lastCodePoint = codePoint + 1n;
            };
            break;
        }

        case properties.canonicalDecompositionType: {
            action = ({codePointStr, DecompositionStr}) => {
                const codePoint = parseCodePoints(codePointStr);
                const decomposition = parseDecompositions(codePoint, DecompositionStr);
                const isCanonicalDecomposition = decomposition.formattingTag === null;

                // The prefixed tags supplied with a subset of the decomposition mappings generally indicate formatting information.
                // Where no such tag is given, the mapping is canonical.
                if (!isCanonicalDecomposition || !decomposition.mapped || decomposition.mappedTo.length !== 2) {
                    return;
                }

                table.add(codePoint, decomposition);
            };
            break;
        }

        // invalid property:
        default:
            throw new Error("Invalid property provided.");
    }

    lines.forEach((line, index) => {
        line = cleanComments(line);

        // ignore empty lines
        if (line.length === 0) {
            return "";
        }

        // Values defined in: https://www.unicode.org/reports/tr44/#UnicodeData.txt
        const [
            codePointStr,            // #0
            codePointName,           // #1
            GeneralCategory,         // #2
            CanonicalCombiningClass, // #3 CCC
            BidiClass,               // #4
            DecompositionStr,        // #5 Decomp: https://www.unicode.org/reports/tr44/#Character_Decomposition_Mappings
            Numeric,                 // #5
            BidiMirrored,            // #6
            Unicode1Name,            // #7
            ISOComment,              // #8
            SimpleUppercaseMapping,  // #9
            SimpleLowercaseMapping,  // #10
            SimpleTitlecaseMapping   // #11
        ] = splitLine(line);

        updateProgressBar(index / lines.length * 100);

        action({
            codePointStr,
            codePointName,
            GeneralCategory,
            CanonicalCombiningClass,
            BidiClass,
            DecompositionStr,
            Numeric,
            BidiMirrored,
            Unicode1Name,
            ISOComment,
            SimpleUppercaseMapping,
            SimpleLowercaseMapping,
            SimpleTitlecaseMapping,
        });
    });
    updateProgressBar(100, `Lines parsed: ${lines.length}`);
};


/// Field number 5 (Decomposition), excluding those that have a tag (contains <something>)
/// Explained in https://www.unicode.org/reports/tr44/#Character_Decomposition_Mappings
export const getCanonicalDecompositions = async () => {
    class GetTable {
        #data = {};

        add(codePoint, {mappedTo}) {
            assert.ok(mappedTo.length === 2);
            this.#data[codePoint] = mappedTo;
        }

        get data() {
            return this.#data;
        }
    }

    const table = new GetTable();
    await parse(table, properties.canonicalDecompositionType);

    return table.data;
};

export const extractedCanonicalDecompositions = async (data = null) => {
    if (data === null || data === undefined) {
        data = await getCanonicalDecompositions();
    }

    // console.log([...Object.values(data)/*.map(item => item?.[0] || undefined)*/]);

    return {
        codePoints: Object.keys(data),

        mappedToFirst: Object.values(data)
            .map(datum => datum?.[0]) // first one
            .filter(codePoint => codePoint !== undefined)
            .reduce((accum, value) => {
                if (!accum.includes(value)) {
                    accum.push(value);
                }
                return accum;
            }, []) // remove duplicates
            .toSorted((a, b) => a - b),

        mappedToSecond: Object.values(data)
            .map(datum => datum?.[1]) // second one
            .filter(codePoint => codePoint !== undefined)
            .reduce((accum, value) => {
                if (!accum.includes(value)) {
                    accum.push(value);
                }
                return accum;
            }, []) // remove duplicates
            .toSorted((a, b) => a - b),
    };
};

if (process.argv[1] === new URL(import.meta.url).pathname) {
    switch (process.argv[2]) {
        case "code-points": {
            const maps = await extractedCanonicalDecompositions();
            for (const codePoint of maps.codePoints) {
                console.log(codePoint.toString(16));
            }
            break;
        }
        case "map1":
        case "map-first":
        case "mapped-first":
        case "mapped-to-first": {
            const maps = await extractedCanonicalDecompositions();
            for (const codePoint of maps.mappedToFirst) {
                console.log(codePoint);
            }
            break;
        }
        case "map2":
        case "map-second":
        case "mapped-second":
        case "mapped-to-second": {
            const maps = await extractedCanonicalDecompositions();
            for (const codePoint of maps.mappedToSecond) {
                console.log(codePoint);
            }
            break;
        }

        case "map1-mask": {
            const maps = (await extractedCanonicalDecompositions()).mappedToFirst;
            console.log(maps.join(", "));
            const mask = findSmallestMask(maps);
            console.log("Mask:", mask);
            console.log(maps.map(cp => cp & mask).join(", "));
            break;
        }

        case "map2-mask": {
            const maps = (await extractedCanonicalDecompositions()).mappedToSecond;
            console.log(maps.join(", "));
            const mask = findSmallestMask(maps);
            console.log("Mask:", mask);
            console.log(maps.map(cp => cp & mask).join(", "));
            break;
        }

        case "map1-compl": {
            const maps = (await extractedCanonicalDecompositions()).mappedToFirst;
            console.log(maps.join(", "));
            const compl = findSmallestComplement(maps);
            console.log("Compl:", compl);
            console.log(maps.map(cp => cp - compl).join(", "));
            break;
        }

        case "map2-compl": {
            const maps = (await extractedCanonicalDecompositions()).mappedToSecond;
            console.log(maps.join(", "));
            const compl = findSmallestComplement(maps);
            console.log("Compl:", compl);
            console.log(maps.map(cp => cp - compl).join(", "));
            break;
        }

        case "map1-compl-masked": {
            const maps = (await extractedCanonicalDecompositions()).mappedToFirst;
            console.log(maps.join(", "));
            const mask = findSmallestMask(maps);
            const compl = findSmallestComplement(maps.map(item => item & mask));
            console.log("Compl:", compl);
            console.log("Mask:", mask);
            console.log(maps.map(cp => (cp & mask) - compl).join(", "));
            break;
        }

        case "map2-compl-masked": {
            const maps = (await extractedCanonicalDecompositions()).mappedToSecond;
            console.log(maps.join(", "));
            const mask = findSmallestMask(maps);
            const compl = findSmallestComplement(maps.map(item => item & mask));
            console.log("Compl:", compl);
            console.log("Mask:", mask);
            console.log(maps.map(cp => (cp & mask) - compl).join(", "));
            break;
        }

        case "map-interleave": {
            const data = await getCanonicalDecompositions();
            const {mappedToSecond, mappedToFirst} = (await extractedCanonicalDecompositions());
            const maps = [];
            const cp1Mask = findSmallestMask(mappedToFirst);
            const cp2Mask = findSmallestMask(mappedToSecond);
            const cp1Compl = findSmallestComplement(mappedToFirst.map(cp => cp & cp1Mask));
            const cp2Compl = findSmallestComplement(mappedToSecond.map(cp => cp & cp2Mask));
            console.log(`cp1 mask:`, cp1Mask);
            console.log(`cp2 mask:`, cp2Mask);
            console.log(`cp1 compl:`, cp1Compl);
            console.log(`cp2 compl:`, cp2Compl);
            for (const codePoint in data) {
                let [cp1, cp2] = data[codePoint];
                cp1 &= cp1Mask;
                // cp1 -= cp1Compl;
                cp2 &= cp2Mask;
                // cp2 -= cp2Compl;
                maps.push(interleaveBits(cp1, cp2));
            }
            console.log(maps.join(", "));
            const mask = findSmallestMask(maps);
            console.log("Mask:", mask);
            // const compl = 0;
            const compl = findSmallestComplement(maps.map(item => item & mask));
            console.log("Compl:", compl);
            const modified = maps.map(cp => (cp & mask) - compl);
            console.log(modified.join(", "));
            console.log(hasDuplicates(modified) ? "Failed: Has duplicates" : "Success: No duplicates");
            break;
        }

        case "map-interleave-reversed": {
            const data = await getCanonicalDecompositions();
            const {mappedToSecond, mappedToFirst} = (await extractedCanonicalDecompositions());
            const maps = [];
            const cp1Compl = findSmallestComplement(mappedToFirst);
            const cp2Compl = findSmallestComplement(mappedToSecond);
            const cp1Mask = findSmallestMask(mappedToFirst.map(cp => cp - cp1Compl));
            const cp2Mask = findSmallestMask(mappedToSecond.map(cp => cp - cp2Compl));
            console.log(`cp1 mask:`, cp1Mask);
            console.log(`cp2 mask:`, cp2Mask);
            console.log(`cp1 compl:`, cp1Compl);
            console.log(`cp2 compl:`, cp2Compl);
            for (const codePoint in data) {
                let [cp1, cp2] = data[codePoint];
                cp1 -= cp1Compl;
                cp1 &= cp1Mask;
                cp2 -= cp2Compl;
                cp2 &= cp2Mask;
                maps.push(interleaveBits(cp1, cp2));
            }
            console.log(maps.join(", "));
            // const compl = 0;
            const compl = findSmallestComplement(maps);
            console.log("Compl:", compl);
            const mask = findSmallestMask(maps.map(item => item - compl));
            console.log("Mask:", mask);
            console.log(maps.map(cp => (cp - compl) & mask).join(", "));
            break;
        }

        case "map-find-mask": {
            const data = await getCanonicalDecompositions();
            const {mappedToSecond, mappedToFirst} = (await extractedCanonicalDecompositions());
            const maps = [];
            const cp1Mask = findSmallestMask(mappedToFirst);
            const cp2Mask = findSmallestMask(mappedToSecond);
            const cp1Compl = findSmallestComplement(mappedToFirst.map(cp => cp & cp1Mask));
            const cp2Compl = findSmallestComplement(mappedToSecond.map(cp => cp & cp2Mask));
            console.log(`cp1 mask:`, cp1Mask);
            console.log(`cp2 mask:`, cp2Mask);
            console.log(`cp1 compl:`, cp1Compl);
            console.log(`cp2 compl:`, cp2Compl);
            for (const codePoint in data) {
                let [cp1, cp2] = data[codePoint];
                cp1 &= cp1Mask;
                // cp1 -= cp1Compl;
                cp2 &= cp2Mask;
                // cp2 -= cp2Compl;
                maps.push(interleaveBits(cp1, cp2));
            }
            console.log(maps.join(", "));
            // const mask = findSmallestMask(maps);
            for (const mask of maskFinder(maps)) {
                console.log("--------------")
                console.log("Mask:", mask);
                // const compl = 0;
                const compl = findSmallestComplement(maps.map(item => item & mask));
                console.log("Compl:", compl);
                const modified = maps.map(cp => ((cp & mask) - compl) % 3049);
                // console.log(modified.join(", "));
                const hasDups = hasDuplicates(modified);
                console.log(hasDups ? "Failed: Has duplicates" : "Success: No duplicates");
                if (!hasDups) {
                    break;
                }
            }
            break;
        }

        case "merged": {
            const maps = await getCanonicalDecompositions();
            let cps = [];
            for (let codePoint in maps) {
                codePoint = parseInt(codePoint);
                let [cp1, cp2] = maps[codePoint];
                cp2 = cp2 || 0;
                const merged = (cp1 + (cp1 >> 2)) * cp2;
                // console.log("Size: ", cp1.toString(2).length + cp2.toString(2).length);
                if (cps.includes(merged)) {
                    console.log("Conflict: ", codePoint.toString(16), merged, cp1.toString(16), cp2.toString(16));
                    continue;
                }
                cps.push(merged);
                console.log(codePoint.toString(16), cp1.toString(16), cp2.toString(16));
            }
            break;
        }

        default: {
            const maps = await getCanonicalDecompositions();
            for (let codePoint in maps) {
                codePoint = parseInt(codePoint);
                console.log(codePoint.toString(16), maps[codePoint].map(item => item.toString(16)).join(", "));
            }
            const keys = Object.keys(maps).toSorted((a, b) => parseInt(a) - parseInt(b));
            console.log("Table length:", keys.length);
            console.log("Start Code Point:", maps[keys[0]]);
            console.log("Last Code Point:", maps[keys[keys.length - 1]]);
        }
    }
}
