import {promises as fs} from 'fs';
import * as process from "node:process";


export const uint8 = Symbol('uint8');
export const uint16 = Symbol('uint16');
export const uint8x2 = Symbol('uint8');
export const uint32 = Symbol('uint32');

export const noop = _ => {};

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

/**
 * This class will let us handle the types of the tables including:
 *   - unsigned integer 32 bit (uint32)
 *   - unsigned integer 8 bit  (uint8)
 *   - unsigned integer 16 bit (uint16)
 */
export class TableTraits {

    constructor(max, type = uint8) {
        switch (type) {
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
        this.type = type;
        this.index = 0;
    }

    get sizeof() {
        switch (this.type) {
            case uint8x2:
                return 16;
            case uint8:
                return 8;
            case uint32:
                return 32;
            default:
                return 0;
        }
    }

    get typeString() { return this.type.description; }

    get postfix() {
        switch (this.type) {
            case uint8x2:
            case uint8:
                return "U";
            case uint32:
                return "ULL";
            default:
                return "";
        }
    }

    get length() { return this.index; }

    get result() { return this.bytes.slice(0, this.length); }

    at(index) { return this.bytes.at(index); }

    * [ Symbol.iterator ]() {
        for (let pos = 0; pos !== this.length; pos++) {
            yield this.at(pos);
        }
    }
    set(index, value) { return this.bytes[index] = value; }

    append(value) { this.bytes[this.index++] = value; }

    appendList(list) {
        for (const value of list) {
            this.append(value);
        }
    }
}

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

    get length() { return this.#end - this.#start; }

    slice(start = 0, end = this.length - start) {
        const newStart = this.#start + start;
        end = Math.min(this.#end, end);
        const newLength = this.#start + end - newStart;
        return new Span(this.#arr, newStart, newLength, this.#func);
    }

    expand(newStart = 0, newLength = this.length + newStart) {
        if (newStart < 0 && newStart >= this.length) {
            throw new Error(`Index out of bounds ${index} out of ${this.length} elements.`);
        }
        newLength = Math.min(this.#arr.length, newLength);
        return new Span(this.#arr, newStart, newLength, this.#func);
    }

    filter(func) {
        let values = [];
        for (const val of this) {
            values.push(func(val));
        }
        return values;
    }

    map(func) { return new Span(this.#arr, this.#start, this.length, func); }

    at(index) {
        if (index >= 0 && index < this.length) {
            return this.#func(this.#arr[this.#start + index]);
        } else {
            throw new Error(`Index out of bounds ${index} out of ${this.length} elements.`);
        }
    }

    * [ Symbol.iterator ]() {
        for (let i = this.#start; i < this.#end; i++) {
            yield this.#func(this.#arr[i]);
        }
    }
}

export class InvalidModifier {
    #data;
    constructor(data) { this.#data = data; }
    toString() { return JSON.stringify(this.#data); }
}

/// Find the start position of "left" in "right"
/// This function finds a place in the "right" table where the specified range
/// will be there.
export const findSimilarRange = (left, right) => {
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
    return null;
};

// Overlap Inserts Optimization:
///    if the "right" table's tail has a match for the beginning of the "left" table,
///    then we can omit inserting the first part of the "left" table.
export const overlapInserts = (left, right) => {
    if (left.length === 0) {
        return 0;
    }
    let rpos = Math.max(0, (right.length - 1) - left.length);
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

