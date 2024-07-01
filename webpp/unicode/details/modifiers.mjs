import {
    alignedSymbol,
    bitOnesOf,
    fillBitsFromRight,
    maxOf,
    popcount, realSizeOf,
    sizeOf,
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
    leftShift = 0;

    /// this is the actual size used in the C++ struct
    actualSize = 0;

    /// if the size of this addendum affects the chunk size
    affectsChunkSize = false;

    /// default value of the addendum
    defaultValue = undefined;

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
        this.affectsChunkSize = data["affectsChunkSize"] || false;
        this.actualSize = this.size;
    }

    get size() {
        return sizeOf(this.sizeof);
    }

    /// If all bits are 1, what would the value be?
    get mask() {
        return bitOnesOf(this.actualSize) << this.leftShift;
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
        return this.typeString;
    }

    render(addenda) {
        const defaultStr = this?.defaultValue !== undefined ? ` = ${this.defaultValue}` : "";
        if (alignedSymbol(this.sizeof) || addenda.allAlignable) {
            return `
        ${this.commentedDescription}
        ${this.STLTypeString} ${this.name}${defaultStr};`;
        } else {
            return `
        ${this.commentedDescription}
        ${this.STLTypeString} ${this.name} : ${this.size}${defaultStr};`;
        }
    }

    renderValueSet(valueName = "value", shiftName = "", maskName = "") {
        maskName = maskName === "" ? this.mask : `${this.name}${maskName}`;
        const maskIt = alignedSymbol(this.sizeof) ? `` : maskName;
        const value = maskIt === "" ? valueName : `(${valueName} & ${maskIt})`;
        if (this.leftShift === 0) {
            return `${this.name}{static_cast<${this.STLTypeString}>(${value})}`;
        } else {
            const shift = shiftName === "" ? `${this.leftShift}U` : `${this.name}${shiftName}`;
            return `${this.name}{static_cast<${this.STLTypeString}>(${value} >> ${shift})}`;
        }
    }

    renderShift(type, shiftName = "", maskName = "") {
        if (this.leftShift === 0) {
            return `static_cast<${type}>(${this.name})`;
        } else {
            const shift = shiftName === "" ? `${this.leftShift}U` : `${this.name}${shiftName}`;
            return `(static_cast<${type}>(${this.name}) << ${shift})`;
        }
    }
}

/**
 * A combination of specified addenda
 */
export class Addenda {
    name;
    description = null;
    addenda;
    min;
    max;
    mask;
    sizeof;
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

