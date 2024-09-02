import {
    promises as fs
} from "fs";

const downloadFileNoCache = async (url, file, process) => {
    try {
        const response = await fetch(url);

        if (!response.ok) {
            console.error(
                `Failed to download file. Status Code: ${response.status}`,
            );
            return;
        }

        const text = await response.text();

        // Save the downloaded file as a cache
        await fs.writeFile(file, text);
        console.log(`Downloaded ${file} from ${url}.`);

        // process the file
        await process(text);
        return text;
    } catch (error) {
        console.error("Error:", error.message);
    }
};

const doneDownloading = async (content) => {
    console.log(`Size: ${content.length}.`);
};


downloadFileNoCache("https://www.unicode.org/Public/UCD/latest/ucd/NormalizationTest.txt", "NormalizationTest.txt", doneDownloading);
