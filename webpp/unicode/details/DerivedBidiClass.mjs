import { splitLine, cleanComments, downloadFile, updateProgressBar, parseCodePointRangeExclusive, noop } from "./utils.mjs";

export const fileUrl =
    "https://www.unicode.org/Public/UCD/latest/ucd/extracted/DerivedBidiClass.txt";
export const cacheFilePath = "DerivedBidiClass.txt";

// These values need to be the exact values in the C++ code base (in bidi.hpp file)
export const bidiDirections = {
    NONE: 0,

    // ---------- Strong -----------------------------------------------
    L: 1,  // Left to right - most letters in LTR scripts
    R: 2,  // Right to left - most letters in non-Arabic RTL scripts
    AL: 3, // Arabic letters - most letters in the Arabic script

    // ----------  Weak  -----------------------------------------------
    EN: 4,  // European Number (0-9, and Extended Arabic-Indic numbers)
    ES: 5,  // European Number Separator (+ and -)
    ET: 6,  // European Number Terminator (currency symbols, the hash sign, the percent sign and so on)
    AN: 7,  // Arabic Number; this encompasses the Arabic-Indic numbers, but not the Extended Arabic-Indics
    CS: 8,  // Common Number Separator (. , / : et al)
    NSM: 9, // Nonspacing Mark - most combining accents
    BN: 10,  // Boundary Neutral - control characters (ZWNJ, ZWJ, and others)

    // ---------- Neutral -----------------------------------------------
    B: 11,  // Paragraph Separator
    S: 12,  // Segment Separator
    WS: 13, // Whitespace, including the SPACE character
    ON: 14, // Other Neutrals, including @, &, parentheses, MIDDLE DOT

    // ---------- Explicit Formatting -----------------------------------
    // these are "directional control characters" and are not used in IDNA labels.
    LRE: 15, // Left-to-Right Embedding
    LRO: 16, // Left-to-Right Override
    RLE: 17, // Right-to-Left Embedding
    RLO: 18, // Right-to-Left Override
    PDF: 19, // Pop Directional Format
    LRI: 20, // Left-to-Right Isolate
    RLI: 21, // Right-to-Left Isolate
    FSI: 22, // First Strong Isolate
    PDI: 23, // Pop Directional Isolate
}

export const download = async (callback = noop) => {
    // database file
    return await downloadFile(fileUrl, cacheFilePath, async (fileContent) => {
        await callback(fileContent);
    });
};

export const parse = async (table, fileContent = undefined) => {
    if (fileContent === undefined) {
        fileContent = await download();
        if (fileContent === undefined) {
            throw new Error(`We were unable to get the ${cacheFilePath} file.`);
        }
    }

    const lines = fileContent.split("\n");
    const data = [];

    lines.forEach((line, index) => {
        line = cleanComments(line);

        // ignore empty lines
        if (line.length === 0) {
            return "";
        }

        let [
            codePointStr,
            BidiClass,
        ] = splitLine(line);

        updateProgressBar((index / lines.length) * 100);

        if (bidiDirections[BidiClass] === undefined) {
            throw new Error(`Invalid property name found (${property}); is there a new update to Unicode?`);
        }

        const [codePointStart, codePointEnd] = parseCodePointRangeExclusive(codePointStr);
        data.push({codePointStart, codePointEnd, codePointStr, BidiClass: bidiDirections[BidiClass]});
    });

    data.sort((lhs, rhs) => Number(lhs.codePointStart) - Number(rhs.codePointStart));

    let lastCodePoint = 0n;
    for (const info of data) {
        for (let cur = lastCodePoint; cur < info.codePointStart; ++cur) {
            table.add(cur, bidiDirections["NONE"]);
        }
        for (let cur = info.codePointStart; cur <= info.codePointEnd; ++cur) {
            table.add(cur, info.BidiClass);
        }
        lastCodePoint = info.codePointEnd + 1n;
    }

    updateProgressBar(100, `Lines parsed: ${lines.length}`);
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
