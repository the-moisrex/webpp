import {
    splitLine, cleanComments, downloadFile, updateProgressBar, parseCodePointRangeExclusive, noop
} from "./utils.mjs";

export const fileUrl = "https://www.unicode.org/Public/UCD/latest/ucd/extracted/DerivedJoiningType.txt";
export const cacheFilePath = "DerivedJoiningType.txt";
export const JoiningTypes = {
    U: 0,
    R: 1,
    L: 2,
    D: 3,
    C: 4,
    T: 5,
    Non_Joining: 0,
    Right_Joining: 1,
    Left_Joining: 2,
    Dual_Joining: 3,
    Join_Causing: 4,
    Transparent: 5,
    0: "Non_Joining",
    1: "Right_Joining",
    2: "Left_Joining",
    3: "Dual_Joining",
    4: "Join_Causing",
    5: "Transparent",
};

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
    let data = [];

    lines.forEach((line, index) => {
        line = cleanComments(line);

        // ignore empty lines
        if (line.length === 0) {
            return "";
        }

        const [codePointStr, joinType] = splitLine(line);
        const [codePointStart, codePointEnd] = parseCodePointRangeExclusive(codePointStr);
        data.push({
            codePointStart, codePointEnd, joinType
        });

        updateProgressBar((index / lines.length) * 100);
    });

    data.sort((a, b) => Number(a.codePointStart - b.codePointStart));

    let lastCodePoint = 0n;
    for (const {codePointStart, codePointEnd, joinType} of data) {
        for (let codePoint = lastCodePoint; codePoint < codePointStart; ++codePoint) {
            table.add(codePoint, JoiningTypes["Non_Joining"]);
        }
        // if (codePointStart === codePointEnd) {
        //     console.log(codePointStart.toString(16).toUpperCase(), codePointEnd, joinType, lastCodePoint);
        // }
        for (let codePoint = codePointStart; codePoint <= codePointEnd; ++codePoint) {
            table.add(codePoint, JoiningTypes[joinType]);
        }
        lastCodePoint = codePointEnd + 1n;
    }

    updateProgressBar(100, `Lines parsed: ${lines.length}`);
};

