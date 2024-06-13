import {promises as fs} from 'fs';
import * as process from "node:process";
import * as assert from "node:assert";
import child_process from "node:child_process";


export const uint6 = Symbol('uint8');
export const uint7 = Symbol('uint8');
export const uint8 = Symbol('uint8');
export const uint16 = Symbol('uint16');
export const uint8x2 = Symbol('uint8');
export const uint32 = Symbol('uint32');
export const uint64 = Symbol('uint64');

// A symbol to identify the choice when a code point is mapped to multiple other values (for example Decomposition)
export const multiMap = Symbol('multiMap');

// Opposite of multiMap; single code point is mapped to one single value (for example CCC)
export const directMap = Symbol('directMap');

export const sizeOf = symbol => {
    switch (symbol) {
        case uint6:
            return 6;
        case uint7:
            return 7;
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
    if (size <= 6) {
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
        return `std::${this.typeString}_t`;
    }

    get postfix() {
        switch (this.type) {
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

const encoder = new TextEncoder('utf-8');
export const utf32To8 = (codePoint) => {
    return encoder.encode(String.fromCharCode(codePoint));
}

export const utf32To8All = (u32Array) => {
    let arr = [];
    for (const codePoint of u32Array) {
        for (const unit of utf32To8(codePoint)) {
            arr.push(unit);
        }
    }
    return arr;
}