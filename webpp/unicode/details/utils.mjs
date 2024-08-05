import {promises as fs} from 'fs';
import * as process from "node:process";
import * as assert from "node:assert";
import child_process from "node:child_process";


export const char1 = Symbol('char');
export const char2 = Symbol('char');
export const char3 = Symbol('char');
export const char4 = Symbol('char');
export const char5 = Symbol('char');
export const char6 = Symbol('char');
export const char7 = Symbol('char');
export const char8 = Symbol('char');
export const char8_1 = Symbol('char8_t');
export const char8_2 = Symbol('char8_t');
export const char8_3 = Symbol('char8_t');
export const char8_4 = Symbol('char8_t');
export const char8_5 = Symbol('char8_t');
export const char8_6 = Symbol('char8_t');
export const char8_7 = Symbol('char8_t');
export const char8_8 = Symbol('char8_t');
export const uint6 = Symbol('std::uint8_t');
export const uint7 = Symbol('std::uint8_t');
export const uint8 = Symbol('std::uint8_t');
export const uint16 = Symbol('std::uint16_t');
export const uint8x2 = Symbol('std::uint8_t');
export const uint32 = Symbol('std::uint32_t');
export const uint64 = Symbol('std::uint64_t');

export const realSizeOf = symbol => {
    switch (symbol) {
        case char8_1:
        case char8_2:
        case char8_3:
        case char8_4:
        case char8_5:
        case char8_6:
        case char8_7:
        case char8_8:
        case char1:
        case char2:
        case char3:
        case char4:
        case char5:
        case char6:
        case uint6:
        case char7:
        case char8:
        case uint7:
        case uint8:
            return 8;
        case uint8x2:
        case uint16:
            return 16;
        case uint32:
            return 32;
        case uint64:
            return 64;
    }
    debugger;
    throw new Error(`Invalid symbol: ${symbol} / ${symbol.description}`);
};

export const isStringType = symbol => {
    switch (symbol) {
        case char8_1:
        case char8_2:
        case char8_3:
        case char8_4:
        case char8_5:
        case char8_6:
        case char8_7:
        case char8_8:
        case char1:
        case char2:
        case char3:
        case char4:
        case char5:
        case char6:
        case char7:
        case char8:
            return true;
        default:
            return false;
    }
};

export const stringPrefixOf = symbol => {
    switch (symbol) {
        case char8_1:
        case char8_2:
        case char8_3:
        case char8_4:
        case char8_5:
        case char8_6:
        case char8_7:
        case char8_8:
            return "u8";
        default:
            return "";
    }
};

function toHexString(char) {
    // random decision, I know both are okay
    if (char <= 0o77) {
        return `\\${char.toString(8)}`;
    } else if (char <= 0xFF) {
        return '\\x' + char.toString(16);
    } else {
        return '\\u' + char.toString(16).padStart(4, '0');
    }
}

export const cppValueOf = (value, symbol) => {
    if (typeof value !== 'number') {
        throw new Error("Invalid value type.");
    }
    switch (symbol) {
        case char8_1:
        case char8_2:
        case char8_3:
        case char8_4:
        case char8_5:
        case char8_6:
        case char8_7:
        case char8_8:
        case char1:
        case char2:
        case char3:
        case char4:
        case char5:
        case char6:
        case uint6:
        case char7:
        case char8:
            return toHexString(value);
        case uint7:
        case uint8:
        case uint8x2:
        case uint16:
        case uint32:
        case uint64:
            return value;
    }
    debugger;
    throw new Error(`Invalid symbol: ${symbol} / ${symbol.description}`);
};

export const sizeOf = symbol => {
    switch (symbol) {
        case char8_1:
        case char1:
            return 1;
        case char8_2:
        case char2:
            return 2;
        case char8_3:
        case char3:
            return 3;
        case char8_4:
        case char4:
            return 4;
        case char8_5:
        case char5:
            return 5;
        case char8_6:
        case char6:
        case uint6:
            return 6;
        case char8_7:
        case char7:
        case uint7:
            return 7;
        case char8_8:
        case char8:
        case uint8:
            return 8;
        case uint8x2:
        case uint16:
            return 16;
        case uint32:
            return 32;
        case uint64:
            return 64;
    }
    debugger;
    throw new Error(`Invalid symbol: ${symbol} / ${symbol.description}`);
};

