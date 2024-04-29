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
const uint8x2 = Symbol('uint8');
const uint32 = Symbol('uint32');

const readmeData = {
  version : "",
  date : ""
};

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
};

const start = async () => {
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

const processReadmeFile = content => {
  readmeData.version = findVersion(content);
  readmeData.date = findDate(content);
};

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

  get result() { return this.bytes.slice(0, this.index); }

  get(index) { return this.bytes[index]; }

  append(value) { this.bytes[this.index++] = value; }

  appendList(list) {
    for (const value of list) {
      this.append(value);
    }
  }
}

class CCCTables {
  constructor() {
    this.lastZero = 0;

    // these numbers are educated guesses from other projects, they're not that important!
    this.indeces = new TableTraits(4353 * 10, uint32);
    this.cccs = new TableTraits((100 + 67) * 256, uint8);

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
    const findSimilarRange = (index = 0, length = 256, mask = resetMask) => {
      const end = index + length;
      const range = this.data.slice(index, end);
      const batchEnd = (this.cccs.length / length) * length;

      top: for (let pos = 0; pos < batchEnd; ++pos) { // iterate over this.cccs table

        // see if we from this "this.cccs"'s position, the "range" is also the same or not:
        for (let dit = 0; dit !== range.length; ++dit) { // iterate over "range" table
          const {codePoint : rangeCodePoint, ccc : rangeCCC} = range[dit];
          const ccc = this.cccs.get(pos + (dit & mask));

          // if it's not a match
          if (rangeCCC !== ccc) {
            // couldn't find it here, let's go to the next position
            continue top;
          }
        }

        // found a range
        return {pos, inserts : []};
      }

      // tail optimization:
      //    if the "this.cccs" table's tail has a match for the beginning of the "range" table,
      //    then we can omit inserting the first part of the "range" table.
      for (let pos = range.length; pos !== 0; --pos) {         // iterate over "this.cccs" table
        for (let dit = 0; dit !== range.length - pos; ++dit) { // iterate over "range" table
          const {codePoint : rangeCodePoint, ccc : rangeCCC} = range[dit];
          const ccc = this.cccs.get(pos + dit);

          if (rangeCCC !== ccc) {
            // found a length
            const start = pos + dit;
            const end = start + length - dit; // we know it can be optimized, but it's for docs
            return {pos, inserts : range.slice(start, end).map(item => item.ccc)};
          }
        }
      }

      // didn't find anything, so let's insert everything:
      return {pos : null, inserts : range};
    };

    const findSimilarMaskedRange = (index = 0, length = 256) => {
      let mask = resetMask;
      let masks = [];
      for (; mask >= 0; --mask) {
        const {inserts, pos} = findSimilarRange(index, length, mask);
        masks.push({pos, mask, inserts});
      }
      return masks.toSorted((a, b) => b.inserts.length - a.inserts.length)?.[0];
    };

    let batchNo = 0;
    let insertedCount = 0;
    let reusedCount = 0;
    let reusedMaskedCount = 0;
    for (let range = 0; range <= this.data.length; range += 256) {

      const codeRange = range >>> 8;
      const length = Math.min(this.data.length - range, 256);

      console.log(`Batch #${batchNo++}: `, codeRange.toString(16), length,
                  `Progress: ${Math.floor(range / this.data.length * 100)}%`);

      const {pos, mask, inserts} = findSimilarMaskedRange(range, length);
      const helperCode = (pos << 8) | mask;
      this.indeces.append(helperCode);
      if (inserts.length > 0) {
        this.cccs.appendList(inserts);
        ++insertedCount;
      } else {
        ++reusedCount;
      }
      console.log(`  Code Range (${inserts.length ? "Inserted-" + inserts.length : "Reused"}):`, codeRange,
                  "mask:", mask, "pos:", pos);

      if (mask !== resetMask) {
        ++reusedMaskedCount;
      }
    }

    console.log("Inserted: ", insertedCount, "reused:", reusedCount);
    console.log("Successful masks:", reusedMaskedCount);
    console.log("CCCs Table Length:", this.cccs.length);
    console.log("Finalizing: done.");
  }

