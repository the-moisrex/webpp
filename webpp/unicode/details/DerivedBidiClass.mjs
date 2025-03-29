import {
    splitLine, cleanComments, downloadFile,
    updateProgressBar, parseCodePointRangeExclusive,
    noop, getMostSpecializedIn
} from "./utils.mjs";
import * as UnicodeData from "./UnicodeData.mjs";
import {properties} from "./UnicodeData.mjs";
import {bidiDirections} from "./bidi.mjs";

export const fileUrl = "https://www.unicode.org/Public/UCD/latest/ucd/extracted/DerivedBidiClass.txt";
export const cacheFilePath = "DerivedBidiClass.txt";

export const download = async (callback = noop) => {
    // database file
    return await downloadFile(fileUrl, cacheFilePath, async (fileContent) => {
        await callback(fileContent);
    });
};

export const parse = async (table, DerivedBidiClassFileContent = undefined) => {
    if (DerivedBidiClassFileContent === undefined) {
        DerivedBidiClassFileContent = await download();
        if (DerivedBidiClassFileContent === undefined) {
            throw new Error(`We were unable to get the ${cacheFilePath} file.`);
        }
    }

    const lines = DerivedBidiClassFileContent.split("\n");
    const data = [];
    const missings = [];

    // Starting with Version 15.0, some data files in the UCD may contain multiple @missing lines
    // defined for the same property.
    // When multiple @missing lines are defined this way, they are to be
    // interpreted as follows: Each successive @missing line specifies an overriding range
    // value for all previous @missing definitions.
    // This convention allows a generic default value to be specified first for the
    // entire Unicode code point range, followed by other specific default values
    // for more constrained, specific sub-ranges. This enables an easy-to-understand
    // and easy-to-maintain way of handling complex default values, as for the Bidi_Class
    // or Line_Break properties. (See Complex Default Values.)
    lines.forEach((line) => {
        if (line.startsWith("# @missing:")) {
            const [codePointStr, bidiClassStr] = line.split(':')[1].split(';');
            const bidiClass = bidiDirections[bidiClassStr.trim()];
            const [starting, ending] = parseCodePointRangeExclusive(codePointStr);
            missings.push({
                starting, ending, bidiClass, bidiClassStr: bidiClassStr.trim()
            });
        }
    });

    lines.forEach((line, index) => {
        line = cleanComments(line);

        // ignore empty lines
        if (line.length === 0) {
            return "";
        }

        let [codePointStr, BidiClass] = splitLine(line);

        updateProgressBar((index / lines.length) * 100);

        if (bidiDirections[BidiClass] === undefined) {
            throw new Error(`Invalid property name found (${BidiClass}); is there a new update to Unicode?`);
        }

        const [codePointStart, codePointEnd] = parseCodePointRangeExclusive(codePointStr);
        data.push({codePointStart, codePointEnd, codePointStr, BidiClass: bidiDirections[BidiClass]});
    });

    data.sort((lhs, rhs) => Number(lhs.codePointStart) - Number(rhs.codePointStart));

    let lastCodePoint = 0n;
    for (const info of data) {
        for (let cur = lastCodePoint; cur < info.codePointStart; ++cur) {
            const curMissing = getMostSpecializedIn(cur, missings);
            // table.add(cur, bidiDirections["NONE"]);
            // console.log(cur, curMissing);
            table.add(cur, curMissing?.bidiClass);
        }
        for (let cur = info.codePointStart; cur < info.codePointEnd; ++cur) {
            table.add(cur, info.BidiClass);
        }
        lastCodePoint = info.codePointEnd;
    }

    updateProgressBar(100, `Lines parsed: ${lines.length}`);

    await UnicodeData.parse(table, properties.bidi);
};


export const getBidiClasses = async () => {
    class GetTable {
        #data = {};

        add(codePoint, BidiClass) {
            this.#data[codePoint] = BidiClass;
        }

        get data() {
            return this.#data;
        }
    }

    const table = new GetTable();
    await parse(table);

    return table.data;
};
