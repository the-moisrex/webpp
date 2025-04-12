import {
    cleanComments, downloadFile,
    findDate,
    findVersion,
    parseCodePointRangeExclusive,
    runClangFormat,
    splitLine, utf32To8All,
    writePieces
} from "./utils.mjs";

export const fileUrl = 'https://www.unicode.org/Public/idna/latest/IdnaMappingTable.txt';
export const cacheFilePath = 'IdnaMappingTable.txt';
export const outFilePath = `idna_mapping_tables.hpp`;

// UTF-16 version:
export const MAPPED = 0b000 << 13;
export const NOT_MAPPED = 0b100 << 13;
export const DISALLOWED = NOT_MAPPED | 0b010;
export const VALID = DISALLOWED | 0b1;

// UTF-8 version:
// const MAPPED = 0b0 << 7;
// const NOT_MAPPED = 0b1 << 7;
// const VALID = NOT_MAPPED | 0b011;
// const DISALLOWED = NOT_MAPPED | 0b010;

export const isMapped = (flags) => flags < NOT_MAPPED;
export const isValid = (flags) => flags === VALID;
export const isDisallowed = (flags) => flags === DISALLOWED;
export const isNotMapped = (flags) => !isMapped(flags);
export const flagsStatus = (flags) => {
    switch (flags) {
        case VALID:
            return "valid";
        case NOT_MAPPED:
            return "not_mapped";
        case DISALLOWED:
            return "disallowed";
        default:
            return isMapped(flags) ? `<Mapped:${flags}>` : `<invalid:${flags.toString(16)}>`;
    }
};
export const flagsOr = (flags, pos) => flags | (isMapped(flags) ? pos : 0);

export const parseMappedCodePoints = codePoints => codePoints.split(" ").map(codePoint => parseInt(codePoint, 16));


export const parseIDNAMappingTable = async (table, fileContent = undefined) => {
    if (fileContent === undefined) {
        fileContent = await downloadFile(fileUrl, cacheFilePath);
    }
    const lines = fileContent.split('\n');
    const version = findVersion(fileContent);
    const creationDate = findDate(fileContent);
    console.assert(version !== undefined, "Could not find the version from the file content.");
    console.assert(creationDate !== undefined, "No date was found.");
    console.log(`Version: ${version}`);
    console.log(`Creation Date: ${creationDate}`);

    let maxMappedCount = 0;
    let cpSum = 0n;
    lines.forEach((line, index) => {
        line = cleanComments(line)

        // ignore empty lines
        if (line.length === 0) {
            return "";
        }

        const [codePoints, status, mapping, IDNA2008Status] = splitLine(line);
        const [rangeStart, rangeEnd] = parseCodePointRangeExclusive(codePoints);
        let mappedValues = mapping ? parseMappedCodePoints(mapping) : [];

        let flags = 0;
        switch (status) {
            case 'deviation': // https://www.unicode.org/reports/tr46/#Deviations
            // Deviations are considered valid in IDNA2008 and UTS #46.
            case 'valid':
                flags |= VALID;
                break;
            case 'ignored':
                flags |= MAPPED;
                mappedValues = [];
                break;
            case 'mapped':
                flags |= MAPPED;
                break;
            case 'disallowed':
                flags |= DISALLOWED;
                break;
            default:
                console.error(`Invalid 'status' found: ${status}; line: ${line}`);
                process.exit(1);
        }

        // table.append(rangeStart, rangeEnd, flags, mappedValues);
        for (let pos = rangeStart; pos <= rangeEnd; ++pos) {
            const value = {
                codePoint: BigInt(pos),
                flags,
                mappedTo: mappedValues,
                utf8MappedTo: utf32To8All(mappedValues)
            };

            table.add(value.codePoint, value);
        }

        if (mappedValues?.length > maxMappedCount) {
            maxMappedCount = mappedValues.length;
        }
        cpSum += rangeEnd - rangeStart + 1n;

        // console.log(`${index}/${cpSum}:`, rangeStart, rangeEnd, status, mappedValues || "", IDNA2008Status || "");
    });


    console.log("Max Mapped Count: ", maxMappedCount);
}