  render() {

    const indeces = this.indeces.result.slice(0, this.lastZero >>> 8);
    const indecesBits = indeces.length * this.indeces.sizeof;
    const cccBits = this.cccs.length * this.cccs.sizeof;

    return `
    /**
     * In "ccc_index" table, any code point bigger than this number will have "zero" as it's CCC value;
     * so it's designed this way to reduce the table size.
     */
    static constexpr auto trailing_zero_cccs = 0x${this.lastZero.toString(16).toUpperCase()}UL;
    
    /**
     * CCC Index Table
     *
     * CCC: Canonical Combining Class
     * These are the indeces that are used to find which values from "ccc_values" table correspond to a
     * unicode code point.
     *
     * Usage:
     * @code
     *    uint32_t code_point = ...;
     *    auto helper_code = ccc_index[code_point >> 8];
     *    auto pos         = helper_code >> 8;           // starting position
     *    auto mask        = helper_code &  0xFF;        // which positions should be converted to zero
     *    auto index       = pos         &  mask;        // now use ccc_values[index]
     * @endcode
     * 
     * Table size:
     *   - in bits:       ${indecesBits}
     *   - in bytes:      ${indecesBits / 8} B
     *   - in KibiBytes:  ${Math.ceil(indecesBits / 8 / 1024)} KiB
     */
    static constexpr std::array<std::${this.indeces.typeString}_t, ${indeces.length}ULL> ccc_index{
        ${indeces.join(", ")}
    };
    
    
    /**
     * CCC Table
     *
     * CCC: Canonical Combining Class
     * These values are calculated and individually represend actual CCC values, but they have no
     * valid order by themselves, and they only make sense if they're being used in conjuction with
     * the "ccc_index" table.
     * 
     * Table size:
     *   - in bits:       ${cccBits}
     *   - in bytes:      ${cccBits / 8} B
     *   - in KibiBytes:  ${Math.ceil(cccBits / 8 / 1024)} KiB
     */
    static constexpr std::array<std::${this.cccs.typeString}_t, ${this.cccs.length}ULL> ccc_values{
        ${this.cccs.result.join(", ")}
    };
        `;
  }
}

const processCachedFile = async fileContent => {
  const lines = fileContent.split('\n');

  const cccsTables = new CCCTables();
  let lastCodePoint = 0;
  let count = 0;
  lines.forEach((line, index) => {
    line = cleanComments(line);

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
      console.log(`Parsing: `, `codePoint(${codePoint})`, `count(${count})`);
    }

    for (let curCodePoint = lastCodePoint + 1; curCodePoint !== codePoint + 1; ++curCodePoint) {
      const curCCC = curCodePoint === codePoint ? ccc : 0;

      cccsTables.add(curCodePoint, curCCC);
    }
    lastCodePoint = codePoint;
  });
  console.log("Code Point Count:", count);

  // magicBucket.print();

  cccsTables.finalize?.();

  await createTableFile([ cccsTables ]);

  console.log('File processing completed.');
};

const decorateTable = async table => { await fs.appendFile(outFilePath, table.render()); };

const createTableFile = async (tables) => {
  const begContent = `
/**
 * Attention: Auto-generated file, don't modify.
 * 
 *   Auto generated from:                ${path.basename(__filename)}
 *   Unicode UCD Database Creation Date: ${readmeData.date}
 *   This file's generation date:        ${new Date().toUTCString()}
 *   Unicode Version:                    ${readmeData.version}
 *
 * Details about the contents of this file can be found here:
 *   UTS #15: https://www.unicode.org/reports/tr15/
 *   UTS #44: https://www.unicode.org/reports/tr44/#UnicodeData.txt
 *   IDN FAQ: https://www.unicode.org/faq/idn.html
 *
 *   UCD Database Code Points (used the get the CCC values and what not):
 *       ${fileUrl}
 *   UCD README file (used to check the version and creation date):
 *       ${readmeUrl}
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
  for (const table of tables) {
    await decorateTable(table);
  }
  // console.log(`  Trailings Removed: ${trailingsRemoved} Bytes`);
  // console.log(`  Trailings Removed: ${Math.ceil(trailingsRemoved / 1024)} KiB`);
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
