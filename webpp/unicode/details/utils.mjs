import {promises as fs} from "fs";
import * as assert from "node:assert";
import child_process from "node:child_process";
import * as process from "node:process";

export const bool1 = Symbol("bool");
export const bool32 = Symbol("std::uint32_t");
export const char1 = Symbol("char");
export const char2 = Symbol("char");
export const char3 = Symbol("char");
export const char4 = Symbol("char");
export const char5 = Symbol("char");
export const char6 = Symbol("char");
export const char7 = Symbol("char");
export const char8 = Symbol("char");
export const char8_1 = Symbol("char8_t");
export const char8_2 = Symbol("char8_t");
export const char8_3 = Symbol("char8_t");
export const char8_4 = Symbol("char8_t");
export const char8_5 = Symbol("char8_t");
export const char8_6 = Symbol("char8_t");
export const char8_7 = Symbol("char8_t");
export const char8_8 = Symbol("char8_t");
export const uint3 = Symbol("std::uint8_t");
export const uint4 = Symbol("std::uint8_t");
export const uint5 = Symbol("std::uint8_t");
export const uint6 = Symbol("std::uint8_t");
export const uint7 = Symbol("std::uint8_t");
export const uint8 = Symbol("std::uint8_t");
export const uint9 = Symbol("std::uint16_t");
export const uint10 = Symbol("std::uint16_t");
export const uint11 = Symbol("std::uint16_t");
export const uint12 = Symbol("std::uint16_t");
export const uint16 = Symbol("std::uint16_t");
export const uint8x2 = Symbol("std::uint8_t");
export const uint32 = Symbol("std::uint32_t");
export const uint64 = Symbol("std::uint64_t");

export const realSizeOf = (symbol) => {
    switch (symbol) {
        case bool32:
            return 1n;
        case bool1:
            return 1n;
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
        case uint3:
        case uint4:
        case uint5:
        case uint6:
        case char7:
        case char8:
        case uint7:
        case uint8:
            return 8n;
        case uint9:
        case uint10:
        case uint11:
        case uint12:
        case uint16:
            return 16n;
        case uint32:
            return 32n;
        case uint64:
            return 64n;
    }
    debugger;
    throw new Error(`Invalid symbol: ${symbol} / ${symbol.description}`);
};

export const isBoolType = (symbol) => {
    switch (symbol) {
        case bool32:
        case bool1:
            return true;
        default:
            return false;
    }
}

export const isStringType = (symbol) => {
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
        case bool1:
        case bool32:
        default:
            return false;
    }
};

