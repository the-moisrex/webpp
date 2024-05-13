import {downloadFile, findDate, findVersion, noop} from "./utils.mjs";

export const fileUrl = 'https://www.unicode.org/Public/UCD/latest/ucd/ReadMe.txt';
export const cachedFilePath = 'ReadMe.txt';

const readmeData = {
    version: undefined,
    date: undefined
};

const processReadmeFile = content => {
    readmeData.version = findVersion(content);
    readmeData.date = findDate(content);
};


export const download = async (callback = noop, print = true) => {
    // readme file for getting the version and what not
    await downloadFile(fileUrl, cachedFilePath, processReadmeFile);
    if (readmeData.version === undefined) {
        const msg = "Could not find the version from the file content.";
        if (print) {
            console.error(msg);
        } else {
            throw new Error(msg);
        }
        return;
    }
    if (readmeData.date === undefined) {
        const msg = "No date was found.";
        if (print) {
            console.error(msg);
        } else {
            throw new Error("msg");
        }
        return;
    }

    if (print) {
        console.log(`Version:       ${readmeData.version}`);
        console.log(`Creation Date: ${readmeData.date}`);
    }

    callback(readmeData);
};

export const getReadme = async () => {
    if (readmeData.version === undefined) {
        await download(noop, false);
    }
    return readmeData;
}