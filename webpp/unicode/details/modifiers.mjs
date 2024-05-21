import {
    alignedSymbol,
    bitOnesOf,
    InvalidModifier,
    maxOf,
    popcount,
    sizeOf,
    Span,
    symbolOf,
    uint16,
    uint8
} from "./utils.mjs";
import * as assert from "node:assert";


/// Check if the specified "chunk" size is "shift-operator-friendly"
export const verifyChunk = chunk => {
    assert.ok(chunk % 2 === 0, "Invalid chunk");
};
export const defaultChunk = 256;
export const chunkMask = chunk => chunk - 1;
export const rangeLength = (codePointStart, dataLength, chunk) => Math.min(dataLength, codePointStart + chunk) - codePointStart;

export class ModifierComputer {

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
            let value = data[name];
            assert.ok(this[name] !== undefined, `Invalid argument specified; arg[${name}] = ${JSON.stringify(value)}`);

            // this, is that :)
            if (value instanceof Function) {
                value = value.bind(this);
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
        assert.ok(this !== undefined, "undefined this?");
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
        yield this.min;
        yield this.max;
        yield this.mask;
        yield 252;
        yield 254;
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
                assert.ok(addendum, `Invalid values provided to get the modifier out of the addenda; values: ${values}`);
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

    /// Generate all possible combinations of the addenda
    * generate(addenda = this.addenda) {
        const generables = addenda.filter(addendum => addendum.generable);
        if (generables.length === 0) {
            yield {};
            return;
        }

        const [head, ...tail] = generables;
        for (const headVal of head.generate()) {
            for (const tailVals of this.generate(tail)) {
                yield {[head.name]: headVal, ...tailVals};
            }
        }
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
        assert.ok(index >= 0 && index < this.length, `Index out of bounds ${index} out of ${this.length} elements.`);

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

