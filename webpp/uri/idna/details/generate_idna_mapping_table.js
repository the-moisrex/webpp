/***
 * This file downloads IdnaMappingTable.txt, and generates a C++ header file.
 *
 * Details on parsing this file can be found here:
 * UTS #46: https://www.unicode.org/reports/tr46/#IDNA_Mapping_Table
 */

const fs = require('fs').promises;
const path = require('path');
const fileUrl =
    'https://www.unicode.org/Public/idna/latest/IdnaMappingTable.txt';
const cacheFilePath = 'IdnaMappingTable.txt';
const outFilePath = `idna_mapping_table.hpp`;

const uint8 = Symbol('uint8');
const uint32 = Symbol('uint32');

const start = async () => {
  try {
    // Check if the file already exists in the cache
    if (!await fs.access(cacheFilePath)) {
      console.log('Using cached file...');
      const fileContent = await fs.readFile(cacheFilePath);
      processCachedFile(fileContent.toString());
      return;
    } else {
      console.log("No cached file exists, let's download it.")
    }

    const response = await fetch(fileUrl);

    if (!response.ok) {
      console.error(`Failed to download file. Status Code: ${response.status}`);
      return;
    }

    const text = await response.text();

    // Save the downloaded file as a cache
    await fs.writeFile(cacheFilePath, text);

    // process the file
    await processCachedFile(text);
  } catch (error) {
    console.error('Error:', error.message);
  }
};

const cleanComments = line => line.split('#')[0].trimEnd()
const splitLine = line => line.split(';').map(seg => seg.trim());
const findVersion = fileContent =>
    fileContent.match(/Version: (\d+\.\d+\.\d+)/)[1];
const findDate = fileContent => fileContent.match(/Date: ([^\n\r]+)/)[1];
const parseCodePoints = codePoints => {
  let [rangeStart, rangeEnd] =
      codePoints.split('..').map(codePoint => parseInt(codePoint, 16));
  rangeEnd = rangeEnd || rangeStart;
  return [ rangeStart, rangeEnd ];
};
const parseMappedCodePoints = codePoints =>
    codePoints.split(" ").map(codePoint => parseInt(codePoint, 16));
const mappingSanityCheck = (rangeStart, rangeEnd) => {
  const length = rangeEnd - rangeStart;
  if (length > 127) { // We only have 7 bits to store a length
    throw new Error(
        `We only have 7bits to store a length, we found a range with the length of ${
            length}; starts with ${rangeStart} and ends with ${rangeEnd}.`);
  }
};

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
}

/**
 * Mapping Reference Table
 *
 * This table will let us see if each character is mapped/disallowed/ignored/...
 * or not.
 */
class MappingReferenceTable extends TableTraits {
  constructor(max, type = uint8) {
    super(max, type);
    this.name = "idna_reference_table";
    this.description = "IDNA Mapping Reference Table"
  }

  append(start, end, isMapped = true) {
    for (; start !== end; ++start) {
      const arrayIndex = Math.floor(this.index / this.sizeof);
      const byteIndex = this.index % this.sizeof;
      const mask = (isMapped ? 0x1 : 0x0) << byteIndex;

      // activating the bit:
      this.bytes[arrayIndex] = this.bytes[arrayIndex] | mask;

      ++this.index;
    }
  }

  /// Get the length of the table
  get length() { return Math.ceil(this.index / this.sizeof); }

  /// Get how many bits are in the whole table
  get bitLength() { return this.index; }

  serializeTable(appendFunc, cols = 20 - this.sizeof) {
    let pos = 0;
    const postfix = this.postfix;
    for (; pos !== this.length;) {
      appendFunc(`${this.bytes[pos]}${postfix}, `);
      ++pos;
      if (pos % cols === 0) {
        appendFunc('\n');
      }
    }
  }
}

/**
 * A table for list of mapped characters
 *
 * First Byte Rules:
 *   - [1bit = 1] + [7bit = length] + [24bit = start]
 *   - start  = is the start of the range
 *   - length = the length of the range
 *   - (byte & 0x80000000 == 0x80000000) meaning far left bit is 0b1
 *   - if it starts with 0xFF000000, then it's a disabled range
 *
 * Bytes after the First Byte:
 *   - Their far left bit will never be 0b1,
 *     that means (byte & 0x80000000 == 0b0)
 *   - You have to continue reading everything after each byte, until
 *     you reach an element that it's far left bit is one.
 *
 * Actions:
 *   - Mapped: [[1bit = 1] [7bits = length] [24bits = range-start]]
 *             + ... N number of characters you should map to ...;
 *
 *             it's first element starts with a 0x80000000, and
 *             anything after that is considered as what you need to
 *             map the range to.
 *
 *   - Ignored: It's equivalent of mapping to empty string
 *
 *   - Disallowed: [[8bits = 1] [24bits = range-start]]
 *                +[ [32bits = range-end] ]
 *             Disallowed is really contains range start and range end.
 */