export const symbolOf = size => {
    if (size <= 1) {
        size = 1;
    } else if (size <= 2) {
        size = 2;
    } else if (size <= 3) {
        size = 3;
    } else if (size <= 4) {
        size = 4;
    } else if (size <= 5) {
        size = 5;
    } else if (size <= 6) {
        size = 6;
    } else if (size <= 7) {
        size = 7;
    } else if (size <= 8) {
        size = 8;
    } else if (size <= 16) {
        size = 16;
    } else if (size <= 32) {
        size = 32;
    } else if (size <= 64) {
        size = 64;
    }
    switch (size) {
        case 1:
            return char1;
        case 2:
            return char2;
        case 3:
            return char3;
        case 4:
            return char4;
        case 5:
            return char5;
        case 6:
            return uint6;
        case 7:
            return uint7;
        case 8:
            return uint8;
        case 16:
            return uint16;
        case 32:
            return uint32;
        case 64:
            return uint64;
    }
    assert.ok(size <= 64, `Invalid size: ${size}`);
};

/// check if the symbol is 8, 16, 32, or 64
export const alignedSymbol = symbol => {
    switch (symbol) {
        case char8_8:
        case char8:
        case uint8:
        case uint16:
        case uint32:
        case uint64:
            return true;
        default:
            return false;
    }
};

export const maxOf = value => {
    if (typeof value === "symbol") {
        value = sizeOf(value);
    }
    return (0b1 << value) - 1;
};

export const bitOnesOf = value => {
    let max = 0;
    for (; value !== 0; --value) {
        max <<= 1;
        max |= 0b1;
    }
    return max;
};

export const noop = _ => {
};

const progressBarLength = 30; // Define the length of the progress bar
const totalItems = 100;       // Total number of items to process
let lastPercent = 0;
export const updateProgressBar = (percent, done = undefined) => {
    if (!process.stdout.isTTY) {
        return;
    }
    // process.stdout.clearLine();
    process.stdout.cursorTo(0); // Move the cursor to the beginning of the line
    if (percent >= totalItems) {
        process.stdout.clearLine();
        if (done) {
            console.log(done instanceof Function ? done() : done);
        }
        return;
    }
    if (Math.round(percent) === lastPercent) {
        return;
    }
    lastPercent = Math.round(percent);
    const progress = Math.round((percent / totalItems) * progressBarLength);
    const progressBar = '='.repeat(progress) + '>' +
        '-'.repeat(progressBarLength - progress);
    process.stdout.write(
        `[${progressBar}] ${Math.round((percent / totalItems) * 100)}%`); // Update the progress bar
};


