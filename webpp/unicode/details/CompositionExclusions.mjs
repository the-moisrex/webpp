import {
    cleanComments,
    downloadFile,
    noop,
    parseCodePointRange,
    splitLine,
    updateProgressBar,
} from "./utils.mjs";

export const fileUrl =
    "https://www.unicode.org/Public/UCD/latest/ucd/CompositionExclusions.txt";
export const cacheFilePath = "CompositionExclusions.txt";

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
    const action = ({ codePointStr }) => {
        const [codePointStart, codePointEnd] = parseCodePointRange(
            codePointStr,
            lastCodePoint,
        );
        for (
            let curCodePoint = codePointStart;
            curCodePoint <= codePointEnd;
            ++curCodePoint
        ) {
            table.add(curCodePoint, {});
        }
        lastCodePoint = codePointEnd + 1;
    };

    lines.forEach((line, index) => {
        line = cleanComments(line);

        // ignore empty lines
        if (line.length === 0) {
            return "";
        }

        const [
            codePointStr, // #0
        ] = splitLine(line);

        updateProgressBar((index / lines.length) * 100);

        action({
            codePointStr,
        });
    });
    updateProgressBar(100, `Lines parsed: ${lines.length}`);
};

if (process.argv[1] === new URL(import.meta.url).pathname) {
    class printTable {
        #data = [];
        add(codePoint) {
            this.#data.push(codePoint);
        }
        printAll() {
            for (const codePoint of this.#data) {
                console.log(codePoint.toString(16));
            }
            console.log("Table length:", this.#data.length);
            console.log("Start Code Point:", this.#data[0]);
            console.log("Last Code Point:", this.#data[this.#data.length - 1]);
        }
    }
    const table = new printTable();
    await parse(table);
    table.printAll();
}
