/***
 * This file downloads IdnaMappingTable.txt, and generates a C++ header file.
 *
 * Details on parsing this file can be found here:
 * UTS #46: https://www.unicode.org/reports/tr46/#IDNA_Mapping_Table
 */

const fs = require('fs').promises;
const fileUrl =
    'https://www.unicode.org/Public/idna/latest/IdnaMappingTable.txt';

const start = async () => {
  const cacheFilePath = 'IdnaMappingTable.txt';

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

function processCachedFile(fileContent) {
  const lines = fileContent.split('\n');

  lines.forEach(line => {
    line = cleanComments(line)
    const [codePoints, status, mapping, IDNA2008Status] = splitLine(line);

    // Process each line here
    console.log(codePoints, status, mapping, IDNA2008Status);
  });

  console.log('File processing complete.');
}

start();