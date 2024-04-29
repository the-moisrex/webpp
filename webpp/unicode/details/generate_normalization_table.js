/***
 * This file downloads UnicodeData.txt, and generates a C++ header file.
 *
 * Details on parsing this file can be found here:
 * UTS #44: https://www.unicode.org/reports/tr44/#UnicodeData.txt
 */

const fs = require('fs').promises;
const path = require('path');
const fileUrl = 'https://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt';
const readmeUrl = 'https://www.unicode.org/Public/UCD/latest/ucd/ReadMe.txt';
const cacheFilePath = 'UnicodeData.txt';
const cacheReadmePath = 'ReadMe.txt';
const outFilePath = `normalization_tables.hpp`;

const uint8 = Symbol('uint8');
const uint32 = Symbol('uint32');

const readmeData = {
  version : "",
  date : ""
}

const popcount = n => {
  let c = 0;
  for (; n !== 0; n >>= 1) {
    if ((n & 1) !== 0) {
      c++;
    }
  }
  return c;
};
const downloadFile = async (url, file, process) => {
  try {
    // Check if the file already exists in the cache
    await fs.access(file);
    try {
      console.log(`Using cached file ${file}...`);
      const fileContent = await fs.readFile(file);
      process(fileContent.toString());
      return;
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
  } catch (error) {
    console.error('Error:', error.message);
  }
} const start = async () => {
  // readme file for getting the version and what not
  await downloadFile(readmeUrl, cacheReadmePath, processReadmeFile);
  if (readmeData.version === undefined) {
    console.error("Could not find the version from the file content.");
    return;
  }
  if (readmeData.date === undefined) {
    console.error("No date was found.");
    return;
  }
  console.log(`Version:       ${readmeData.version}`);
  console.log(`Creation Date: ${readmeData.date}`);

  // database file
  await downloadFile(fileUrl, cacheFilePath, processCachedFile);
};

const processReadmeFile =
    content => {
      readmeData.version = findVersion(content);
      readmeData.date = findDate(content);
    }

const cleanComments = line => line.split('#')[0].trimEnd()
const splitLine = line => line.split(';').map(seg => seg.trim());
const findVersion = fileContent => fileContent.match(/Version:? (\d+\.\d+\.\d+)/)[1];
const findDate = fileContent => fileContent.match(/Date: ([^\n\r]+)/)[1];
const parseCodePoints = codePoint => parseInt(codePoint, 16);

/**
 * This class will let us handle the types of the tables including:
 *   - unsigned integer 32 bit (uint32)
 *   - unsigned integer 8 bit  (uint8)
 */
class TableTraits {

  constructor(max, type = uint8) {
    switch (type) {
    case uint8:
      this.bytes = new Uint8Array(max);
      break;
    case uint32:
      this.bytes = new Uint32Array(max);
      break;
    default:
      throw new Error('Invalid type provided to CodePointMapper.');
    }
    this.type = type;
    this.index = 0;
  }

  get sizeof() {
    switch (this.type) {
    case uint8:
      return 8;
    case uint32:
      return 32;
    default:
      return 0;
    }
  }

  get typeString() { return this.type.description; }

  get postfix() { return this.type === uint8 ? "U" : "ULL"; }

  get length() { return this.index; }

  append(value) { this.bytes[this.index++] = value; }

  appendList(list) {
    for (const value of list) {
      this.append(value);
    }
  }
}

/*
class MagicNumberStat {
    constructor(magicNumber = 0, category = 0) {
        this.category = category;
        this.magicNumber = magicNumber;

        this.xor2zero = 0;
        this.xor2cat = 0;
        this.and2zero = 0;
        this.and2cat = 0;
        this.total = 0;
        this.falsePositives = 0;
    }

    add(codePoint, ccc) {
        ++this.total;
        if (this.category !== ccc) {
            ++this.falsePositives;
        }

        if ((codePoint ^ this.magicNumber) === 0) {
            ++this.xor2zero;
        }

        if ((codePoint & this.magicNumber) === 0) {
            ++this.and2zero;
        }

        if ((codePoint ^ this.magicNumber) === this.category) {
            ++this.xor2cat;
        }

        if ((codePoint & this.magicNumber) === this.category) {
            ++this.and2cat;
        }
    }

    percentOf(value) {
        return Math.floor(value / this.total * 100);
    }

    printItem(valueStr) {
        const value = this[valueStr];
        const percent = this.percentOf(value);
        const falsePercent = this.percentOf(this.falsePositives);
        if (percent <= 50) {
            return;
        }
        console.log(valueStr, `   ${value} / ${this.total}  `, `${percent}%`, `false(${falsePercent}%)`,
            `   cat(${this.category})`, `   magic(${this.magicNumber})`);
    }

    print() {
        this.printItem(`xor2zero`);
        this.printItem(`and2zero`);
        this.printItem(`xor2cat`);
        this.printItem(`and2cat`);
    }
}

class MagicBucket {
    constructor(start = 1, end = 1000) {
        this.rangeStart = start;
        this.rangeEnd = end;
        this.bucket = [];

        console.log("Preparing Magic Number Buckets...");
        for (let index = this.rangeStart; index !== this.rangeEnd; ++index) {
            for (let cat = 0; cat !== 256; ++cat) {
                this.bucket.push(new MagicNumberStat(index, cat));
            }
        }
        console.log("Preparing Magic Number Buckets: done.");
    }

    add(codePoint, ccc) {
        for (const mag of this.bucket) {
            mag.add(codePoint, ccc);
        }
    }

    print() {
        for (const mag of this.bucket) {
            mag.print();
        }
    }
}

class range {
    constructor(start = 0, end = start) {
        this.start = start;
        this.end = end;
    }
}
*/

class CCCTables {
  constructor() {
    this.lastZero = 0;
    this.indeces = new TableTraits(4353, uint32);
    this.cccs = new TableTraits(68 * 256, uint8);
    this.data = [];
  }

  add(codePoint, ccc) {

    // fill the data
    this.data.push({codePoint, ccc});

    // calculating the last item that it's ccc is zero
    if (ccc !== 0) {
      this.lastZero = codePoint + 1;
    }
  }

  /// Post-Processing
  finalize() {
    console.log("Finalizing...");

    const resetMask = 0xFF;
    const isZeroingOut = (mask, codePoint) =>
        (mask & (codePoint & resetMask)) === 0 && (codePoint & resetMask) !== 0;

    const isZeroingOutRange = (mask, index, length) => {
      const end = index + length;
      for (; index !== end; ++index) {
        const {codePoint} = this.data[index];
        if (isZeroingOut(mask, codePoint)) {
          return true;
        }
      }
      return false;
    };

    /// This function finds a place in the "this.cccs" table where the specified range
    /// will be there.
    const findSimilarRange = (index = 0, length = 256) => {
      const end = index + length;
      const range = this.data.slice(index, end);

      let failedMasks = [];
      top: for (let pos = 0; pos < (this.data.length - length); ++pos) {

        // try finding a range:
        let mask = resetMask;
        for (let subPos = pos; subPos < (pos + length); ++subPos) {
          const diff = subPos - pos;
          const {codePoint : subCodePoint, ccc : subCCC} = this.data[subPos];
          const {codePoint : rangeCodePoint, ccc : rangeCCC} = range[diff];

          if (rangeCCC !== subCCC) {

            // try omitting this with a mask if possible
            if (rangeCCC === 0) {

              // ignoring this code point by masking it
              mask &= ~diff;

              // check the mask for bytes checked so far
              if (isZeroingOutRange(mask, pos, diff)) {
                failedMasks.push(mask);
                mask = resetMask;
                continue top;
              }

              continue;
            }

            // couldn't find it here, let's go to the next position
            continue top;
          }

          // the mask doesn't hold:
          if (isZeroingOut(mask, subCodePoint)) {
            failedMasks.push(mask);
            mask = resetMask;
            continue top;
          }
        }
        if (failedMasks.length > 0) {
          // console.log("  Failed Masks(found):", failedMasks.length);
        }

        // found a range
        return {pos, mask};
      }
      if (failedMasks.length > 0) {
        console.log("  Failed Masks(not found):", failedMasks.length);
      }
      return {pos : null, mask : null};
    };

    let batchNo = 0;
    let insertedCount = 0;
    let reusedCount = 0;
    for (let range = 0; range <= this.data.length; range += 256) {

      const codeRange = range >>> 8;
      const length = Math.min(this.data.length - range, 256);

      console.log(`Batch #${batchNo++}: `, codeRange, length,
                  `Progress: ${Math.floor(range / this.data.length * 100)}%`);

      // for (let index = range; index !== (range + length); ++index) {
      const {codePoint, ccc} = this.data[range];
      const remPoint = codePoint % 256;
      const {pos : startPos, mask} = findSimilarRange(range, length);
      if (startPos === null) {
        // add the date to the cccs list as well:
        const helperCode = (codeRange << 8) | mask;
        this.indeces.append(helperCode);
        this.cccs.appendList(this.data.slice(range, range + length).map(item => item.codePoint));
        console.log("  Code Range (inserted):", codeRange)++ insertedCount;
      } else {
        // no need to add the data to the cccs list, we already have similar thing there:
        const helperCode = (startPos << 8) | mask;
        this.indeces.append(helperCode);
        console.log("  Code Range (reused):", codeRange, "Mask: ", mask);
        ++reusedCount;
      }
      // }
    }

    console.log("Inserted: ", insertedCount, "reused:", reusedCount);
    console.log("CCCs Table Length:", this.cccs.length);
    console.log("Finalizing: done.");
  }

  render() {
    return `
    
    static constexpr auto trailing_zero_cccs = 0x${this.lastZero.toString(16).toUpperCase()}UL;
    
    /**
     * CCC Index Table
     *
     * CCC: Canonical Combining Class
     * 
     * Table size:
     *   - in bits:       ${bitLength}
     *   - in bytes:      ${bitLength / 8} B
     *   - in KibiBytes:  ${Math.ceil(bitLength / 8 / 1024)} KiB
     */
    static constexpr std::array<std::${this.typeString}_t, ${this.length}ULL> ${this.name}{
    };
        `;
  }
}

const processCachedFile =
    async fileContent => {
  const lines = fileContent.split('\n');

  const cccsTables = new CCCTables();
  let lastCodePoint = 0;
  let count = 0;
  // const magicBucket = new MagicBucket();
  lines.forEach((line, index) => {
    line = cleanComments(line)

    // ignore empty lines
    if (line.length === 0) {
      return "";
    }

    const [codePointStr, codePointName, GeneralCategory, CanonicalCombiningClass, BidiClass,
           DecompositionType,
           // DecompositionMapping,
           NumericType,
           // NumericValue,
           BidiMirrored, Unicode1Name, ISOComment, SimpleUppercaseMapping, SimpleLowercaseMapping,
           SimpleTitlecaseMapping] = splitLine(line);
    const codePoint = parseCodePoints(codePointStr);
    const ccc = parseInt(CanonicalCombiningClass);

    /// stats
    ++count;
    if (count % 1000 === 0) {
      console.log(`Processing: `, `codePoint(${codePoint})`, `count(${count})`);
    }

    for (let curCodePoint = lastCodePoint + 1; curCodePoint !== codePoint + 1; ++curCodePoint) {
      const curCCC = curCodePoint === codePoint ? ccc : 0;

      cccsTables.add(curCodePoint, curCCC);

      // if (cccs[curCCC] === undefined) {
      //     cccs[curCCC] = [];
      // }
      // const accum = cccs[curCCC][cccs[curCCC].length - 1];
      // if ((accum?.end || accum) === curCodePoint - 1) {
      //     if (accum instanceof range) {
      //         accum.end = curCodePoint;
      //     } else {
      //         cccs[curCCC][cccs[curCCC].length - 1] = new range(accum, curCodePoint);
      //     }
      // } else {
      //     cccs[curCCC].push(curCodePoint);
      // }
    }
    lastCodePoint = codePoint;

    // console.log(index, codePoint, `ccc(${CanonicalCombiningClass})`);
  });
  console.log("Code Point Count:", count);

  // magicBucket.print();

  cccsTables.finalize?.();
  cccsTables.simplifyTrailing?.();

  // await createTableFile([refTable, mapTable]);

  console.log('File processing completed.');
}

const decorateTable = async table => {
  const bitLength = table.length * table.sizeof;
  console.log("Decorating table", table.name, " Length:", table.length);
  console.log(`Tables size,`);
  console.log(`  in bits: ${bitLength},`);
  console.log(`  in bytes: ${bitLength / 8},`);
  console.log(`  in KibiBytes: ${Math.ceil(bitLength / 8 / 1024)} KiB\n`);

  const header = `

    /**
     * ${table.description}
     * 
     * Table size:
     *   - in bits:       ${bitLength}
     *   - in bytes:      ${bitLength / 8} B
     *   - in KibiBytes:  ${Math.ceil(bitLength / 8 / 1024)} KiB
     */
    static constexpr std::array<std::${table.typeString}_t, ${table.length}ULL> ${table.name}{
  `;
  const footer = `
    };
  `;

  // header
  await fs.appendFile(outFilePath, header);

  // content
  let content = "";
  table.serializeTable(line => content += line);
  await fs.appendFile(outFilePath, content);

  // footer
  await fs.appendFile(outFilePath, footer);
};

const createTableFile = async (tables) => {
  const begContent = `
/**
 * Attention: Auto-generated file, don't modify.
 * 
 *   Auto generated from:          ${path.basename(__filename)}
 *   IDNA Creation Date:           ${readmeData.date}
 *   This file's generation date:  ${new Date().toUTCString()}
 *   IDNA Mapping Table Version:   ${readmeData.version}
 *
 * Details about the contents of this file can be found here:
 *   UTS #15: https://www.unicode.org/reports/tr15/
 *   UTS #44: https://www.unicode.org/reports/tr44/#UnicodeData.txt
 *   IDN FAQ: https://www.unicode.org/faq/idn.html
 */
 
#ifndef WEBPP_UNICODE_NORMALIZATION_TABLES_HPP
#define WEBPP_UNICODE_NORMALIZATION_TABLES_HPP

#include <array>
#include <cstdint>

namespace webpp::unicode::details {

`;

  const endContent = `
} // webpp::unicode::details

#endif // WEBPP_UNICODE_NORMALIZATION_TABLES_HPP
    `;

  await fs.writeFile(outFilePath, begContent);
  let bitLength = 0;
  let simplifiedCount = 0;
  let bitsSaved = 0;
  let mappedTable = undefined;
  let trailingsRemoved = 0;
  for (const table of tables) {
    bitLength += table.length * table.sizeof;
    trailingsRemoved += table?.trailingsRemoved || 0;
    if (table.simplifiedCount !== undefined) {
      mappedTable = table;
      simplifiedCount += table.simplifiedCount;
      bitsSaved += table.simplifiedBits;
      bitsSaved += table?.sequencedMappingCount * table?.sizeof * 2;
    }
    await decorateTable(table);
  }
  console.log(`Total tables size,`);
  console.log(`  in bits: ${bitLength},`);
  console.log(`  in bytes: ${bitLength / 8},`);
  console.log(`  in KibiBytes: ${Math.ceil(bitLength / 8 / 1024)} KiB\n`);
  console.log(`  Simplified counts: ${simplifiedCount}`);
  console.log(`  Bits Saved: ${bitsSaved}`);
  console.log(`  Bytes Saved: ${bitsSaved / 8}`);
  console.log(`  KibiBytes Saved: ${Math.ceil(bitsSaved / 8 / 1024)} KiB\n`);
  console.log(`  Mapped count: ${mappedTable.mappedCount}`);
  console.log(`  Ignored count: ${mappedTable.ignoredCount}`);
  console.log(`  Disallowed count: ${mappedTable.disallowedCount}`);
  console.log(`  Sequenced Mapping count: ${mappedTable.sequencedMappingCount}`);
  console.log(`  Trailings Removed: ${trailingsRemoved} Bytes`);
  console.log(`  Trailings Removed: ${Math.ceil(trailingsRemoved / 1024)} KiB`);
  await fs.appendFile(outFilePath, endContent);

  // Reformat the file
  require('child_process').exec(`clang-format -i "${outFilePath}"`, err => {
    if (err) {
      console.error("Could not re-format the file.", err);
    } else {
      console.log("Clang-format completed.");
    }
  });
};

start();
