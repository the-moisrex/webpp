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

  const table = lines.map((line, index) => {
    line = cleanComments(line)

    // ignore empty lines
    if (line.length === 0) {
      return "";
    }

    const [codePoints, status, mapping, IDNA2008Status] = splitLine(line);

    switch (status) {
    case 'valid':
    case 'mapped':
    case 'disallowed_STD3_valid':
    case 'disallowed_STD3_mapped':
    case 'ignored':
    case 'disallowed':
    case 'deviation':
      break;
    default:
      console.error(`Invalid 'status' found: ${status}; line: ${line}`);
      process.exit(1);
      return;
    }

    // Process each line here
    console.log(index, codePoints, status, mapping, IDNA2008Status);
    return `${codePoints}`;
  });

  createTableFile(version, creationDate, table);

  console.log('File processing complete.');
}

const createTableFile =
    async (version, creationDate, table) => {
  const fileContent = `
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

namespace webpp::uri::idna::details {
    
} // webpp::uri::idna::details

#endif // WEBPP_URI_IDNA_MAPPING_TABLE_HPP
    `;

  await fs.writeFile(outFilePath, fileContent);
}

start();