export const downloadFile = async (url, file, process) => {
    try {
        // Check if the file already exists in the cache
        await fs.access(file);
        try {
            console.log(`Using cached file ${file}...`);
            const fileSource = await fs.readFile(file);
            const fileContent = fileSource.toString();
            await process(fileContent);
            return fileContent;
        } catch (error) {
            console.error(error);
            return;
        }
    } catch (error) {
        console.log("No cached file exists, let's download it.");
    }

    try {
        const response = await fetch(url);

        if (!response.ok) {
            console.error(`Failed to download file. Status Code: ${response.status}`);
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
        console.error('Error:', error.message);
    }
};

export const cleanComments = line => line.split('#')[0].trimEnd();
export const splitLine = line => line.split(';').map(seg => seg.trim());
export const findVersion = fileContent => fileContent.match(/Version:? (\d+\.\d+\.\d+)/)[1];
export const findDate = fileContent => fileContent.match(/Date: ([^\n\r]+)/)[1];
export const parseCodePoints = codePoint => parseInt(codePoint, 16);
export const parseCodePointRange = (codePoints, lastCodePoint = 0) => {
    let rangeArr = codePoints.split("..").map(codePoint => codePoint.trim());
    if (rangeArr.length === 1) {
        rangeArr = [`${lastCodePoint.toString(16)}`, rangeArr[0]];
    }
    return rangeArr.map(codePoint => parseInt(codePoint, 16));
};

export class Span {
    #arr;
    #start;
    #end;
    #func;

    constructor(arr = [], start = 0, length = arr.length - start, func = item => item) {
        this.#arr = arr;
        this.#start = start;
        this.#end = start + length;
        this.#func = func;
        if (this.#end === undefined || isNaN(this.#end)) {
            debugger;
            throw new Error(`Unexpected end: ${this.#end}`);
        }
    }

    get length() {
        return this.#end - this.#start;
    }

    slice(start = 0, end = this.length - start) {
        const newStart = this.#start + start;
        end = Math.min(this.length, end);
        const newLength = this.#start + end - newStart;
        return new Span(this.#arr, newStart, newLength, this.#func);
    }

    // expand(newStart = 0, newLength = this.length + newStart) {
    //     if (newStart < 0 && newStart >= this.length) {
    //         throw new Error(`Index out of bounds ${index} out of ${this.length} elements.`);
    //     }
    //     newLength = Math.min(this.#arr.length, newLength);
    //     return new Span(this.#arr, newStart, newLength, this.#func);
    // }

    filter(func) {
        let values = [];
        for (const val of this) {
            values.push(func(val));
        }
        return values;
    }

    map(func) {
        return new Span(this.#arr, this.#start, this.length, func);
    }

    at(index) {
        assert.ok(index >= 0 && index < this.length, `Index out of bounds ${index} out of ${this.length} elements.`);
        return this.#func(this.#arr[this.#start + index]);
    }

    * [Symbol.iterator]() {
        for (let i = this.#start; i < this.#end; i++) {
            yield this.#func(this.#arr[i]);
        }
    }
}

/**
 * This class will let us handle the types of the tables including:
 *   - unsigned integer 32 bit (uint32)
 *   - unsigned integer 8 bit  (uint8)
 *   - unsigned integer 16 bit (uint16)
 */
export class TableTraits {
    #type = uint8;
    index = 0;

    constructor(max, type = uint8) {
        switch (type) {
            case char8_1:
            case char8_2:
            case char8_3:
            case char8_4:
            case char8_5:
            case char8_6:
            case char8_7:
            case char8_8:
            case char1:
            case char2:
            case char3:
            case char4:
            case char5:
            case char6:
            case char7:
            case char8:
            case uint6:
            case uint7:
            case uint8:
                this.bytes = new Uint8Array(max);
                break;
            case uint32:
                this.bytes = new Uint32Array(max);
                break;
            case uint8x2:
                this.bytes = [];
                break;
            default:
                throw new Error('Invalid type provided to CodePointMapper.');
        }
        this.#type = type;
    }

    get type() {
        return this.#type;
    }

    get sizeof() {
        return sizeOf(this.type);
    }

    get typeString() {
        return this.type.description;
    }

    get STLTypeString() {
        return this.typeString;
    }

    get postfix() {
        switch (this.type) {
            case char1:
            case char2:
            case char3:
            case char4:
            case char5:
            case char6:
            case char7:
            case char8:
                return "";
            case uint6:
            case uint7:
            case uint8x2:
            case uint16:
            case uint8:
                return "U";
            case uint32:
                return "ULL";
            default:
                return "";
        }
    }

    get length() {
        return this.index;
    }

    get result() {
        return this.bytes.slice(0, this.length);
    }

    trimAt(index) {
        this.index = index;
    }

    at(index) {
        if (index >= this.length) {
            throw new RangeError(`Index out of bounds ${index} out of ${this.length} elements.`);
        }
        return this.bytes.at(index);
    }

    * [Symbol.iterator]() {
        for (let pos = 0; pos !== this.length; pos++) {
            yield this.at(pos);
        }
    }

    set(index, value) {
        return this.bytes[index] = value;
    }

    append(value) {
        this.bytes[this.index++] = value;
    }

    appendList(list) {
        for (const value of list) {
            this.append(value);
        }
    }
}

/// Find the start position of "left" in "right"
/// This function finds a place in the "right" table where the specified range
/// will be there.
export const findSimilarRange = (left, right) => {
    assert.ok(Number.isSafeInteger(left.length), "Table should have a valid length");
    assert.ok(Number.isSafeInteger(right.length), "Table should have a valid length");
    // if (left.length > right.length) {
    //     return null;
    // }
    try {
        top: for (let rpos = 0; rpos !== right.length; ++rpos) {
            for (let lpos = 0; lpos !== left.length; ++lpos) {
                const rvalue = right.at(rpos + lpos);
                const lvalue = left.at(lpos);
                if (rvalue !== lvalue) {
                    continue top;
                }
            }
            return rpos;
        }
    } catch (err) {
        if (!(err instanceof RangeError)) {
            throw err;
        }
        // else, just say we found nothing
    }
    return null;
};

// Overlap Inserts Optimization:
///    if the "right" table's tail has a match for the beginning of the "left" table,
///    then we can omit inserting the first part of the "left" table.
export const overlapInserts = (left, right) => {
    if (left.length === 0) {
        return 0;
    }
    let rpos = Math.max(0, right.length - left.length);
    top: for (; rpos !== right.length; ++rpos) {
        const length = right.length - rpos;
        for (let lpos = 0; lpos !== length; ++lpos) {
            const lvalue = left.at(lpos);
            const rvalue = right.at(rpos + lpos);
            if (lvalue !== rvalue) {
                continue top;
            }
        }
        return length;
    }
    return 0;
};

export const popcount = n => {
    let c = 0;
    for (; n !== 0; n >>= 1) {
        if ((n & 1) !== 0) {
            c++;
        }
    }
    return c;
};

/// Here's a JavaScript function that takes an integer and returns an integer with all the bits from
/// the right filled with 1 up to the input integer:
export const fillBitsFromRight = (num) => {
    // Shift 1 left by the number of bits in the input integer
    let mask = 1 << num;

    // Subtract 1 from the mask to fill the bits
    return mask - 1;
};

export const bitFloor = (num) => {
    if (num === 0) {
        return -1; // No set bits
    }

    let res = 0b1;
    while ((res & num) === 0) {
        res <<= 1;
    }
    return res;
};

/// Not the same as std::bit_ceil in C++20
export const bitCeil = (x) => {
    if (x === 0) {
        return 0;
    }
    let p = 0b1;
    while (p <= x) {
        p <<= 1;
    }
    return p;
};


export const writePieces = async (outFile, pieces) => {

    await fs.writeFile(outFile, "");
    for (const piece of pieces) {
        if (typeof piece !== "string") {
            throw new Error(`Invalid input, piece: ${piece}`);
        }
        await fs.appendFile(outFile, piece);
    }
};

export const runClangFormat = async filePath => {
    // Reformat the file
    try {
        await child_process.exec(`clang-format -i "${filePath}"`);
        console.log("Clang-format completed.");
    } catch (err) {
        console.error("Could not re-format the file.", err);
    }
};

const encoder = new TextEncoder();
export const utf32To8 = (codePoint) => {
    return encoder.encode(String.fromCodePoint(codePoint));
};

export const utf32To8All = (u32Array) => {
    let arr = [];
    for (const codePoint of u32Array) {
        for (const unit of utf32To8(codePoint)) {
            arr.push(unit);
        }
    }
    return arr;
};

export const renderTableValues = ({name, printableValues, type, len}) => {

    let valuesTable = "";
    if (isStringType(type)) {
        const prefix = stringPrefixOf(type);
        valuesTable = `static constexpr std::basic_string_view<${type.description}> ${name.toLowerCase()}_values {
        ${printableValues.map(val => {
            let res = "";
            if (val.comment) {
                res += `
        // ${val.comment}
        `
            }
            res += `${prefix}"${val.join("")}"`;
            return res;
        }).join("\n")},
        // done.
        ${len}UL // String Length
    };
            `
    } else {
        valuesTable = `static constexpr std::array<${type.description}, ${len}ULL> ${name.toLowerCase()}_values{
        ${printableValues.map(val => {
            let res = "";
            if (val.comment) {
                res += `
        // ${val.comment}
        `
            }
            res += val.join(", ");
            return res;
        }).join(", \n")}
    };
            `;
    }

    return valuesTable;
};

export const findTopLongestZeroRanges = (arr, topN = 5) => {
    let ranges = [];
    let currentLength = 0;
    let startIndex = -1;

    for (let i = 0; i <= arr.length; i++) {
        // Check if the current element is zero
        if (arr[i] === 0) {
            if (currentLength === 0) {
                // Start a new range
                startIndex = i;
            }
            currentLength++;
        } else {
            // If we hit a non-zero, check if we have a valid range
            if (currentLength > 0) {
                ranges.push({start: startIndex, length: currentLength});
                currentLength = 0; // Reset for the next range
            }
        }
    }

    // Sort ranges by length in descending order
    ranges.sort((a, b) => b.length - a.length);

    // Get the top 5 longest ranges
    return ranges.slice(0, topN);
};


// export const interleaveBits = (x, y) => {
//     const B = [0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF];
//     const S = [1, 2, 4, 8];
//
//     // x and y must initially be less than 65536.
//     assert.ok(x < 65536, `x == ${x}`);
//     assert.ok(y < 65536, `y == ${y}`);
//
//     x = (x | (x << S[3])) & B[3];
//     x = (x | (x << S[2])) & B[2];
//     x = (x | (x << S[1])) & B[1];
//     x = (x | (x << S[0])) & B[0];
//
//     y = (y | (y << S[3])) & B[3];
//     y = (y | (y << S[2])) & B[2];
//     y = (y | (y << S[1])) & B[1];
//     y = (y | (y << S[0])) & B[0];
//
//     return x | (y << 1);
// };

const MortonTable256 = [
    0x0000, 0x0001, 0x0004, 0x0005, 0x0010, 0x0011, 0x0014, 0x0015,
    0x0040, 0x0041, 0x0044, 0x0045, 0x0050, 0x0051, 0x0054, 0x0055,
    0x0100, 0x0101, 0x0104, 0x0105, 0x0110, 0x0111, 0x0114, 0x0115,
    0x0140, 0x0141, 0x0144, 0x0145, 0x0150, 0x0151, 0x0154, 0x0155,
    0x0400, 0x0401, 0x0404, 0x0405, 0x0410, 0x0411, 0x0414, 0x0415,
    0x0440, 0x0441, 0x0444, 0x0445, 0x0450, 0x0451, 0x0454, 0x0455,
    0x0500, 0x0501, 0x0504, 0x0505, 0x0510, 0x0511, 0x0514, 0x0515,
    0x0540, 0x0541, 0x0544, 0x0545, 0x0550, 0x0551, 0x0554, 0x0555,
    0x1000, 0x1001, 0x1004, 0x1005, 0x1010, 0x1011, 0x1014, 0x1015,
    0x1040, 0x1041, 0x1044, 0x1045, 0x1050, 0x1051, 0x1054, 0x1055,
    0x1100, 0x1101, 0x1104, 0x1105, 0x1110, 0x1111, 0x1114, 0x1115,
    0x1140, 0x1141, 0x1144, 0x1145, 0x1150, 0x1151, 0x1154, 0x1155,
    0x1400, 0x1401, 0x1404, 0x1405, 0x1410, 0x1411, 0x1414, 0x1415,
    0x1440, 0x1441, 0x1444, 0x1445, 0x1450, 0x1451, 0x1454, 0x1455,
    0x1500, 0x1501, 0x1504, 0x1505, 0x1510, 0x1511, 0x1514, 0x1515,
    0x1540, 0x1541, 0x1544, 0x1545, 0x1550, 0x1551, 0x1554, 0x1555,
    0x4000, 0x4001, 0x4004, 0x4005, 0x4010, 0x4011, 0x4014, 0x4015,
    0x4040, 0x4041, 0x4044, 0x4045, 0x4050, 0x4051, 0x4054, 0x4055,
    0x4100, 0x4101, 0x4104, 0x4105, 0x4110, 0x4111, 0x4114, 0x4115,
    0x4140, 0x4141, 0x4144, 0x4145, 0x4150, 0x4151, 0x4154, 0x4155,
    0x4400, 0x4401, 0x4404, 0x4405, 0x4410, 0x4411, 0x4414, 0x4415,
    0x4440, 0x4441, 0x4444, 0x4445, 0x4450, 0x4451, 0x4454, 0x4455,
    0x4500, 0x4501, 0x4504, 0x4505, 0x4510, 0x4511, 0x4514, 0x4515,
    0x4540, 0x4541, 0x4544, 0x4545, 0x4550, 0x4551, 0x4554, 0x4555,
    0x5000, 0x5001, 0x5004, 0x5005, 0x5010, 0x5011, 0x5014, 0x5015,
    0x5040, 0x5041, 0x5044, 0x5045, 0x5050, 0x5051, 0x5054, 0x5055,
    0x5100, 0x5101, 0x5104, 0x5105, 0x5110, 0x5111, 0x5114, 0x5115,
    0x5140, 0x5141, 0x5144, 0x5145, 0x5150, 0x5151, 0x5154, 0x5155,
    0x5400, 0x5401, 0x5404, 0x5405, 0x5410, 0x5411, 0x5414, 0x5415,
    0x5440, 0x5441, 0x5444, 0x5445, 0x5450, 0x5451, 0x5454, 0x5455,
    0x5500, 0x5501, 0x5504, 0x5505, 0x5510, 0x5511, 0x5514, 0x5515,
    0x5540, 0x5541, 0x5544, 0x5545, 0x5550, 0x5551, 0x5554, 0x5555
];
export const interleaveBits = (x, y) => {
    // x and y must initially be less than 65536.
    assert.ok(x < 65536, `x == ${x}`);
    assert.ok(y < 65536, `y == ${y}`);
    return MortonTable256[y >> 8] << 17 |
        MortonTable256[x >> 8] << 16 |
        MortonTable256[y & 0xFF] << 1 |
        MortonTable256[x & 0xFF];
};


export function* findSmallest(arr, op, maxMask = arr.toSorted((a, b) => b - a)[0]){
    let mask = 0;

    top: for (; mask <= maxMask; ++mask) {
        const maskedValues = [];
        for (const val of arr) {
            const masked = op(val, mask);
            if (maskedValues.includes(masked)) {
                continue top;
            }
            maskedValues.push(masked);
        }
        yield mask;
    }
}

export function maskFinder (arr) {
    return findSmallest(arr, (val, mask) => val & mask);
}

export const findSmallestMask = (arr) => {
    return maskFinder(arr).next().value;
};

export const findSmallestDivision = (arr) => {
    return findSmallest(arr, (val, div) => Math.floor(val / div)).next().value;
};

export const findSmallestXor = (arr) => {
    return findSmallest(arr, (val, mask) => val ^ mask).next().value;
};

export const findSmallestComplement = (arr) => {
    return arr.toSorted((a, b) => a - b)[0];
};

export const hasDuplicates = (arr) => {
    // Get the length of the array
    const length = arr.length;

    // Use a nested loop to compare each element with every other element
    for (let i = 0; i < length; i++) {
        for (let j = i + 1; j < length; j++) {
            // If a duplicate is found, return true
            if (arr[i] === arr[j]) {
                return true; // Duplicate found
            }
        }
    }

    // If no duplicates are found, return false
    return false;
};