    constructor(name, addenda, funcs) {
        this.name = name;
        this.addenda = addenda;
        this.sizeof = symbolOf(addenda.reduce((sum, addendum) => sum + sizeOf(addendum.sizeof), 0));

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
        let leftShift = this.allAlignable ? this.alignedSize : this.packedSize;
        for (let addendum of this.addenda) {
            addendum.actualSize = this.allAlignable ? realSizeOf(addendum.sizeof) : addendum.size;
            leftShift -= addendum.actualSize;
            addendum.leftShift += leftShift;
        }

        this.min = addenda.reduce((min, addendum) => (min << addendum.actualSize) | addendum.min, 0);
        this.max = addenda.reduce((max, addendum) => (max << addendum.actualSize) | addendum.max, 0);
        this.mask = addenda.reduce((mask, addendum) => (mask << addendum.actualSize) | addendum.mask, 0);

        /// Enabling addenda.mask, addenda["pos"], and what not syntax
        for (const addendum of this.addenda) {
            this[addendum.name] = addendum;
        }

        if (!Number.isSafeInteger(this.minSize)) {
            debugger;
            throw new Error("Invalid minSize");
        }
        this.#chunkSize = fillBitsFromRight(this.minSize) + 1;
        if (funcs.modify) {
            this.modify = funcs.modify;
        }

        this.#tests();

        if (typeof this.sizeof !== "symbol") {
            debugger;
            throw new Error("Invalid sizeof");
        }
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

    get alignedSize() {
        return this.addenda.reduce((sum, addendum) => sum + realSizeOf(addendum.sizeof), 0);
    }

    get unusedSize() {
        return this.alignedSize - this.size;
    }

    get allAlignable() {
        return this.alignedSize <= this.size;
    }

    get packedSize() {
        return this.addenda.reduce((sum, addendum) => sum + addendum.size, 0);
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

    get defaultValues() {
        const values = {};
        for (const addendum of this.addenda) {
            if (addendum?.defaultValue !== undefined) {
                values[addendum.name] = addendum.defaultValue;
            }
        }
        return values;
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

    has(name) {
        return this.addendum(name) instanceof Addendum;
    }

    /// return the required size for shifting the specified addendum
    shiftOf(addendum) {
        if (typeof addendum === "string") {
            const name = addendum;
            addendum = this.addendum(name);
            assert.ok(addendum, `The specified input should be a valid addendum or a valid addendum name, not ${name}`);
        }
        const {placement: position} = addendum;
        return this.addenda.reduce((sum, addendum) => sum + (addendum.placement > position ? addendum.actualSize : 0), 0);
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

    addendumValueOf(addendum, code) {
        if (typeof addendum === "string") {
            addendum = this.addendum(addendum);
        }
        return (code & addendum.mask) >> this.shiftOf(addendum);
    }

    valuesOf(code) {
        return Object.fromEntries(this.addenda.map(addendum => [addendum.name, this.addendumValueOf(addendum, code)]));
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
                mod.set({[head.name]: headVal, ...tailMod.values()});
                yield mod;
            }
        }
    }

    renderPlacements() {
        return this.addenda.toSorted((a, b) => a.placement - b.placement).map(addendum => `[${addendum.actualSize}bits = ${addendum.name}]`).join(" + ");
    }

    get typeString() {
        return this.sizeof.description;
    }

    get STLTypeString() {
        return this.typeString;
    }

    get desc() {
        return `${this?.description || this.name}`.replace(/\n/g, '\n         * ');
    }

    render() {
        const addenda = this.addenda.toSorted((a, b) => a.placement - b.placement);
        return `
    /**
     * ${this.desc}
     */
    struct alignas(${this.STLTypeString}) ${this.name} {
    
        /// The shifts required to extract the values out of a ${this.STLTypeString}; you can use masks as well:
        ${addenda.map(addendum => `static constexpr std::uint8_t ${addendum.name}_shift = ${addendum.leftShift}U;`).join("\n        ")}
        
        /// The masks required to extracting the values out of a ${this.STLTypeString}; you can use shifts as well:
        ${addenda.map(addendum => `static constexpr ${this.STLTypeString} ${addendum.name}_mask = 0x${addendum.mask.toString(16).toUpperCase()}U;`).join("\n        ")}
    
        ${addenda.map(addendum => addendum.render(this)).join("\n        ")}
       
        /**
         * ${this.renderPlacements()}
         */ 
        explicit(false) consteval ${this.name}(${this.STLTypeString} const value) noexcept 
            : ${addenda.map(addendum => addendum.renderValueSet('value', '_shift', '_mask')).join(",\n              ")} {}
${addenda.length <= 1? "" : ` 
        explicit consteval ${this.name}(${addenda.map(addendum => `${addendum.STLTypeString} const inp_${addendum.name}`).join(",\n                       ")}) noexcept 
            : ${addenda.map(addendum => `${addendum.name}{inp_${addendum.name}}`).join(",\n              ")} {}
`}
        
        [[nodiscard]] constexpr ${this.STLTypeString} value() const noexcept {
            return ${addenda.reverse().map(addendum => addendum.renderShift(this.STLTypeString, '_shift', '_mask')).join(" | ")};
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

    clone() {
        return new Modifier(this.#addenda, this.values());
    }

    set(values) {
        for (const name in values) {
            if (!this.addenda.addendum(name)) {
                throw new Error(`Invalid addendum name ${name}`);
            }
            this[name] = values[name];
        }
        this.modifier = this.addenda.modifierCode(this.values());
    }

    /// reset the addenda that can be reset
    reset() {
        this.set(this.#addenda.defaultValues);
    }

    resetOnly(values) {
        const newValues = {};
        for (const name of values) {
            const defaultValue = this.#addenda.addendum(name)?.defaultValue;
            if (defaultValue !== undefined) {
                newValues[name] = defaultValue;
            }
        }
        this.set(newValues);
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

    get generableAddenda() {
        let values = {};
        for (const addendum of this.#addenda.addenda) {
            const value = this[addendum.name];
            if (value === undefined || !addendum.generable) {
                continue;
            }
            values[addendum.name] = value;
        }
        return values;
    }

    get generableModifier() {
        return this.addenda.modifierCode(this.generableAddenda);
    }

    get categorizableAddenda() {
        let values = {};
        for (const addendum of this.#addenda.addenda) {
            const value = this[addendum.name];
            if (value === undefined || !addendum.isCategorizable) {
                continue;
            }
            values[addendum.name] = value;
        }
        return values;
    }

    get categorizableModifier() {
        return this.addenda.modifierCode(this.categorizableAddenda);
    }

    get chunkSize() {
        // return this.#addenda.chunkSize;

        // call the affectsChunkSize functions
        return this.#addenda.addenda.reduce((size, addendum) => typeof addendum.affectsChunkSize == "function" ? addendum.affectsChunkSize(this, size) : size, this.#addenda.chunkSize);
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

    constructor(data, modifier) {
        this.#data = data;
        this.#modifier = modifier;
    }

    at(index) {
        if (!(index >= 0 && index < this.#modifier.chunkSize)) {
            debugger;
            throw new RangeError(`Index out of bounds ${index} out of ${this.length} elements; chunk size: ${this.#modifier.chunkSize}, start: ${this.start}`);
        }
        return this.#modifier.apply(this.#data, this.start, index);
        // if (!Number.isSafeInteger(res)) {
        //     debugger;
        //     throw new InvalidModifier(this.#modifier);
        // }
        // return res;
    }

    get start() {
        assert.ok(Number.isSafeInteger(this.#modifier.pos), "Invalid position");
        return this.#modifier.pos;
    }

    get data() {
        return this.#data;
    }

    get length() {
        // return rangeLength(this.#start, this.#data.length, this.#modifier.chunkSize);
        // return this.#modifier.chunkSize;
        return Math.min(this.#modifier.chunkSize, this.#data.length);
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
            ${this.pos.STLTypeString} const remaining_pos = static_cast<${this.pos.STLTypeString}>(request_position) & chunk_mask;
            ${this.pos.STLTypeString} const masked_remaining_pos = masked(remaining_pos);
            return pos + masked_remaining_pos;
        }
        `;
}


export const genShiftAddendum = (type = uint8) => new Addendum({
    name: "shift",
    description: "This value gets added to the retrieved value at the end of the operation.",
    sizeof: type,
    affectsChunkSize: false,
    defaultValue: 0,
    isCategorizable: true,
    * generate() {
        assert.ok(this !== undefined, "undefined this?");
        yield this.min;
        yield this.max;
        yield this.mask;
        for (let index = this.min + 1; index <= this.max; ++index) {
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

export const genMaxLengthAddendum = (type = uint8) => new Addendum({
    name: "max_length",
    description: "Length of the UTF-8 Encoded Decomposition Code Points.\n" +
        "This value is the max length of each mappings; there should be additional empty values added\n" +
        "in between the values of the values table in order to make sure we can easily find the needed \n" +
        "mappings for all the code points without searching for them.",
    affectsChunkSize: (modifier, curChunkSize) => {
        return Infinity;
    },
    sizeof: type,
    defaultValue: 1,
    isCategorizable: true,
    modify(modifier, meta) {
        assert.ok(Number.isSafeInteger(modifier.max_length), "Bad max_length?");
        assert.ok(Number.isSafeInteger(meta.pos), "Bad pos?");
        // if (modifier.max_length === 0) {
        //     throw new Error("max_length should be greater than 0");
        // }
        return {
            ...meta,

            // calculate the new position
            pos: meta.pos * modifier.max_length,

            // calculate the new table length
            // length: Math.ceil(meta.length / modifier.max_length)
        };
    }
});

export const genMaskAddendum = (type = uint8) => new Addendum({
    name: "mask",
    description: "This is used to mask the 'remaining position' of the values table;\n" +
        "meaning, instead of getting the values_table[0x12'34], we would get values_table[0x12'00].\n" +
        "The mask does not apply to the whole index, but only to the remaining index.",
    affectsChunkSize: true,
    sizeof: type,
    defaultValue: maxOf(type), // 255 for uint8
    isCategorizable: true,
    * generate() {
        yield this.min;
        yield this.max;
        yield this.mask;
        // for (let index = this.min + 1; index <= this.max; ) {
        //     yield index;
        //     index <<= 1;
        //     // yield index;
        //     index |= 0b1;
        //     // yield (index & ~0b10);
        // }
        if (this.max >= 255) {
            yield 252;
            yield 254;
        }
    },
    modify(modifier, meta) {
        assert.ok(Number.isSafeInteger(modifier.mask), "Bad mask?");
        assert.ok(Number.isSafeInteger(meta.pos), "Bad pos?");
        return {...meta, pos: modifier.mask & meta.pos};
    },
});

export const genPositionAddendum = () => new Addendum({
    name: "pos",
    description: "This is the position that should be looked for in the values table.",
    sizeof: uint16,
    defaultValue: 0,
    affectsChunkSize: false,
    isCategorizable: false,
});

export const genDefaultAddendaPack = (type = uint8) => [genPositionAddendum(), genMaskAddendum(type), genShiftAddendum(type)];
export const genMaskedAddendaPack = (type = uint8) => [genPositionAddendum(), genMaskAddendum(type)];

export const genIndexAddenda = (name = "index", type = uint8) => {
    const addenda = new Addenda(name, genDefaultAddendaPack(type), {
        modify: function (table, modifier, range, pos) {
            const {pos: maskedPos} = this.mask.modify(modifier, {pos});
            const newPos = range + maskedPos;
            if (newPos >= table.length) {
                throw new RangeError(`Invalid position calculated; range: ${range}, pos: ${pos}, faulty pos: ${newPos}, table length: ${table.length}, modifier: ${JSON.stringify(modifier)}`);
            }
            const value = table.at(newPos);
            const newValue = this.shift.modify(modifier, {pos: maskedPos, value}).value;
            if (!Number.isSafeInteger(newValue)) {
                debugger;
                throw new Error(`Something went really wrong; range: ${range}, value: ${newValue}, pos: ${maskedPos}, table: ${JSON.stringify([...table])}`);
            }
            return newValue;
        }
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


export const genMaskedIndexAddenda = (name = "index", type = uint8) => {
    const addenda = new Addenda(name, genMaskedAddendaPack(type), {
        modify: function (table, modifier, range, pos) {
            const {pos: maskedPos} = this.mask.modify(modifier, {pos});
            const newPos = range + maskedPos;
            if (newPos >= table.length) {
                // throw new RangeError(`Invalid position calculated; range: ${range}, pos: ${pos}, faulty pos: ${newPos}, table length: ${table.length}, modifier: ${JSON.stringify(modifier)}`);
                return null;
            }
            const value = table.at(newPos);
            if (!Number.isSafeInteger(value)) {
                debugger;
                throw new Error(`Something went really wrong; range: ${range}, value: ${value}, pos: ${maskedPos}, table: ${JSON.stringify([...table])}`);
            }
            return value;
        }
    });
    addenda.modifierFunctions = {
        applyMask: function (pos) {
            return this.addenda.mask.modify(this, {pos});
        }
    };
    addenda.renderFunctions = [staticFields, maskedFunction, getPositionFunction];
    return addenda;
};
