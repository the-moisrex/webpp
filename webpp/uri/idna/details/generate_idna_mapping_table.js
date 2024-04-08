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
    const res = fs.writeFile(cacheFilePath, text);

    // process the file
    processCachedFile(text);

    // wait for the file to be written:
    await res;
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

class codePointMapper {
  constructor(max, type = 'uint8') {
    switch (type) {
    case 'uint8':
      this.bytes = new Uint8Array(max);
      break;
    case 'uint32':
      this.bytes = new Uint32Array(max);
      break;
    default:
      throw new Error('Invalid type provided to CodePointMapper.');
    }
    this.type = type;
    this.index = 0;
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

  get sizeof() {
    switch (this.type) {
    case 'uint8':
      return 8;
    case 'uint32':
      return 32;
    default:
      return 0;
    }
  }

  /// Get how many bits are in the whole table
  get bitLength() { return this.index; }

  serializeTable(appendFunc, cols = 20 - this.sizeof) {
    let pos = 0;
    const postfix = this.type === "uint8" ? "U" : "ULL";
    for (; pos !== this.length;) {
      appendFunc(`${this.bytes[pos]}${postfix}, `);
      ++pos;
      if (pos % cols === 0) {
        appendFunc('\n');
      }
    }
  }
}

class STD3Mapper {
  constructor(max) {
    this.bytes = new Uint8Array(max);
    this.index = 0;
  }

  append(start, end, isMapped = false) {}
}

function processCachedFile(fileContent) {
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

  const table = new codePointMapper(200000);
  const STD3Table = new STD3Mapper(1000);
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
      table.append(rangeStart, rangeEnd, true);
      break;
    case 'deviation': // https://www.unicode.org/reports/tr46/#Deviations
    // Deviations are considered valid in IDNA2008 and UTS #46.
    case 'valid':
      table.append(rangeStart, rangeEnd, false);
      break;
    case 'disallowed_STD3_mapped':
      STD3Table.append(rangeStart, rangeEnd, true);
      table.append(rangeStart, rangeEnd, true);
      break;
    case 'mapped':
      table.append(rangeStart, rangeEnd, true);
      break;
    case 'ignored':
      table.append(rangeStart, rangeEnd, true);
      break;
    case 'disallowed':
      table.append(rangeStart, rangeEnd, true);
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

  console.log(`Table Length: ${table.length}`);
  console.log(`Table Bit Length: ${table.bitLength}`);

  createTableFile(version, creationDate, table);

  console.log('File processing complete.');
}

const createTableFile =
    async (version, creationDate, table) => {
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

    /**
     * IDNA Mapping Table
     */
    static constexpr std::array<std::${table.type}_t, ${
      table.length}ULL> idna_mapping_table{
`;

  const endContent = `
    };
} // webpp::uri::idna::details

#endif // WEBPP_URI_IDNA_MAPPING_TABLE_HPP
    `;

  await fs.writeFile(outFilePath, begContent);
  let content = "";
  table.serializeTable(line => content += line);
  await fs.appendFile(outFilePath, content);
  await fs.appendFile(outFilePath, endContent);

  // Reformat the file
  require('child_process').exec(`clang-format -i "${outFilePath}"`, err => {
    if (err) {
      console.error("Could not re-format the file.", err);
    }
  });
}

start();