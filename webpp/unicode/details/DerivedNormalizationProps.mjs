import {
    cleanComments, downloadFile, noop, parseCodePointRange, parseCodePointRangeExclusive, splitLine, updateProgressBar,
} from "./utils.mjs";

export const fileUrl = "https://www.unicode.org/Public/UCD/latest/ucd/DerivedNormalizationProps.txt";
export const cacheFilePath = "DerivedNormalizationProps.txt";

export const download = async (callback = noop) => {
    // database file
    return await downloadFile(fileUrl, cacheFilePath, async (fileContent) => {
        await callback(fileContent);
    });
};

export const props = (() => {
    let props = {
        FC_NFKC_Closure: Symbol("FC_NFKC"), // Deprecated
        Full_Composition_Exclusion: Symbol("Full_Composition_Exclusion"),
        NFD_Quick_Check: Symbol("NFD_QC"), // NFD_Quick_Check=No
        NFC_Quick_Check: Symbol("NFC_QC"), // NFC_Quick_Check=No/Maybe
        NFKD_Quick_Check: Symbol("NFKD_QC"), // NFKD_Quick_Check=No
        NFKC_Quick_Check: Symbol("NFKC_QC"), // NFKC_Quick_Check=No
        Quick_Check: Symbol("NF_QC"), // NFKD_Quick_Check=No
        Expands_On_NFD: Symbol("Expands_On_NFD"), // Deprecated
        Expands_On_NFC: Symbol("Expands_On_NFC"), // Deprecated
        Expands_On_NFKD: Symbol("Expands_On_NFKD"), // Deprecated
        Expands_On_NFKC: Symbol("Expands_On_NFKC"), // Deprecated
        NFKC_Casefold: Symbol("NFKC_CF"),
        NFKC_Simple_Casefold: Symbol("NFKC_SCF"),
        Changes_When_NFKC_Casefolded: Symbol("Changes_When_NFKC_Casefolded"),
    };
    for (let propLongName in props) {
        const propShortName = props[propLongName].description;
        props[propShortName] = props[propLongName];
    }
    return props;
})();

export const isDeprecated = (property) => {
    switch (property) {
        case props.FC_NFKC_Closure:
        case props.Expands_On_NFC:
        case props.Expands_On_NFD:
        case props.Expands_On_NFKC:
        case props.Expands_On_NFKD:
            return true;
    }
    return false;
}

export const defaultValueOf = (property) => {
    switch (property) {
        case props.Quick_Check:
        case props.NFC_Quick_Check:
        case props.NFD_Quick_Check:
        case props.NFKD_Quick_Check:
        case props.NFKC_Quick_Check:
            return "Y";
        default:
            throw new Error(`Haven't implemented yet: ${property?.description || property}`);
    }
};

export const parse = async (table, fileContent = undefined) => {
    if (fileContent === undefined) {
        fileContent = await download();
        if (fileContent === undefined) {
            throw new Error(`We were unable to get the ${cacheFilePath} file.`);
        }
    }

    const lines = fileContent.split("\n");

    lines.forEach((line, index) => {
        line = cleanComments(line);

        // ignore empty lines
        if (line.length === 0) {
            return "";
        }

        // https://www.unicode.org/reports/tr44/#DerivedNormalizationProps.txt
        let [
            codePointStr, // #0
            property, // Symbol
            value // property-dependent value (mappedTo, Yes/No/Maybe)
        ] = splitLine(line);

        updateProgressBar((index / lines.length) * 100);

        if (props[property] === undefined) {
            throw new Error(`Invalid property name found (${property}); is there a new update to Unicode?`);
        }

        property = props[property];

        const [codePointStart, codePointEnd] = parseCodePointRangeExclusive(codePointStr);
        for (let curCodePoint = codePointStart; curCodePoint <= codePointEnd; ++curCodePoint) {
            table.add(curCodePoint, {codePoint: curCodePoint, codePointStr, property, value});
        }
    });
    updateProgressBar(100, `Lines parsed: ${lines.length}`);
};


class GetTable {
    #prop = null;
    #data = [];
    #mode = null;

    static arrayMode = Symbol("ArrayMode");
    static objectMode = Symbol("ObjectMode");
    static auto = Symbol("AutoMode");

    constructor(prop, mode = GetTable.auto) {
        this.#prop = prop;
        this.#mode = mode;
        if (this.#mode === GetTable.auto) {
            this.#mode = (() => {
                switch (this.#prop) {
                    case props.Changes_When_NFKC_Casefolded:
                    case props.Expands_On_NFD:
                    case props.Expands_On_NFC:
                    case props.Expands_On_NFKD:
                    case props.Expands_On_NFKC:
                    case props.Full_Composition_Exclusion:
                        return GetTable.arrayMode;

                    case props.NFKC_Simple_Casefold:
                    case props.NFKC_Casefold:
                    case props.NFKD_Quick_Check:
                    case props.NFKC_Quick_Check:
                    case props.NFD_Quick_Check:
                    case props.NFC_Quick_Check:
                    case props.FC_NFKC_Closure:
                    case props.Quick_Check:
                        return GetTable.objectMode;
                }
                return GetTable.arrayMode;
            })();
        }
        switch (this.#mode) {
            case GetTable.arrayMode:
                this.#data = [];
                break;
            case GetTable.objectMode:
                this.#data = {length: 0};
                break;
            default:
                throw new Error(`Invalid mode: ${this.#mode}`);
        }
    }

    add(codePoint, {property, value}) {
        if (this.#prop === props.Quick_Check) {
            switch (property) {
                case props.NFKD_Quick_Check:
                case props.NFC_Quick_Check:
                case props.NFKC_Quick_Check:
                case props.NFD_Quick_Check:
                    this.#data[codePoint] = {
                        [property.description]: value,
                        ...(this.#data?.[codePoint] || {})
                    };
                    this.#data.length += 1;
                    break;
            }
            return
        }
        if (property === this.#prop) {
            switch (this.#mode) {
                case GetTable.arrayMode:
                    this.#data.push(codePoint);
                    break;
                case GetTable.objectMode:
                    this.#data[codePoint] = value;
                    this.#data.length += 1;
                    break;
                default:
                    throw new Error("Invalid mode.");
            }
        }
    }

    get data() {
        return this.#data;
    }
}

export const getProp = (data, codePoint, property) => data?.[codePoint] || defaultValueOf(property);

export const getDerivedNormalizationProps = async (property = props.Full_Composition_Exclusion) => {
    const table = new GetTable(property);
    await parse(table);
    return table.data;
};

export const getQuickChecks = async () => {
    return getDerivedNormalizationProps(props.Quick_Check);
};

if (process.argv[1] === new URL(import.meta.url).pathname) {
    const qcs = await getQuickChecks();
    for (const codePoint in qcs) {
        console.log(parseInt(codePoint).toString(16), getProp(qcs, codePoint, props.Quick_Check));
    }
    console.log("Table length:", qcs.length);
    console.log("Start Code Point:", qcs[0]);
    console.log("Last Code Point:", qcs[qcs.length - 1]);
}