export const stringPrefixOf = (symbol) => {
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

export function isIterable(input) {
    return typeof input?.[Symbol.iterator] === 'function';
}

export function recursiveLength(arr, length = arr.length) {
    if (!isIterable(arr)) {
        return arr?.length || 1;
    }

    // return arr.reduce((sum, item) => sum + recursiveLength(item), 0);
    let sum = 0;
    for (let index = 0; index !== length; ++index) {
        sum += recursiveLength(arr[index]);
    }
    return sum
}

export function toHexString(char) {
    // random decision, I know both are okay
    if (char <= 0o77) {
        return `\\${char.toString(8)}`;
    } else if (char <= 0xff) {
        return "\\x" + char.toString(16);
    } else {
        return "\\u" + char.toString(16).padStart(4, "0");
    }
}

export const cppValueOf = (value, symbol) => {
    if (typeof value !== "number") {
        throw new Error("Invalid value type.");
    }
    switch (symbol) {
        case bool32:
            return value ? "1" : "0";
        case bool1:
            return value ? "true" : "false";
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
        case uint3:
        case uint4:
        case uint5:
        case uint6:
        case char7:
        case char8:
            return toHexString(value);
        case uint7:
        case uint8:
        case uint9:
        case uint10:
        case uint11:
        case uint12:
        case uint8x2:
        case uint16:
        case uint32:
        case uint64:
            return value;
    }
    debugger;
    throw new Error(`Invalid symbol: ${symbol} / ${symbol.description}`);
};

export const sizeOf = (symbol) => {
    switch (symbol) {
        case bool32:
            return 32n;
        case char8_1:
        case bool1:
        case char1:
            return 1n;
        case char8_2:
        case char2:
            return 2n;
        case char8_3:
        case char3:
        case uint3:
            return 3n;
        case char8_4:
        case char4:
        case uint4:
            return 4n;
        case char8_5:
        case char5:
        case uint5:
            return 5n;
        case char8_6:
        case char6:
        case uint6:
            return 6n;
        case char8_7:
        case char7:
        case uint7:
            return 7n;
        case char8_8:
        case char8:
        case uint8:
            return 8n;
        case uint9:
            return 9n;
        case uint10:
            return 10n;
        case uint11:
            return 11n;
        case uint12:
            return 12n;
        case uint8x2:
        case uint16:
            return 16n;
        case uint32:
            return 32n;
        case uint64:
            return 64n;
    }
    debugger;
    throw new Error(`Invalid symbol: ${symbol} / ${symbol.description}`);
};

export const symbolOf = (size, symbolType = uint8.description) => {
    size = Number(size);
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
    } else if (size <= 9) {
        size = 9;
    } else if (size <= 10) {
        size = 10;
    } else if (size <= 11) {
        size = 11;
    } else if (size <= 12) {
        size = 12;
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
            return symbolType === uint3.description ? uint3 : char3;
        case 4:
            return symbolType === uint4.description ? uint4 : char4;
        case 5:
            return symbolType === uint5.description ? uint5 : char5;
        case 6:
            return uint6;
        case 7:
            return uint7;
        case 8:
            return uint8;
        case 9:
            return uint9;
        case 10:
            return uint10;
        case 11:
            return uint11;
        case 12:
            return uint12;
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
export const alignedSymbol = (symbol) => {
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

export const maxOf = (value) => {
    if (typeof value === "symbol") {
        value = sizeOf(value);
    }
    return (0b1n << value) - 1n;
};

export const bitOnesOf = (value) => {
    let max = 0n;
    for (; value > 0n; --value) {
        max <<= 1n;
        max |= 0b1n;
    }
    return max;
};

export const noop = (_) => {
};

const progressBarLength = 30; // Define the length of the progress bar
const totalItems = 100; // Total number of items to process
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
    const progressBar = "=".repeat(progress) + ">" + "-".repeat(progressBarLength - progress);
    process.stdout.write(`[${progressBar}] ${Math.round((percent / totalItems) * 100)}%`,); // Update the progress bar
};

export const downloadFile = async (url, file, process = noop) => {
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
            console.error(`Failed to download file. Status Code: ${response.status}`,);
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

export const cleanComments = (line) => line.split("#")[0].trimEnd();
export const splitLine = (line) => line.split(";").map((seg) => seg.trim());
export const findVersion = (fileContent) => fileContent.match(/Version:? (\d+\.\d+\.\d+)/)[1];
export const findDate = (fileContent) => fileContent.match(/Date: ([^\n\r]+)/)[1];
export const parseCodePoints = (codePoint) => BigInt(parseInt(codePoint, 16));
export const parseCodePointRange = (codePoints, lastCodePoint = 0) => {
    let rangeArr = codePoints.split("..").map((codePoint) => codePoint.trim());
    if (rangeArr.length === 1) {
        rangeArr = [`${lastCodePoint.toString(16)}`, rangeArr[0]];
    }
    return rangeArr.map((codePoint) => BigInt(parseInt(codePoint, 16)));
};

export const parseCodePointRangeExclusive = (codePoints) => {
    let rangeArr = codePoints.split("..").map((codePoint) => codePoint.trim());
    if (rangeArr.length === 1) {
        rangeArr = [rangeArr[0], rangeArr[0]];
    }
    return rangeArr.map((codePoint) => BigInt(parseInt(codePoint, 16)));
};

export class Span {
    #arr;
    #start;
    #end;
    #func;

    constructor(arr = [], start = 0n, length = BigInt(arr.length) - BigInt(start), func = (item) => item) {
        this.#arr = arr;
        this.#start = BigInt(start);
        this.#end = this.#start + BigInt(length);
        this.#func = func;
        if (this.#end === undefined || (typeof this.#end == "number" && isNaN(this.#end))) {
            debugger;
            throw new Error(`Unexpected end: ${this.#end}`);
        }
    }

    get length() {
        return Number(this.#end - this.#start);
    }

    slice(start = 0n, end = BigInt(this.length) - BigInt(start)) {
        const newStart = this.#start + BigInt(start);
        end = BigInt(Math.min(this.length, Number(end)));
        const newLength = this.#start + end - newStart;
        return new Span(this.#arr, newStart, newLength, this.#func);
    }

    getAll() {
        let res = [];
        for (let i = this.#start; i < this.#end; ++i) {
            const val = this.#arr.at(Number(i));
            res.push(this.#func(val));
        }
        return res;
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
        index = Number(index);
        if (!(index >= 0 && index < this.length)) {
            debugger;
            throw new Error(`Index out of bounds ${index} out of ${this.length} elements.`,);
        }
        return this.#func(this.#arr[Number(this.#start) + index]);
    }

    * [Symbol.iterator]() {
        for (let i = this.#start; i < this.#end; i++) {
            yield this.#func(this.#arr[i]);
        }
    }
}

export function newArray(type, max) {
    switch (type) {
        case char8_1:
        case char8_2:
        case char8_3:
        case char8_4:
        case char8_5:
        case char8_6:
        case char8_7:
        case char8_8:
        case bool1:
        case char1:
        case char2:
        case char3:
        case char4:
        case char5:
        case char6:
        case char7:
        case char8:
        case uint3:
        case uint4:
        case uint5:
        case uint6:
        case uint7:
        case uint8:
            return new Uint8Array(max);
        case uint16:
            return new Uint16Array(max);
        case bool32:
        case uint32:
            return new Uint32Array(max);
        case uint8x2:
        case null:
            return [];
        default:
            throw new Error("Invalid type provided to CodePointMapper.");
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
        this.bytes = newArray(type, max);
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
            case bool32:
            case bool1:
            case char1:
            case char2:
            case char3:
            case char4:
            case char5:
            case char6:
            case char7:
            case char8:
                return "";
            case uint3:
            case uint4:
            case uint5:
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
        return Number(this.index);
    }

    get result() {
        return this.bytes.slice(0, Number(this.length));
    }

    trimAt(index) {
        this.index = BigInt(index);
    }

    at(index) {
        index = Number(index);
        if (index >= this.length) {
            throw new RangeError(`Index out of bounds ${index} out of ${this.length} elements.`,);
        } else if (index < 0) {
            index = this.length - index;
        }
        return this.bytes.at(index);
    }

    atOr(index, defaultValue = 0) {
        index = Number(index);
        if (index >= this.length) {
            return defaultValue;
        }
        return this.bytes.at(index);
    }

    * [Symbol.iterator]() {
        for (let pos = 0; pos !== this.length; pos++) {
            yield this.at(pos);
        }
    }

    set(index, value) {
        if (index > this.index) {
            this.index = index; // change the length
        }
        return (this.bytes[index] = value);
    }

    setAt(index, values = []) {
        const endIndex = index + values.length;
        if (endIndex > this.index) {
            this.index = endIndex; // change the length
        }

        for (let value of values) {
            this.bytes[index++] = value;
        }
    }

    setOrFill(index, value, fillValue = 0) {
        if (index >= this.index) {
            for (; this.index <= index; ++this.index) {
                this.set(this.index, fillValue);
            }
        }
        this.set(index, value);
    }

    isAll(index, length, value) {
        const end = index + length;
        if (end > this.index) {
            return true;
        }
        for (; index !== end; ++index) {
            if (this.bytes[index] !== value) {
                return false;
            }
        }
        return true;
    }

    append(value) {
        if (this.length >= this.bytes.length) {
            throw new Error(`Maximum limit has reached; max length: ${this.bytes.length}`);
        }
        this.bytes[this.index++] = Number(value);
    }

    push(value) {
        this.append(value);
    }

    add(value) {
        this.append(value);
    }

    appendList(list) {
        for (const value of list) {
            this.append(value);
        }
    }

    setLength(len) {
        this.index = len + 1;
    }

    clear(value = 0) {
        this.bytes.fill(value);
        this.index = 0;
    }

    findOrAdd(values) {
        let pos = findSimilarRange(values, this);
        if (pos !== null) {
            return pos;
        }
        pos = this.index;
        this.appendList(values);
        return pos;
    }
}

// left  =       [0xFF, 0xFF, 0xFF, ...]
// right = [0, 0, 0xFF, 0xFF, 0xFF, ...]
export const findSimilarSubBlocks = (left, right, blockSize = uint8) => {
    assert.ok(Number.isSafeInteger(left.length), "Table should have a valid length",);
    assert.ok(Number.isSafeInteger(right.length), "Table should have a valid length",);
    try {
        const blockLen = Number(sizeOf(blockSize));
        top: for (let rpos = 0; rpos !== right.length; rpos += blockLen) {
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


// left  =  [1, 2, 3, ...]
// right = [[1, 2, 3, ...], [...], [...]]
export const findSimilarSubRange = (left, right) => {
    assert.ok(Number.isSafeInteger(left.length), "Table should have a valid length",);
    assert.ok(Number.isSafeInteger(right.length), "Table should have a valid length",);
    top: for (let rpos = 0; rpos !== right.length; ++rpos) {
        if (left.length !== right[rpos].length) {
            continue;
        }
        for (let lpos = 0; lpos !== left.length; ++lpos) {
            const rvalue = right[rpos][lpos];
            const lvalue = left[lpos];
            if (rvalue !== lvalue) {
                continue top;
            }
        }
        return rpos;
    }
    return null;
};


/// Find the start position of "left" in "right"
/// This function finds a place in the "right" table where the specified range
/// will be there.
export const findSimilarRange = (left, right) => {
    assert.ok(Number.isSafeInteger(left.length), "Table should have a valid length",);
    assert.ok(Number.isSafeInteger(right.length), "Table should have a valid length",);
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
///    if the "left" table's tail has a match for the beginning of the "right" table,
///    then we can omit inserting the first part of the "right" table.
export const overlapInserts = (lhs, rhs) => {
    if (rhs.length === 0) {
        return 0;
    }
    let rpos = Math.max(0, lhs.length - rhs.length);
    top: for (; rpos !== lhs.length; ++rpos) {
        const length = lhs.length - rpos;
        for (let lpos = 0; lpos !== length; ++lpos) {
            const lvalue = rhs.at(lpos);
            const rvalue = lhs.at(rpos + lpos);
            if (lvalue !== rvalue) {
                continue top;
            }
        }
        return length;
    }
    return 0;
};

/// We remove both of them, but of course you need to keep one of them:
export const removeOverlaps = (lhs, rhs) => {
    const overlapLen = overlapInserts(lhs, rhs);
    if (overlapLen !== 0) {
        lhs = lhs.slice(0, lhs.length - overlapLen);
        rhs = rhs.slice(overlapLen, rhs.length);
    }
    return {lhs, rhs, overlapLen};
}

export const smashOverlappedBlocks = (blocks, onSmash = noop) => {
    let smashedBlocks = [];
    for (let i = 0; i < blocks.length - 1; ++i) {
        const cur = blocks.at(i);
        const nxt = blocks.at(i + 1);

        const {lhs, rhs, overlapLen} = removeOverlaps(cur, nxt);
        onSmash(overlapLen, i, i + 1); // call the callback
        smashedBlocks.push(lhs, rhs);
    }
    return smashedBlocks;
}

export const popcount = (n) => {
    let c = 0n;
    for (; n !== 0n; n >>= 1n) {
        if ((n & 1n) !== 0n) {
            ++c;
        }
    }
    return c;
};

/// Here's a JavaScript function that takes an integer and returns an integer with all the bits from
/// the right filled with 1 up to the input integer:
export const fillBitsFromRight = (num) => {
    // Shift 1 left by the number of bits in the input integer
    let mask = 1n << num;

    // Subtract 1 from the mask to fill the bits
    return mask - 1n;
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
    if (x === 0n) {
        return 0n;
    }
    let p = 0b1n;
    while (p <= x) {
        p <<= 1n;
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

export const runClangFormat = async (filePath) => {
    // Reformat the file
    try {
        await child_process.exec(`clang-format -i "${filePath}"`);
        console.log("Clang-format completed.");
    } catch (err) {
        console.error("Could not re-format the file.", err);
    }
};

export const runCmd = (cmd) => {
    try {
        console.log(`Running:`, cmd);
        return child_process.execSync(cmd).toString();
    } catch (err) {
        console.error("Error running the command:", err);
    }
}

const encoder = new TextEncoder();
export const utf32To8 = (codePoint) => {
    return encoder.encode(String.fromCodePoint(Number(codePoint)));
};

const decoder = new TextDecoder('utf-8');
export const utf8To32 = (utf8Bytes) => {
    return decoder.decode(new Uint8Array(utf8Bytes));
}

export const utf32To8All = (u32Array) => {
    let arr = [];
    for (const codePoint of u32Array) {
        arr.push(...utf32To8(codePoint));
    }
    return arr;
};

export const renderTableValues = (info) => {
    let {name, printableValues, type, len} = info;
    len = len || printableValues.length;
    let valuesTable;
    if (isStringType(type)) {
        const prefix = stringPrefixOf(type);
        valuesTable = `static constexpr std::basic_string_view<${type.description}> ${name.toLowerCase()} {
        ${printableValues.map((val) => {
            val = info?.map?.(val, info) || val;
            let res = "";
            if (val.comment) {
                res += `
                    // ${val.comment}
                `;
            }
            res += `${prefix}"${val?.join("") || val}"`;
            if (val.inline_comment) {
                res += ` // ${val.inline_comment}`;
            }
            if (val?.trailing_comment) {
                res += `
                    // ${val.trailing_comment}
                    `;
            }
            return res;
        })
            .join("\n")}
        , ${len}UL // String Length
    };
            `;
    } else {
        valuesTable = `static constexpr std::array<${type?.name || type.description}, ${len}ULL> ${name.toLowerCase()} {
        ${printableValues.map((val) => {
            val = info?.map?.(val, info) || val;
            let res = "";
            if (val.comment) {
                res += `
                    // ${val.comment}
               `;
            }
            if (isBoolType(type)) {
                res += `0b${val.toString(2).padStart(Number(sizeOf(type)), "0")}`;
            } else if (Array.isArray(val) | ArrayBuffer.isView(val)) {
                res += val.join(", ");
            } else if (typeof val === "number" || typeof val === "bigint") {
                res += `0x${val.toString(16).toUpperCase()}`;
            } else if (typeof val === "string") {
                res += val;
            } else {
                throw new Error(`Don't know how to print this: ${val}`);
            }
            res += ", "
            if (val?.inline_comment) {
                res += ` // ${val.inline_comment}`;
            }
            if (val?.trailing_comment) {
                res += `
                     // ${val.trailing_comment}
                 `;
            }
            return res;
        }).join("\n")}
        // done
    };
            `;
    }

    return valuesTable;
};

export const findTopLongestZeroRanges = (arr, invalidCodePoint = 0, topN = 5,) => {
    let ranges = [];
    let currentLength = 0;
    let startIndex = -1;

    const findRange = (value, i) => {
        // Check if the current element is zero
        if (value === invalidCodePoint) {
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
    };

    if (Array.isArray(arr)) {
        for (let i = 0; i <= arr.length; i++) {
            findRange(arr[i], i);
        }
    } else {
        for (let codePoint in arr) {
            const i = Number(codePoint);
            const value = Number(arr[codePoint]);
            if (isNaN(value) || isNaN(i)) {
                continue;
            }
            findRange(value, i);
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

const MortonTable256 = [0x0000n, 0x0001n, 0x0004n, 0x0005n, 0x0010n, 0x0011n, 0x0014n, 0x0015n, 0x0040n, 0x0041n, 0x0044n, 0x0045n, 0x0050n, 0x0051n, 0x0054n, 0x0055n, 0x0100n, 0x0101n, 0x0104n, 0x0105n, 0x0110n, 0x0111n, 0x0114n, 0x0115n, 0x0140n, 0x0141n, 0x0144n, 0x0145n, 0x0150n, 0x0151n, 0x0154n, 0x0155n, 0x0400n, 0x0401n, 0x0404n, 0x0405n, 0x0410n, 0x0411n, 0x0414n, 0x0415n, 0x0440n, 0x0441n, 0x0444n, 0x0445n, 0x0450n, 0x0451n, 0x0454n, 0x0455n, 0x0500n, 0x0501n, 0x0504n, 0x0505n, 0x0510n, 0x0511n, 0x0514n, 0x0515n, 0x0540n, 0x0541n, 0x0544n, 0x0545n, 0x0550n, 0x0551n, 0x0554n, 0x0555n, 0x1000n, 0x1001n, 0x1004n, 0x1005n, 0x1010n, 0x1011n, 0x1014n, 0x1015n, 0x1040n, 0x1041n, 0x1044n, 0x1045n, 0x1050n, 0x1051n, 0x1054n, 0x1055n, 0x1100n, 0x1101n, 0x1104n, 0x1105n, 0x1110n, 0x1111n, 0x1114n, 0x1115n, 0x1140n, 0x1141n, 0x1144n, 0x1145n, 0x1150n, 0x1151n, 0x1154n, 0x1155n, 0x1400n, 0x1401n, 0x1404n, 0x1405n, 0x1410n, 0x1411n, 0x1414n, 0x1415n, 0x1440n, 0x1441n, 0x1444n, 0x1445n, 0x1450n, 0x1451n, 0x1454n, 0x1455n, 0x1500n, 0x1501n, 0x1504n, 0x1505n, 0x1510n, 0x1511n, 0x1514n, 0x1515n, 0x1540n, 0x1541n, 0x1544n, 0x1545n, 0x1550n, 0x1551n, 0x1554n, 0x1555n, 0x4000n, 0x4001n, 0x4004n, 0x4005n, 0x4010n, 0x4011n, 0x4014n, 0x4015n, 0x4040n, 0x4041n, 0x4044n, 0x4045n, 0x4050n, 0x4051n, 0x4054n, 0x4055n, 0x4100n, 0x4101n, 0x4104n, 0x4105n, 0x4110n, 0x4111n, 0x4114n, 0x4115n, 0x4140n, 0x4141n, 0x4144n, 0x4145n, 0x4150n, 0x4151n, 0x4154n, 0x4155n, 0x4400n, 0x4401n, 0x4404n, 0x4405n, 0x4410n, 0x4411n, 0x4414n, 0x4415n, 0x4440n, 0x4441n, 0x4444n, 0x4445n, 0x4450n, 0x4451n, 0x4454n, 0x4455n, 0x4500n, 0x4501n, 0x4504n, 0x4505n, 0x4510n, 0x4511n, 0x4514n, 0x4515n, 0x4540n, 0x4541n, 0x4544n, 0x4545n, 0x4550n, 0x4551n, 0x4554n, 0x4555n, 0x5000n, 0x5001n, 0x5004n, 0x5005n, 0x5010n, 0x5011n, 0x5014n, 0x5015n, 0x5040n, 0x5041n, 0x5044n, 0x5045n, 0x5050n, 0x5051n, 0x5054n, 0x5055n, 0x5100n, 0x5101n, 0x5104n, 0x5105n, 0x5110n, 0x5111n, 0x5114n, 0x5115n, 0x5140n, 0x5141n, 0x5144n, 0x5145n, 0x5150n, 0x5151n, 0x5154n, 0x5155n, 0x5400n, 0x5401n, 0x5404n, 0x5405n, 0x5410n, 0x5411n, 0x5414n, 0x5415n, 0x5440n, 0x5441n, 0x5444n, 0x5445n, 0x5450n, 0x5451n, 0x5454n, 0x5455n, 0x5500n, 0x5501n, 0x5504n, 0x5505n, 0x5510n, 0x5511n, 0x5514n, 0x5515n, 0x5540n, 0x5541n, 0x5544n, 0x5545n, 0x5550n, 0x5551n, 0x5554n, 0x5555n,];
export const interleaveBits = (x, y) => {
    // x and y must initially be less than 65536.
    if (x >= 65536n || y >= 65536n) {
        throw new Error(`Too big: ${x} ${y}`);
    }
    return ((MortonTable256[y >> 8n] << 17n) | (MortonTable256[x >> 8n] << 16n) | (MortonTable256[y & 0xffn] << 1n) | MortonTable256[x & 0xffn]);
};

export function* findSmallest(arr, op, maxMask = arr.toSorted((a, b) => Number(b - a))[0],) {
    let mask = 0n;

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

export function maskFinder(arr) {
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

export function* findComplements(arr) {
    const sorted = arr.toSorted((a, b) => Number(a - b));
    for (const item in sorted) {
        yield sorted[item];
    }
}

export const findSmallestComplement = (arr) => {
    return findComplements(arr).next().value;
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

export const allBitsTill = (length) => {
    let mask = 0n;
    for (let index = 0n; index !== BigInt(length); ++index) {
        mask |= index;
    }
    return mask;
};

/// Find the biggest mask that if applied to all the positions, then `value` would never come out.
export const largestPositionMask = (table, value = 0) => {
    let mask = bitCeil(BigInt(table.length));
    mask |= mask - 1n;
    for (let index = 0n; index !== BigInt(table.length); ++index) {
        if (table.at(Number(index)) === value) {
            continue;
        }
        mask &= index; // remove the position from the mask
    }
    return mask;
};

export const chunked = (size) => {
    const chunkSize = fillBitsFromRight(BigInt(size)) + 1n;
    const chunkMask = chunkSize - 1n;
    const chunkShift = popcount(chunkMask);
    return {chunkSize, chunkMask, chunkShift};
};

export function fillEmpty(arr, invalidValue = null) {
    return Array.from(arr, (val, i) => (i in arr ? val : invalidValue));
}

export function fillEmptyObject(obj, invalidValue = null) {
    const maxCP = BigInt(Object.keys(obj).map(cp => parseInt(cp)).filter(Number.isInteger).toSorted((a, b) => b - a)[0]);
    for (let codePoint = 0n; codePoint !== maxCP; ++codePoint) {
        if (codePoint in obj) {
            continue;
        }
        obj[codePoint] = invalidValue;
    }
    return obj;
}


/// This is done in reverse order
export function packBoolsIntoInts(boolArray, blockSize = 8n) {
    const result = [];
    const len = BigInt(boolArray.length);
    const lastBit = 0b1n << (blockSize - 1n);
    let currentInt = 0n;

    let bit = 0b1n;
    for (const curBit of boolArray) {
        if (curBit === true) {
            currentInt |= bit;
        } else if (curBit !== false) {
            throw new Error(`Invalid boolean: ${JSON.stringify(curBit)}`);
        }

        // If we've packed 8 booleans, push the current integer to the result
        if (bit === lastBit) {
            result.push(currentInt);
            currentInt = 0n; // Reset for the next integer
            bit = 0b1n;
        } else {
            bit <<= 1n;
        }
    }

    // If there are remaining booleans that don't fill a complete byte
    if (len % blockSize !== 0n) {
        result.push(currentInt);
    }

    return result;
}


export function findBreakPoint(table, tolerance = 3, getValue = (val) => val) {
    let last = NaN;
    let i = table.length - 1;
    let length = 0;
    let lastLength = 0;
    for (; i >= 0 && tolerance !== 0; --i) {
        const cur = table.at(i);
        const curVal = getValue(cur);
        if (last !== curVal) {
            --tolerance;
            lastLength = length;
            length = 0;
        }
        last = curVal;
        ++length;
    }
    return {start: i, length: lastLength};
}

export function getSplitPointsMulti(table, getValue = (val) => val, min_length = 1, splittingValues = undefined) {
    let last = NaN;
    let start = 0;
    let tables = [];
    let i = 0;
    let values = new Set();
    for (; i < (table.length + 1); ++i) {
        const cur = table?.at(i);
        const curVal = getValue?.(cur);
        if (getValue(last) !== curVal && (splittingValues === undefined || splittingValues.includes(cur) || splittingValues.includes(last))) {
            const length = (i - 1) - start;
            if (length >= min_length) {
                tables.push({
                    start, length, // Exclusive (the last value is not included)
                    values: [...values]
                });
            }
            start = i;
            values.clear();
        }
        last = cur;
        values.add(cur);
    }
    return tables;
}


export function getSplitPoints(table, getValue = (val) => val, min_length = 1, splittingValues = undefined) {
    let last = NaN;
    let start = 0;
    let tables = [];
    let i = 0;
    for (; i < (table.length + 1); ++i) {
        const cur = table?.at(i);
        const curVal = getValue?.(cur);
        if (last !== curVal && (splittingValues === undefined || splittingValues.includes(curVal) || splittingValues.includes(last))) {
            const length = (i - 1) - start;
            if (length >= min_length) {
                tables.push({
                    start, length, // Exclusive (the last value is not included)
                    commonValue: last
                });
            }
            start = i;
        }
        last = curVal;
    }
    // console.log(min_length, tables, splittingValues);
    // const length = i - start;
    // if (length >= min_length) {
    //     tables.push({ start, length, commonValue: last });
    // }
    // return tables.toSorted(({ length: lhs }, { length: rhs }) => lhs >= rhs);
    return tables;
}

export function splitOn(table, splits = getSplitPoints(table)) {
    let tables = [];
    let lastEnd = 0;
    let offset = 0;
    for (const {start, length, commonValue} of splits) {
        if (start > lastEnd) {
            tables.push({
                start: lastEnd, length: start - lastEnd, table: table.slice(lastEnd, start), offset
                // no common value here
            });
        }
        tables.push({
            start, length, commonValue, // table: table.slice(start, start + length),
            offset
        });
        lastEnd = start + length;
        offset += length;
    }
    if (lastEnd !== table.length) {
        tables.push({
            start: lastEnd, length: table.length - lastEnd, table: table.slice(lastEnd, table.length), offset
        });
    }
    return tables;
}

/**
 * Finds the topCount packs of continuous values from the input table and splits
 * the table from those found breakpoints.
 * @param {Array} table
 * @param {Number} topCount
 * @param {Function} getValue
 * @param {boolean} singleValue
 * @returns {Array}
 */
export function splitInto(table, topCount = 3, getValue = (val) => val, singleValue = false) {
    if (topCount <= 1) {
        return [{
            start: 0, length: table.length, table
        }];
    }
    const splits = getSplitPoints(table, getValue);
    topCount = splits.length < topCount ? splits.length : topCount;

    const commons = splits
        .filter(item => item?.commonValue !== undefined);
    const lengths = commons.map(({length}) => length)
        .toSorted((lhs, rhs) => rhs - lhs)
        .slice(0, topCount);
    const min_length = lengths[lengths.length - 1];

    if (singleValue) {
        const values = Object.groupBy(commons, ({commonValue}) => commonValue);
        let bestValue = 0;
        for (const commonValue in values) {
            if (values[commonValue].length >= (values[bestValue]?.length || 0)) {
                bestValue = values[commonValue][0].commonValue;
            }
        }
        // console.log(`Best value: ${bestValue}, ${min_length}, ${topCount}, ${lengths} ${JSON.stringify(commons
        //     .toSorted((lhs, rhs) => rhs.length - lhs.length))}`);
        return splitOn(table, getSplitPoints(table, getValue, min_length, [bestValue]));
    } else {
        return splitOn(table, getSplitPoints(table, getValue, min_length));
    }
}


export function splitIntoMulti(table, getValue = (val) => val, minLength = 1) {
    const splits = getSplitPointsMulti(table, getValue, minLength);
    let tables = [];
    let lastEnd = 0;
    let offset = 0;
    for (const {start, length, values} of splits) {
        if (start > lastEnd) {
            tables.push({
                start: lastEnd, length: start - lastEnd, table: table.slice(lastEnd, start), offset
                // no common value here
            });
        }
        tables.push({
            start, length, values, table: table.slice(start, start + length),
            offset
        });
        lastEnd = start + length;
        offset += length;
    }
    if (lastEnd !== table.length) {
        tables.push({
            start: lastEnd, length: table.length - lastEnd, table: table.slice(lastEnd, table.length), offset
        });
    }
    return tables;
}


/// Get the most specific range of values that matches best for the input value
export function getMostSpecializedIn(value, list) {
    let selected = undefined;
    for (const cur of list) {
        if (value >= cur.starting && value <= cur.ending) {
            const selRange = (selected?.ending || BigInt(Number.MAX_VALUE)) - (selected?.starting || 0n);
            const curRange = BigInt(cur.ending - cur.starting);
            if (curRange < selRange) {
                selected = cur;
            }
        }
    }
    return selected;
}

export function getLastSpecializedIn(value, list) {
    for (let i = list.length - 1; i >= 0; --i) {
        const cur = list[i];
        if (value >= cur.starting && value <= cur.ending) {
            return cur;
        }
    }
    return undefined;
}


export function commentify(desc) {
    return desc?.trim()?.replace(/[\n\r]/gim, "\n     * ") || "";
}

export function minRequireStorage(val) {
    val = Number(val);
    for (const bit of [1, 8, 16, 32, 64]) {
        if (val < (0b1 << bit)) {
            return bit;
        }
    }
    throw new Error(`Value it too big: ${val}`);
}

export function findBestTypeFrom(arr, field) {
    try {
        const maxVal = arr.reduce((pick, fields) => Math.max(fields?.[field] || 0, pick), 1);
        return symbolOf(minRequireStorage(maxVal));
    } catch (err) {
        console.error("field:", field, "Arr:", arr);
        throw err;
    }
}

export function alignmentOf(sizes) {
    let max = sizes[0]; // Start with the first element
    for (let i = 1; i < sizes.length; i++) {
        if (Number(sizes[i]) > max) {
            max = Number(sizes[i]);
        }
    }
    return max;
}


// find the overlap of the small table in the big table, if any.
export function findOverlap(small, big) {
    const found = findSimilarRange(small, big);
    if (found !== null) {
        return {
            start: found,
            inserts: null,
            overlap: small.length,
        };
    }
    const overlap = overlapInserts(big, small);
    const start = big.length - overlap;
    return {
        start,
        overlap,
        inserts: small.slice(overlap)
    };
}

// left small table
// right is big table and gets inserted into
export function findOrInsert(left, right) {
    const {start, inserts, overlap} = findOverlap(left, right);
    if (inserts !== null) {
        // console.log(start, overlap, inserts, right)
        right.push(...inserts);
    }
    return start;
}


/// Find the smallest remaining value for the input sparse table
export function findRem(inpTable, defValue = 0, iter = noop) {
    let rem = 0;
    let table = [];

    // Find the remaining:
    nextRem: for (; ; ++rem, iter(rem)) {
        table = [];
        for (let {pos, val} of inpTable) {
            const newPos = pos % rem;
            const newVal = table?.[newPos]?.val;
            if (/* newVal !== val && */ newVal !== undefined) {
                continue nextRem;
            }
            table[pos % rem] = {val, pos};
        }

        break;
    }

    for (let pos = 0; pos !== rem; ++pos) {
        if (table[pos] === undefined) {
            table[pos] = {val: defValue, pos: 0};
        }
    }

    return {
        rem,
        table
    };
}

