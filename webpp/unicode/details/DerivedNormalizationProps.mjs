import {
    cleanComments, downloadFile, noop, parseCodePointRange, splitLine, updateProgressBar,
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

export const parse = async (table, fileContent = undefined) => {
    if (fileContent === undefined) {
        fileContent = await download();
        if (fileContent === undefined) {
            throw new Error(`We were unable to get the ${cacheFilePath} file.`);
        }
    }

    const lines = fileContent.split("\n");

    let lastCodePoint = 0n;
    const action = (meta) => {
    };

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


        const [codePointStart, codePointEnd] = parseCodePointRange(codePointStr, lastCodePoint);
        for (let curCodePoint = codePointStart; curCodePoint <= codePointEnd; ++curCodePoint) {
            table.add(curCodePoint, {codePoint: curCodePoint, codePointStr, property, value});
        }
        lastCodePoint = codePointEnd + 1n;
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
                this.#data = {};
                break;
            default:
                throw new Error(`Invalid mode: ${this.#mode}`);
        }
    }

    add(codePoint, {property, value}) {
        if (property === this.#prop) {
            switch (this.#mode) {
                case GetTable.arrayMode:
                    this.#data.push(codePoint);
                    break;
                case GetTable.objectMode:
                    this.#data[codePoint] = value;
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


export const getDerivedNormalizationProps = async (property = props.Full_Composition_Exclusion) => {
    const table = new GetTable(property);
    await parse(table);
    return table.data;
};

if (process.argv[1] === new URL(import.meta.url).pathname) {
    const exclusions = await getDerivedNormalizationProps(props.Full_Composition_Exclusion);
    for (const codePoint of exclusions) {
        console.log(codePoint.toString(16));
    }
    console.log("Table length:", exclusions.length);
    console.log("Start Code Point:", exclusions[0]);
    console.log("Last Code Point:", exclusions[exclusions.length - 1]);
}