class MapTable extends TableTraits {

  e_disallowed = Symbol("Disallowed");
  e_ignored = Symbol("Ignored");
  e_mapped = Symbol("Mapped");
  e_none = Symbol("None");

  disallowedMask = 0xFF000000;
  mappedMask = 0x80000000;
  lengthLimit = 126; // We have 7 bits, but 0xFF would equal to disallowedMask
  endingCodePoint = 0xFFFFFFFF; // this.disallowedMask | 0x00FFFFFF;

  constructor(max) {
    super(max, uint32);
    this.name = "idna_mapping_table";
    this.description = "IDNA Mapping Table";
    this.prevAction = this.e_none;
    this.prevLength = 0;
    this.simplifiedCount = 0;
    this.simplifiedBits = 0;
    this.prevRangeEnd = 0;
    this.mappedCount = 0;
    this.disallowedCount = 0;
    this.ignoredCount = 0;
  }

  simplify(start, end, action) {
    const isSimplified = (() => {
      if (this.prevAction !== action || this.prevRangeEnd !== (start - 1)) {
        return false;
      }
      switch (action) {
      case this.e_disallowed:
        this.bytes[this.index - 1] = end;
        return true;
      case this.e_mapped:
        // todo: next character, maps to the next mapped character, optimization
      }
      return false;
    })();
    this.prevAction = action;
    this.prevLength = (end - start) + this.prevLength;
    this.prevRangeEnd = end;
    if (isSimplified) {
      ++this.simplifiedCount;
      this.simplifiedBits += this.savesIfSimplified(action);
      console.log(`Simplified (${action.description}): ${start}-${
          end} (count: ${end - start})`)
    }
    return isSimplified;
  }

  savesIfSimplified(action) {
    switch (action) {
    case this.e_none:
      return 0;
    case this.e_mapped:
      return 0;
    case this.e_ignored:
      return this.sizeof;
    case this.e_disallowed:
      return 2 * this.sizeof;
    }
    return 0;
  }

  /// Split lengths that are bigger than 127 because
  /// we only have 7bits of storage for the "length"
  ///
  /// This only happens once for the `ignored` as of unicode 15.1.0
  splitIfNeeded(start, end, mappedTo) {
    const length = end - start;
    if (length > this.lengthLimit) {
      console.log(`Splitting block: ${start}-${end}`);
      let page = start
      for (; page < end; page += this.lengthLimit + 1) {
        console.log(`Splitting: ${page}-${page + this.lengthLimit}`);
        this.map(page, page + this.lengthLimit, mappedTo);
      }
      this.map(page - this.lengthLimit, end, mappedTo);
      console.log(`Splitting: ${page - this.lengthLimit}-${end}`);
      return true;
    }
    return false;
  }

  /// Map this range of characters
  map(start, end, mappedTo = []) {
    if (this.splitIfNeeded(start, end, mappedTo)) {
      return;
    }
    mappingSanityCheck(start, end);
    if (this.simplify(start, end, this.e_mapped)) {
      return;
    }

    if (mappedTo.length === 0) {
      ++this.ignoredCount;
    } else {
      ++this.mappedCount;
    }

    const length = end - start;
    let byte = start;

    // Adding length to the byte
    byte |= length << 24;

    // Enabling far left bit to show that this is a "First Byte"
    byte |= this.mappedMask;

    this.bytes[this.index] = byte;
    ++this.index;

    /// Appending the mappedTo bytes
    for (const char of mappedTo) {
      this.bytes[this.index] = char;
      ++this.index;
    }
  }

  /// Ignore this range of characters (remove them)
  ignore(start, end) {
    this.map(start, end); // map to empty range
  }

  /// Disallow this range of characters
  disallow(start, end) {
    if (this.simplify(start, end, this.e_disallowed)) {
      return;
    }

    ++this.disallowedCount;

    let byte = start;

    // Making it a "First Byte" and make sure it's marked as "disallowed"
    byte |= this.disallowedMask;

    this.bytes[this.index] = byte;
    ++this.index;
    this.bytes[this.index] = end;
    ++this.index;
  }

  /// Call this method when you're done with adding things to the table.
  finish() {
    // add a valid first-byte at the end for simplifying the algorithm, so we
    // won't have to check the length of the array while performing mapping.
    this.bytes[this.index++] = this.endingCodePoint;
  }

  get length() { return this.index; }

