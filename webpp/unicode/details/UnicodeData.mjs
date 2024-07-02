import {cleanComments, downloadFile, parseCodePoints, splitLine, updateProgressBar, noop} from "./utils.mjs";

export const fileUrl = 'https://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt';
export const cacheFilePath = 'UnicodeData.txt';

export const properties = {
    codePoints: Symbol("Explicitly Mentioned Code Points"),
    ccc: Symbol("Canonical Combining Class"),
    decompositionType: Symbol("Decomposition Tables"),
};

export const download = async (callback = noop) => {
    // database file
    return await downloadFile(fileUrl, cacheFilePath, async (fileContent) => {
        await callback(fileContent);
    });
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
            let lastCodePoint = 0;
            action = ({codePointStr, CanonicalCombiningClass}) => {
                const ccc = parseInt(CanonicalCombiningClass);
                const codePoint = parseCodePoints(codePointStr);
                for (let curCodePoint = lastCodePoint; curCodePoint <= codePoint; ++curCodePoint) {
                    const curCCC = curCodePoint === codePoint ? ccc : 0;

                    table.add(curCodePoint, curCCC);
                }
                lastCodePoint = codePoint + 1;
            };
            break;
        }

        case properties.decompositionType: {
            let lastCodePoint = 0;
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
                let formattingTag = "";

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
            action = ({codePointStr, DecompositionStr}) => {
                const codePoint = parseCodePoints(codePointStr);
                const decomposition = parseDecompositions(codePoint, DecompositionStr);

                for (let curCodePoint = lastCodePoint; curCodePoint <= codePoint; ++curCodePoint) {
                    const curDecompositionMapping = curCodePoint === codePoint ? decomposition : parseDecompositions(curCodePoint);
                    table.add(curCodePoint, curDecompositionMapping);
                }
                lastCodePoint = codePoint + 1;
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
            DecompositionStr,        // #5 Decomp
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
            SimpleTitlecaseMapping
        });
    });
    updateProgressBar(100, `Lines parsed: ${lines.length}`);
};

