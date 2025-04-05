import {cleanComments, downloadFile, noop, splitLine, updateProgressBar} from "./utils.mjs";

export const fileUrl = "https://www.unicode.org/Public/UCD/latest/ucd/PropertyValueAliases.txt";
export const cacheFilePath = "PropertyValueAliases.txt";

let content = "";
export const download = async (callback = noop) => {
    if (content !== "") {
        callback(content);
        return content;
    }
    // database file
    return await downloadFile(fileUrl, cacheFilePath, async (fileContent) => {
        content = fileContent;
        await callback(fileContent);
    });
};

export const parse = async (alias = undefined, action = noop, fileContent = undefined) => {
    if (alias === undefined) {
        throw new Error("Invalid property provided.");
    }
    if (fileContent === undefined) {
        fileContent = await download();
        if (fileContent === undefined) {
            throw new Error(`We were unable to get the ${cacheFilePath} file.`);
        }
    }

    const lines = fileContent.split("\n");

    lines.forEach((line, index) => {
        line = cleanComments(line);

        // ignore empty lines
        if (line.length === 0) {
            return "";
        }

        const info = splitLine(line);

        updateProgressBar((index / lines.length) * 100);

        if (info[0] !== alias) {
            return;
        }

        let props = {};
        switch (info[0]) {
            case 'gc': // General Category
                props = {
                    short: info[1], long: info[2],
                };
                break;
            case 'bc': // Bidi Class
                props = {
                    short: info[1], long: info[2],
                };
                break;
            default:
                throw new Error("Not yet implemented...");
        }

        action({
            property: info[0], ...props
        });
    });
    updateProgressBar(100, `Lines parsed: ${lines.length}`);
};

export const getAliases = async (alias) => {
    let res = [];
    await parse(alias, (info) => res.push(info));
    return res;
}

export const getGeneralCategories = async () => getAliases('gc');
export const getBidiClasses = async () => getAliases('bc');

export const makeEnum = (vals) => {
    let obj = {};
    let index = 0;
    for (const {long, short} of vals) {
        obj[long] = index;
        obj[short] = index;
        obj[index] = `${long}`;
        ++index;
    }
    return obj;
}

export const renderEnum = (name, enumObj, type = "std::uint8_t") => {
    const keys = Object.keys(enumObj).filter((key) => isNaN(parseInt(key)));
    return `
        enum struct ${name} : ${type} {
            ${keys.map((key) => `${key} = ${enumObj[key]}U,`).join("\n")}
        };
    `;
}