  serializeTable(appendFunc, cols = 20 - this.sizeof) {
    let pos = 0;
    const postfix = this.postfix;
    for (; pos !== this.length;) {
      const byte = this.bytes[pos];
      const is_first_byte = (byte >>> 31) === 0b1;
      const is_disallowed = (byte >>> 24) === (this.disallowedMask >>> 24);
      appendFunc(`${byte}${postfix} `);
      if (byte === this.endingCodePoint) {
        appendFunc('/* Ending Code Point */');
      } else if (is_disallowed) {
        appendFunc('/* Disallowed */');
      } else if (is_first_byte) {
        if ((pos + 1 !== this.length) && (this.bytes[pos + 1] >>> 31) === 0b1) {
          appendFunc('/* Ignored */');
        } else {
          appendFunc('/* Mapped */');
        }
      }
      appendFunc(`, `);
      ++pos;
      if (pos % cols === 0) {
        appendFunc('\n');
      }
    }
  }
}

class STD3Mapper extends TableTraits {
  constructor(max, type = uint8) { super(max, type) }

  append(start, end, isMapped = false) {}
}

const processCachedFile =
    async fileContent => {
  const lines = fileContent.split('\n');

  const version = findVersion(fileContent);
  const creationDate = findDate(fileContent);
  if (version === undefined) {
    console.error("Could not find the version from the file content.");
    return;
  }
  if (creationDate === undefined) {
    console.error("No date was found.");
    return;
  }
  console.log(`Version: ${version}`);
  console.log(`Creation Date: ${creationDate}`);

  const refTable = new MappingReferenceTable(200000);
  const STD3Table = new STD3Mapper(1000);
  const mapTable = new MapTable(100000);
  lines.forEach((line, index) => {
    line = cleanComments(line)

    // ignore empty lines
    if (line.length === 0) {
      return "";
    }

    const [codePoints, status, mapping, IDNA2008Status] = splitLine(line);
    const [rangeStart, rangeEnd] = parseCodePoints(codePoints);
    const mappedValues = mapping ? parseMappedCodePoints(mapping) : undefined;

    switch (status) {
    case 'disallowed_STD3_valid':
      STD3Table.append(rangeStart, rangeEnd, false);
      refTable.append(rangeStart, rangeEnd, true);
      break;
    case 'deviation': // https://www.unicode.org/reports/tr46/#Deviations
    // Deviations are considered valid in IDNA2008 and UTS #46.
    case 'valid':
      refTable.append(rangeStart, rangeEnd, false);
      break;
    case 'disallowed_STD3_mapped':
      STD3Table.append(rangeStart, rangeEnd, true);
      refTable.append(rangeStart, rangeEnd, true);
      mapTable.map(rangeStart, rangeEnd, mappedValues);
      break;
    case 'mapped':
      refTable.append(rangeStart, rangeEnd, true);
      mapTable.map(rangeStart, rangeEnd, mappedValues);
      break;
    case 'ignored':
      refTable.append(rangeStart, rangeEnd, true);
      mapTable.ignore(rangeStart, rangeEnd);
      break;
    case 'disallowed':
      refTable.append(rangeStart, rangeEnd, true);
      mapTable.disallow(rangeStart, rangeEnd);
      break;
    default:
      console.error(`Invalid 'status' found: ${status}; line: ${line}`);
      process.exit(1);
      return;
    }

    // Process each line here
    console.log(index, rangeStart, rangeEnd, status, mappedValues,
                IDNA2008Status);
    return `${codePoints}`;
  });

  refTable.finish?.();
  mapTable.finish?.();

  await createTableFile(version, creationDate, [ refTable, mapTable ]);

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
    static constexpr std::array<std::${table.typeString}_t, ${
      table.length}ULL> ${table.name}{
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

const createTableFile = async (version, creationDate, tables) => {
  const begContent = `
/**
 * Attention: Auto-generated file, don't modify.
 * 
 *   Auto generated from:          ${path.basename(__filename)}
 *   IDNA Creation Date:           ${creationDate}
 *   This file's generation date:  ${new Date().toUTCString()}
 *   IDNA Mapping Table Version:   ${version}
 *
 * Details about the contents of this file can be found here:
 *   UTS #46: https://www.unicode.org/reports/tr46/#IDNA_Mapping_Table
 *   IDN FAQ: https://www.unicode.org/faq/idn.html
 */
 
#ifndef WEBPP_URI_IDNA_MAPPING_TABLE_HPP
#define WEBPP_URI_IDNA_MAPPING_TABLE_HPP

#include <array>
#include <cstdint>

namespace webpp::uri::idna::details {

`;

  const endContent = `
} // webpp::uri::idna::details

#endif // WEBPP_URI_IDNA_MAPPING_TABLE_HPP
    `;

  await fs.writeFile(outFilePath, begContent);
  let bitLength = 0;
  let simplifiedCount = 0;
  let bitsSaved = 0;
  let mappedTable = undefined;
  for (const table of tables) {
    bitLength += table.length * table.sizeof;
    if (table.simplifiedCount !== undefined) {
      mappedTable = table;
      simplifiedCount += table.simplifiedCount;
      bitsSaved += table.simplifiedBits;
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