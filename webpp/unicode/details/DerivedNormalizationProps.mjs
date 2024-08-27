import {
    cleanComments,
    downloadFile,
    noop,
    parseCodePointRange,
    splitLine,
    updateProgressBar,
} from "./utils.mjs";

export const fileUrl =
    "https://www.unicode.org/Public/UCD/latest/ucd/DerivedNormalizationProps.txt";
export const cacheFilePath = "DerivedNormalizationProps.txt";

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

    let lastCodePoint = 0;
    const action = (meta) => {
        const codePointStr = meta.codePointStr;
        const [codePointStart, codePointEnd] = parseCodePointRange(
            codePointStr,
            lastCodePoint,
        );
        for (
            let curCodePoint = codePointStart;
            curCodePoint <= codePointEnd;
            ++curCodePoint
        ) {
            table.add(curCodePoint, meta);
        }
        lastCodePoint = codePointEnd + 1;
    };

    lines.forEach((line, index) => {
        line = cleanComments(line);

        // ignore empty lines
        if (line.length === 0) {
            return "";
        }

        // https://www.unicode.org/reports/tr44/#DerivedNormalizationProps.txt
        const [
            codePointStr, // #0
            normalizationForm,
            mappedTo,
        ] = splitLine(line);

        updateProgressBar((index / lines.length) * 100);

        action({
            codePointStr,
            normalizationForm,
            mappedTo,
        });
    });
    updateProgressBar(100, `Lines parsed: ${lines.length}`);
};

export const getCompositionExclusions = async () => {
    class GetTable {
        #data = [];

        add(codePoint, { normalizationForm, mappedTo }) {
            if (normalizationForm === "Full_Composition_Exclusion") {
                this.#data.push(codePoint);
            }
        }

        get data() {
            return this.#data;
        }
    }

    const table = new GetTable();
    await parse(table);

    return table.data;
};

if (process.argv[1] === new URL(import.meta.url).pathname) {
    const exclusions = await getCompositionExclusions();
    for (const codePoint of exclusions) {
        console.log(codePoint.toString(16));
    }
    console.log("Table length:", exclusions.length);
    console.log("Start Code Point:", exclusions[0]);
    console.log("Last Code Point:", exclusions[exclusions.length - 1]);
}
