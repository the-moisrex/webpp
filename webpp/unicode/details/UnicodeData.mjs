import {cleanComments, downloadFile, parseCodePoints, splitLine, updateProgressBar, noop} from "./utils.mjs";

export const fileUrl = 'https://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt';
export const cacheFilePath = 'UnicodeData.txt';

export const properties = {
    codePoints: Symbol("Explicitly Mentioned Code Points"),
    ccc: Symbol("Canonical Combining Class"),
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
        case properties.ccc:
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

        const [codePointStr, codePointName, GeneralCategory, CanonicalCombiningClass, BidiClass,
            DecompositionType,
            // DecompositionMapping,
            NumericType,
            // NumericValue,
            BidiMirrored, Unicode1Name, ISOComment, SimpleUppercaseMapping, SimpleLowercaseMapping,
            SimpleTitlecaseMapping] = splitLine(line);

        updateProgressBar(index / lines.length * 100);

        action({
            codePointStr, codePointName, GeneralCategory, CanonicalCombiningClass, BidiClass,
            DecompositionType,
            NumericType,
            BidiMirrored, Unicode1Name, ISOComment, SimpleUppercaseMapping, SimpleLowercaseMapping,
            SimpleTitlecaseMapping
        });
    });
    updateProgressBar(100, `Lines parsed: ${lines.length}`);
};

