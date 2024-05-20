import {alignedSymbol, bitOnesOf, InvalidModifier, maxOf, popcount, sizeOf, symbolOf, uint16, uint8} from "./utils.mjs";


/// Check if the specified "chunk" size is "shift-operator-friendly"
export const verifyChunk = chunk => {
    if (chunk % 2 !== 0) {
        throw new Error("Invalid chunk");
    }
};
export const defaultChunk = 256;
export const chunkMask = chunk => chunk - 1;
export const rangeLength = (codePointStart, dataLength, chunk) => Math.min(dataLength, codePointStart + chunk) - codePointStart;

export class ModifierComputer {
    #chunk = defaultChunk;

    constructor(data, codePointStart = 0, chunk = defaultChunk) {
        this.index = 0;
        this.#chunk = chunk;
        const dataLength = rangeLength(codePointStart, data.length, this.#chunk);
        const masks = new Set();
        const shifts = new Set();
        let zerosIndex = 0;

        for (let codePoint = codePointStart; codePoint < (codePointStart + dataLength); ++codePoint) {
            const ccc = data.at(codePoint);
            masks.add(ccc);
            shifts.add(ccc);
            if (ccc === 0 && zerosIndex === 0) {
                zerosIndex = codePoint;
            }
        }

        // add some masks
        for (let i = 0; ; ++i) {
            const length = masks.size;
            if (i === length) {
                break;
            }
            for (let pos = i; pos !== length; ++pos) {
                masks.add(masks[i] | masks[i + pos]);
            }
        }
        // for (let i = 0; i !== 0xFF; ++i) {
        //     masks.add(i);
        // }

        let value = 0;
        while (value < zerosIndex) {
            value <<= 1;
            value |= 0x1;
        }
        masks.add(value);
        value >>= 1;
        masks.add(value);

        // add some shifts
        for (const item of shifts) {
            shifts.add(-1 * item);
            // shifts.add(-1 * Math.floor(item / 2));
            // shifts.add(Math.floor(item / 2));
        }

        for (let i = 0; i !== 0xFF; ++i) {
            shifts.add(i);
        }

        masks.add(0);
        masks.add(chunkMask(this.#chunk));
        masks.add(this.#chunk - dataLength);
        masks.add(dataLength);
        masks.add(zerosIndex);
        masks.add(Math.floor(this.#chunk / 2));
        masks.add(Math.floor(this.#chunk / 2) - 1);
        masks.add(252);
        masks.add(254);

        shifts.add(0);
        shifts.add(chunkMask(this.#chunk));
        shifts.add(this.#chunk - dataLength);
        shifts.add(dataLength);

        this.shifts = Array.from(shifts).filter(val => (val & chunkMask(this.#chunk)) === val);
        this.masks = Array.from(masks).filter(val => (val & chunkMask(this.#chunk)) === val);
    }

    get mask() {
        return this.masks[Math.floor(this.index / this.shifts.length)];
    }

    get shift() {
        return this.shifts[this.index % this.shifts.length];
    }

    get length() {
        return this.masks.length * this.shifts.length;
    }

    get percent() {
        return this.index / this.length * 100;
    }

    next() {
        ++this.index;
    }
}

/**
 * Each part of a modifier
 */
export class Addendum {
    name = "unknown";
    #desc = null;
    sizeof = uint8;

    /// minimum possible value
    min = 0;

    /// maximum possible value
    max = maxOf(this.sizeof);

    /// where in the Addenda this addendum is; it represents the order of the addenda:
    placement = null;

    /// the value is left-shifted by this amount (it's calculated later on in the Addenda):
    leftShit = 0;

    generable = false;
    generate = () => {
        throw new Error("Cannot generate new values.");
    };

    constructor(data) {
        for (const name in data) {
            const value = data[name];
            if (this[name] === undefined) {
                throw new Error(`Invalid argument specified; arg[${name}] = ${value}`);
            }
            this[name] = value;
        }
        if (data["min"] === undefined) {
            this.min = 0;
        }
        if (data["max"] === undefined) {
            this.max = maxOf(this.sizeof);
        }
        this.generable = data["generate"] !== undefined;
    }

    get size() {
        return sizeOf(this.sizeof);
    }

    /// If all bits are 1, what would the value be?
    get mask() {
        return bitOnesOf(this.size);
    }

    get description() {
        return this.#desc || `${this.name}:`;
    }

    get commentedDescription() {
        return "/// " + this.description.replaceAll(/(\r\n|\r|\n)/g, "\n        /// ");
    }

    set description(value) {
        this.#desc = value;
    }

    render() {
        if (alignedSymbol(this.sizeof)) {
            return `
        ${this.commentedDescription}
        std::${this.sizeof.description} ${this.name};`;
        } else {
            return `
        ${this.commentedDescription}
        std::${this.sizeof.description} ${this.name} : ${this.size};`;
        }
    }

    renderValueSet(valueName = "value") {
        if (this.leftShit === 0) {
            return `${this.name}{static_cast<std::${this.sizeof.description}_t>(${valueName})}`;
        } else {
            return `${this.name}{static_cast<std::${this.sizeof.description}_t>(${valueName} >> ${this.leftShit})}`;
        }
    }
}

export const shiftAddendum = new Addendum({
    name: "shift",
    description: "This value gets added to the retrieved value at the end of the operation.",
    sizeof: uint8,
    * generate() {
        for (let index = this.min; index !== this.max; ++index) {
            yield index;
        }
    }
});

export const maskAddendum = new Addendum({
    name: "mask",
    description: "This is used to mask the 'remaining position' of the values table;\n" +
        "meaning, instead of getting the values_table[0x12'34], we would get values_table[0x12'00].\n" +
        "The mask does not apply to the whole index, but only to the remaining index.",
    sizeof: uint8,
    * generate() {
        yield this.mask;
        yield this.min;
        yield this.max;
    }
});

export const positionAddendum = new Addendum({
    name: "pos",
    description: "This is the position that should be looked for in the values table.",
    sizeof: uint16,
});

export const defaultAddendaPack = [positionAddendum, maskAddendum, shiftAddendum];

/**
 * A combination of specified addenda
 */
export class Addenda {
    name;
    description = null;

    constructor(name = "unknown", addenda = defaultAddendaPack) {
        this.name = name;
        this.addenda = addenda;
        this.sizeof = symbolOf(addenda.reduce((sum, addendum) => sum + sizeOf(addendum.sizeof), 0));
        this.min = addenda.reduce((min, addendum) => (min << addendum.size) | addendum.min, 0);
        this.max = addenda.reduce((max, addendum) => (max << addendum.size) | addendum.max, 0);
        this.mask = addenda.reduce((mask, addendum) => (mask << addendum.size) | addendum.mask, 0);

        /// re-order the placements
        if (addenda.some(addendum => !addendum.placement)) {
            let index = 0;
            for (let addendum of this.addenda) {
                addendum.placement = index;
                ++index;
            }
        }

        // sort based on their placement
        this.addenda.sort((a, b) => a.placement - b.placement);

        // Resetting the shifts:
        let leftShift = this.size;
        for (let addendum of this.addenda) {
            leftShift -= addendum.size;
            addendum.leftShit += leftShift;
        }

        this.#tests();
    }

    #tests() {
        // testing for unthinkable:
        let lastAddendumPlacement = -1;
        for (const addendum of this.addenda) {
            if (addendum.placement !== lastAddendumPlacement + 1) {
                throw new Error(`Invalid placement requirements; ` +
                    `blank placement found in the addenda.` +
                    `Addendum: ${addendum.name} Placement: ${addendum.placement} ` +
                    `Last placement: ${lastAddendumPlacement}`);
            }
            lastAddendumPlacement = addendum.placement;
        }
    }

    /// Get the combined modifier based on the specified values:
    modifier(values = this.addenda.map(_ => 0)) {
        let mod = 0;

        // re-ordering the values based on their names:
        if (values instanceof Object) {
            let newValues = [];
            for (const name of values) {
                const value = values[name];
                const addendum = this.addenda.find(addendum => addendum.name === name);
                if (!addendum) {
                    throw new Error(`Invalid values provided to get the modifier out of the addenda; values: ${values}`);
                }
                newValues[addendum.placement] = value;
            }
            values = newValues;
        }

        // generating the modifier:
        if (values instanceof Array) {
            if (values.length !== this.addenda.length) {
                throw new Error(`Not the right amount of values provided: ${values.length}; values: ${values}`);
            }
            let lastSize = 0;
            for (const index of values) {
                const value = values[index];
                const addendum = this.addenda[index];

                mod <<= lastSize;
                mod |= value;

                lastSize = addendum.size;
            }
        }
        return mod & this.size;
    }

    get size() {
        return sizeOf(this.sizeof);
    }

    renderPlacements() {
        return this.addenda.map(addendum => `[${addendum.size}bits = ${addendum.name}]`).join(" + ");
    }

    render() {
        return `
    /**
     * ${this?.description || this.name}
     */
    struct alignas(std::${this.sizeof.description}_t) ${this.name} {
        ${this.addenda.map(addendum => addendum.render()).join("\n        ")}
       
        /**
         * ${this.renderPlacements()}
         */ 
        consteval ${this.name}(std::${this.sizeof.description}_t const value) noexcept 
            : ${this.addenda.map(addendum => addendum.renderValueSet('value')).join(",\n            ")} {}
    };
        `;
    }
}

/// This shows how the actual algorithm will work.
export class Modifier {
    chunk = defaultChunk;
    #addenda = new Addenda();
    #modifier;

    constructor(modifier, addenda, chunk = defaultChunk) {
        verifyChunk(chunk);
        this.#addenda = addenda;
        this.#modifier = modifier;
        this.chunk = defaultChunk;
        // this.bitCount = popcount(chunkMask(this.chunk));
    }

    static compact(mask, shift) {
        return mask | (shift << 8);
    }

    static createFrom(computer, chunk = defaultChunk) {
        return new Modifier(Modifier.compact(computer.mask, computer.shift), chunk);
    }

    /// only apply the mask
    applyMask(pos) {
        return pos & this.#modifier;
    }

    unapplyShift(value, pos) {
        let shift = (this.#modifier >>> this.bitCount);
        // const maskedPos = modifiers.applyMask(pos, modifier);
        // if (maskedPos === 0) {
        //     shift = 0;
        // }
        return this.#fix(value - shift);
    }

    #fix(value) {
        return value < this.chunk ? value : undefined;
    }

    /// Apply the mask and the shift and finding the actual value in the second table
    /// This is the heart of the algorithm that in C++ we have to implement as well
    apply(pos, table, range = 0) {
        let shift = (this.#modifier >>> this.bitCount) & this.chunkMask;
        const maskedPos = this.applyMask(pos);
        // if (maskedPos === 0) {
        //     shift = 0;
        // }
        return this.#fix((table.at(range + maskedPos)) + shift);
    }

    unshiftAll(list) {
        return list.map((value, index) => this.unapplyShift(value, index));
    }

    /// get the helper code
    helperCode(pos) {
        return (pos << 16) | this.#modifier;
    }

    get mask() {
        return this.#modifier & this.resetMask;
    }

    cccIndexOf(code) {
        return code >>> 16;
    }

    get shift() {
        return this.#modifier >>> 8;
    }

    get chunkMask() {
        return chunkMask(this.chunk);
    }

    get addendumCount() {

    }

    get addendumSize() {
        return this.bitCount * this.addendumCount;
    }

    info() {
        return ({mask: this.mask, shift: this.shift});
    }

    // applyPosition : (pos, modifier, table, range = 0) => table.at(range + (pos & modifier)),

    // applyShift : (value, modifier) => modifiers.fix(value + (modifier >>> 8)),

    // modify: (value, pos, modifier) =>
    //         (pos & modifier) === 0 ? 0 : modifiers.fix(value - (modifier >>> 8)),

    // unapply : (pos, modifier, table, range = 0) =>
    //     modifier.fix((table.at(range + (pos & modifier)) - (modifier >>> 8))),

    // matches : (left, right, lstart, rstart, pos, modifier) => {
    //     const lccc = modifiers.unapply(pos, modifier, left, lstart);
    //     if (lccc === undefined || isNaN(lccc)) {
    //         return false;
    //     }
    //     return right.at(rstart + pos) === lccc;
    // },

    // matchesMask : (left, right, lstart, rstart, pos, modifier) => {
    //     const lccc = modifiers.applyPosition(pos, modifier, left, lstart);
    //     if (lccc === undefined || isNaN(lccc)) {
    //         return false;
    //     }
    //     return right.at(rstart + pos) === lccc;
    // },

}

/**
 * It's an array-looking class that it's values get modified based on the inputted modifier
 */
export class ModifiedSpan {
    #data;
    #modifier;
    #start;

    constructor(data, codePointStart, modifier) {
        this.#data = data;
        this.#modifier = modifier;
        this.#start = codePointStart;
    }

    at(index) {
        if (index < 0 && index >= this.length) {
            throw new Error(`Index out of bounds ${index} out of ${this.length} elements.`);
        }

        const res = this.#modifier.apply(index, this.#data, this.#start);
        if (res === undefined) {
            throw new InvalidModifier(this.#modifier);
        }
        return res;
    }

    get length() {
        return rangeLength(this.#start, this.#data.length);
    }

    slice(index = 0, endIndex = this.length - index) {
        let values = [];
        for (let pos = index; pos !== endIndex; ++pos) {
            values.push(this.at(pos));
        }
        return values;
    }

    * [Symbol.iterator]() {
        for (let pos = 0; pos !== this.length; pos++) {
            yield this.at(pos);
        }
    }

    filter(func) {
        return this.slice().filter(func);
    }
}

