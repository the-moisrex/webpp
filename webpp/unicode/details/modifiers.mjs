import {
    alignedSymbol,
    bitOnesOf,
    fillBitsFromRight,
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
export const chunkMask = chunk => chunk - 1;
export const rangeLength = (codePointStart, dataLength, chunk) => Math.min(dataLength, codePointStart + chunk) - codePointStart;

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

    /// if the size of this addendum affects the chunk size
    affectsChunkSize = false;

    generable = false;
    generate = () => {
        throw new Error("Cannot generate new values.");
    };

    /// Modify the values and essentially apply the addendum to the values
    /// @param modifier: Modifier
    /// @param meta: {value: ..., pos: ...}
    modify = (modifier, meta) => {
        return meta; /// no changes to meta
    };

    constructor(data) {
        for (const name in data) {
            let value = data[name];
            // assert.ok(this[name] !== undefined, `Invalid argument specified; arg[${name}] = ${JSON.stringify(value)}`);

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
        this.affectsChunkSize = !!data["affectsChunkSize"] || false;
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

    get typeString() {
        return this.sizeof.description;
    }

    get STLTypeString() {
        return `std::${this.typeString}_t`;
    }

    render() {
        if (alignedSymbol(this.sizeof)) {
            return `
        ${this.commentedDescription}
        ${this.STLTypeString} ${this.name};`;
        } else {
            return `
        ${this.commentedDescription}
        ${this.STLTypeString} ${this.name} : ${this.size};`;
        }
    }

    renderValueSet(valueName = "value") {
        if (this.leftShit === 0) {
            return `${this.name}{static_cast<${this.STLTypeString}>(${valueName})}`;
        } else {
            return `${this.name}{static_cast<${this.STLTypeString}>(${valueName} >> ${this.leftShit})}`;
        }
    }

    renderShift(type) {
        if (this.leftShit === 0) {
            return `static_cast<${type}>(${this.name})`;
        } else {
            return `(static_cast<${type}>(${this.name}) << ${this.leftShit}U)`;
        }
    }
}

/**
 * A combination of specified addenda
 */
export class Addenda {
    name;
    description = null;
    #chunkSize = NaN;
    #renderFunctions = [];

    /// @param table = the data
    /// @param range = start of the bucket
    /// @param pos   = the remaining position
    #modify = (table, modifier, range, pos) => {
        debugger;
        throw new Error("The addenda's algorithm is not implemented.");
    };

    #modifierFunctions = {};

    constructor(name, addenda, modifyFunc) {
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

        /// Enabling addenda.mask, addenda["pos"], and what not syntax
        for (const addendum of this.addenda) {
            this[addendum.name] = addendum;
        }

        this.#chunkSize = fillBitsFromRight(this.minSize) + 1;
        this.modify = modifyFunc;

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

    get size() {
        return sizeOf(this.sizeof);
    }

    get modify() {
        return this.#modify;
    }

    set modify(value) {
        this.#modify = value.bind(this);
    }

    get modifierFunctions() {
        return this.#modifierFunctions;
    }

    set modifierFunctions(value) {
        if (typeof value !== 'object') {
            throw new Error(`Invalid modifier function: ${value}`);
        }
        for (const funcName in value) {
            if (typeof value[funcName] !== 'function') {
                throw new Error(`Invalid modifier function: ${value}; it's supposed to be a function.`);
            }
        }
        this.#modifierFunctions = value;
    }

    set renderFunctions(value) {
        this.#renderFunctions = value.map(func => func.bind(this));
    }

    addendum(name) {
        return this.addenda.find(addendum => addendum.name === name);
    }

    /// return the required size for shifting the specified addendum
    shiftOf(addendum) {
        if (typeof addendum === "string") {
            const name = addendum;
            addendum = this.addendum(name);
            assert.ok(addendum, `The specified input should be a valid addendum or a valid addendum name, not ${name}`);
        }
        const {placement: position} = addendum;
        return this.addenda.reduce((sum, addendum) => sum + (addendum.placement > position ? addendum.size : 0), 0);
    }

    /// Get the combined modifier based on the specified values:
    modifierCode(values) {
        let mod = 0;
        for (const name in values) {
            const value = values[name];
            mod |= (value << this.shiftOf(name));
        }
        return mod;
    }

    valueOf(addendum, code) {
        if (typeof addendum === "string") {
            addendum = this.addendum(addendum);
        }
        return code >> this.shiftOf(addendum);
    }

    valuesOf(code) {
        return Object.fromEntries(this.addenda.map(addendum => [addendum.name, this.valueOf(addendum, code)]));
    }

    modifierOf(code) {
        return new Modifier(this, this.valuesOf(code));
    }

    get minSize() {
        return this.addenda.reduce((min, addendum) => !addendum.affectsChunkSize || addendum.size > min ? min : addendum.size, NaN);
    }

    get chunkSize() {
        return this.#chunkSize;
    }

    get chunkMask() {
        return this.#chunkSize - 1;
    }

    get chunkShift() {
        return popcount(this.chunkMask);
    }

    /// Generate all possible combinations of the addenda
    * generate(addenda = this.addenda) {
        const generables = addenda.filter(addendum => addendum.generable);
        let mod = new Modifier(this);
        if (generables.length === 0) {
            yield mod;
            return;
        }

        const [head, ...tail] = generables;
        for (const headVal of head.generate()) {
            for (const tailMod of this.generate(tail)) {
                let values = {[head.name]: headVal, ...tailMod.values()};
                mod.set({...values, modifier: this.modifierCode(values)});
                yield mod;
            }
        }
    }

    renderPlacements() {
        return this.addenda.map(addendum => `[${addendum.size}bits = ${addendum.name}]`).join(" + ");
    }

    get typeString() {
        return this.sizeof.description;
    }

    get STLTypeString() {
        return `std::${this.typeString}_t`;
    }

    render() {
        return `
    /**
     * ${this?.description || this.name}
     */
    struct alignas(${this.STLTypeString}) ${this.name} {
        ${this.addenda.map(addendum => addendum.render()).join("\n        ")}
       
        /**
         * ${this.renderPlacements()}
         */ 
        explicit(false) consteval ${this.name}(${this.STLTypeString} const value) noexcept 
            : ${this.addenda.map(addendum => addendum.renderValueSet('value')).join(",\n              ")} {}
${""/*       
        constexpr ${this.name}(${this.addenda.map(addendum => `${addendum.STLTypeString} const inp_${addendum.name}`).join(",\n                       ")}) noexcept 
            : ${this.addenda.map(addendum => `${addendum.name}{inp_${addendum.name}}`).join(",\n              ")} {}
*/}
        
        [[nodiscard]] constexpr ${this.STLTypeString} value() const noexcept {
            return ${this.addenda.map(addendum => addendum.renderShift(this.STLTypeString)).join(" & ")};
        }

${this.#renderFunctions.map(func => func()).join("\n\n")}

    };
        `;
    }
}

export class InvalidModifier {
    #data;

    constructor(data) {
        this.#data = data;
    }

    toString() {
        return JSON.stringify(this.#data);
    }
}

/// This shows how the actual algorithm will work.
export class Modifier {
    #addenda;
    modifier = 0;

    constructor(addenda, values = {}) {
        if (!(addenda instanceof Addenda)) {
            throw new Error("'addenda' should be Addenda");
        }
        this.#addenda = addenda;

        // Making the functions public
        for (const funcName in this.#addenda.modifierFunctions) {
            this[funcName] = this.#addenda.modifierFunctions[funcName].bind(this);
        }
        this.set(values);
    }

    get addenda() {
        return this.#addenda;
    }

    get name() {
        return this.#addenda.name;
    }

    set(values) {
        this.modifier = values.modifier || this.modifier;
        delete values.modifier;
        for (const name in values) {
            if (!this.addenda.addendum(name)) {
                throw new Error(`Invalid addendum name ${name}`);
            }
            this[name] = values[name];
        }
    }

    values() {
        let values = {};
        for (const addendum of this.#addenda.addenda) {
            const value = this[addendum.name];
            if (value === undefined) {
                continue;
            }
            values[addendum.name] = value;
        }
        return values;
    }

    get chunkSize() {
        return this.#addenda.chunkSize;
    }

    get chunkShift() {
        return this.#addenda.chunkShift;
    }

    get chunkMask() {
        return this.#addenda.chunkMask;
    }

    /// Apply the mask and the shift and finding the actual value in the second table
    /// This is the heart of the algorithm that in C++ we have to implement as well
    apply(table, range, pos) {
        return this.#addenda.modify(table, this, range, pos);
    }

    /// Get only important stuff, used for printing mostly
    necessaries() {
        return {
            modifier: this.modifier,
            ...Object.fromEntries(this.#addenda.addenda.filter(addendum => addendum.generable).map(addendum => [addendum.name, this[addendum.name]]))
        };
    }

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
        if (!(index >= 0 && index < this.length)) {
            throw new RangeError(`Index out of bounds ${index} out of ${this.length} elements.`);
        }

        const res = this.#modifier.apply(this.#data, this.#start, index);
        if (!isFinite(res)) {
            debugger;
            throw new InvalidModifier(this.#modifier);
        }
        return res;
    }

    get length() {
        return rangeLength(this.#start, this.#data.length, this.#modifier.chunkSize);
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


////////////////////////////// Implementation of Defaults //////////////////////////////

export function staticFields() {
    return `
        static constexpr ${this.pos.STLTypeString} chunk_mask = 0x${this.chunkMask.toString(16).toUpperCase()}U;
        static constexpr std::size_t chunk_size = ${this.chunkSize}U;
        static constexpr std::uint8_t chunk_shift = ${this.chunkShift}U;
            `;
}
export function maskedFunction() {
    return `
        /**
         * Apply the mask to the position specified in the input.
         * Attention: input is the remaining part of the position, meaning 
         *    remaining_pos = (code_point & ${this.chunkMask}); // ${this.chunkMask} refers to chunk_mask
         */
        [[nodiscard]] constexpr ${this.pos.STLTypeString} masked(auto const remaining_pos) const noexcept {
            return static_cast<${this.pos.STLTypeString}>(${this.mask.name}) & static_cast<${this.pos.STLTypeString}>(remaining_pos);
        }
            `;
}
export function getPositionFunction() {
    return `
        /**
         * Get the final position of the second table.
         * This does not apply the shift or get the value of the second table for you; this only applies tha mask.
         */
        [[nodiscard]] constexpr ${this.pos.STLTypeString} get_position(auto const request_position) const noexcept {
            auto const range = static_cast<${this.pos.STLTypeString}>(request_position) >> chunk_shift;
            auto const remaining_pos = static_cast<${this.pos.STLTypeString}>(request_position) & chunk_mask;
            auto const masked_remaining_pos = masked(remaining_pos);
            return range + masked_remaining_pos;
        }
            `;
}


export const genShiftAddendum = () => new Addendum({
    name: "shift",
    description: "This value gets added to the retrieved value at the end of the operation.",
    sizeof: uint8,
    affectsChunkSize: false,
    * generate() {
        assert.ok(this !== undefined, "undefined this?");
        for (let index = this.min; index !== this.max; ++index) {
            yield index;
        }
    },
    modify(modifier, meta) {
        return {...meta, value: modifier.shift + meta.value};
    },
    unshift(modifier, meta) {
        return {...meta, value: meta.value - modifier.shift};
    }
});

export const genMaskAddendum = () => new Addendum({
    name: "mask",
    description: "This is used to mask the 'remaining position' of the values table;\n" +
        "meaning, instead of getting the values_table[0x12'34], we would get values_table[0x12'00].\n" +
        "The mask does not apply to the whole index, but only to the remaining index.",
    affectsChunkSize: true,
    sizeof: uint8,
    * generate() {
        yield this.min;
        yield this.max;
        yield this.mask;
        yield 252;
        yield 254;
    },
    modify(modifier, meta) {
        assert.ok(isFinite(modifier.mask), "Bad mask?");
        return {...meta, pos: modifier.mask & meta.pos};
    },
});

export const genPositionAddendum = () => new Addendum({
    name: "pos",
    description: "This is the position that should be looked for in the values table.",
    sizeof: uint16,
    affectsChunkSize: false,
});

export const genDefaultAddendaPack = () => [genPositionAddendum(), genMaskAddendum(), genShiftAddendum()];
export const genMaskedAddendaPack = () => [genPositionAddendum(), genMaskAddendum()];

export const genIndexAddenda = (name = "index") => {
    const addenda = new Addenda(name, genDefaultAddendaPack(), function (table, modifier, range, pos) {
        const {pos: maskedPos} = this.mask.modify(modifier, {pos});
        const value = table.at(range + maskedPos);
        if (!isFinite(value)) {
            debugger;
            throw new Error("Something went really wrong.");
        }
        return this.shift.modify(modifier, {pos: maskedPos, value}).value;
    });
    addenda.modifierFunctions = {
        applyMask: function (pos) {
            return this.addenda.mask.modify(this, pos);
        },
        unshiftAll: function (list) {
            const modifier = this;
            return list.map((value, index) => this.addenda.shift.unshift(modifier, {
                value,
                pos: index
            }).value);
        }
    };
    addenda.renderFunctions = [staticFields, maskedFunction, getPositionFunction];
    return addenda;
};


export const genMaskedIndexAddenda = (name = "index") => {
    const addenda = new Addenda(name, genMaskedAddendaPack(), function (table, modifier, range, pos) {
        const {pos: maskedPos} = this.mask.modify(modifier, {pos});
        const value = table.at(range + maskedPos);
        if (!isFinite(value)) {
            debugger;
            throw new Error("Something went really wrong.");
        }
        return value;
    });
    addenda.modifierFunctions = {
        applyMask: function (pos) {
            return this.addenda.mask.modify(this, pos);
        }
    };
    addenda.renderFunctions = [staticFields, maskedFunction, getPositionFunction];
    return addenda;
};
