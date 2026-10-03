#!/usr/bin/env node
// Merge the per-ada-ref JSON files produced by benchmarks/ada_url/run.sh into
// markdown tables (one table per benchmark piece).
//
// Usage:
//   node tools/merge-benchmark-results.mjs [results-dir] [--out FILE.md]
//
// Defaults to benchmarks/ada_url/results.
//
// Naming contract: benchmark names are "piece/impl/config/input", optionally
// suffixed by an aggregate ("/median" or the older "_median"). The ada ref comes
// from the file name (<ref>.json), giving one column set per ref. webpp rows are
// identical in every file, so only the first file's webpp rows are kept.

import { readdirSync, readFileSync, writeFileSync } from 'node:fs';
import { basename, join } from 'node:path';
import process from 'node:process';

const args = process.argv.slice(2);
let resultsDir = 'benchmarks/ada_url/results';
let outFile = null;
for (let i = 0; i < args.length; i++) {
  if (args[i] === '--out' && args[i + 1]) {
    outFile = args[++i];
  } else if (!args[i].startsWith('--')) {
    resultsDir = args[i];
  }
}

const unitToNs = { ns: 1, us: 1e3, µs: 1e3, ms: 1e6, s: 1e9 };
const AGGREGATE = /\/(mean|median|stddev|cv)$|_(mean|median|stddev|cv)$/;

function median(values) {
  const sorted = [...values].sort((a, b) => a - b);
  const mid = sorted.length >> 1;
  return sorted.length % 2 ? sorted[mid] : (sorted[mid - 1] + sorted[mid]) / 2;
}

function parseFile(file, ref) {
  const json = JSON.parse(readFileSync(file, 'utf8'));
  // name -> list of samples (ns); aggregates get their own pseudo-sample names
  const perName = new Map();
  for (const bench of json.benchmarks ?? []) {
    const scale = unitToNs[bench.time_unit] ?? 1;
    const value = bench.real_time * scale;
    const list = perName.get(bench.name) ?? [];
    list.push(value);
    perName.set(bench.name, list);
  }

  const cells = new Map(); // piece -> input -> column -> ns

  // first pass: aggregate preference per base name (median > mean > computed)
  const chosen = new Map(); // baseName -> ns
  for (const [name, samples] of perName) {
    const match = name.match(AGGREGATE);
    if (match) {
      const base = name.replace(AGGREGATE, '');
      const kind = match[1] ?? match[2];
      if (kind === 'median' || (kind === 'mean' && !chosen.has(base))) {
        chosen.set(base, samples[0]);
      }
    }
  }
  for (const [name, samples] of perName) {
    if (AGGREGATE.test(name)) continue;
    if (!chosen.has(name)) chosen.set(name, median(samples));
  }

  for (const [name, ns] of chosen) {
    const parts = name.split('/');
    if (parts.length < 4) continue;
    const [piece, impl, config, input] = parts;
    const column = impl === 'webpp' ? `webpp/${config}` : `ada@${ref}/${config}`;
    const perInput = cells.get(piece) ?? new Map();
    const perColumn = perInput.get(input) ?? new Map();
    perColumn.set(column, ns);
    perInput.set(input, perColumn);
    cells.set(piece, perInput);
  }

  return cells;
}

function formatNs(ns) {
  if (ns >= 1e6) return `${(ns / 1e6).toFixed(2)} ms`;
  if (ns >= 1e3) return `${(ns / 1e3).toFixed(2)} µs`;
  return `${ns.toFixed(1)} ns`;
}

function main() {
  let files;
  try {
    files = readdirSync(resultsDir)
      .filter((f) => f.endsWith('.json'))
      .sort();
  } catch (err) {
    console.error(`cannot read ${resultsDir}: ${err.message}`);
    process.exit(1);
  }
  if (files.length === 0) {
    console.error(`no *.json results in ${resultsDir}`);
    process.exit(1);
  }

  const merged = new Map(); // piece -> input -> column -> ns
  files.forEach((file, index) => {
    const ref = basename(file, '.json');
    const cells = parseFile(join(resultsDir, file), ref);
    for (const [piece, perInput] of cells) {
      const outInputs = merged.get(piece) ?? new Map();
      for (const [input, perColumn] of perInput) {
        const outColumns = outInputs.get(input) ?? new Map();
        for (const [column, ns] of perColumn) {
          // webpp rows repeat in every file; keep them from the first file only
          if (column.startsWith('webpp/') && index > 0 && outColumns.has(column)) continue;
          outColumns.set(column, ns);
        }
        outInputs.set(input, outColumns);
      }
      merged.set(piece, outInputs);
    }
  });

  const lines = [];
  lines.push(`# webpp vs ada — merged benchmark results`);
  lines.push('');
  lines.push(`Sources: ${files.map((f) => basename(f)).join(', ')}`);
  lines.push('');
  lines.push('Values are the median run time per iteration. Lower is better.');
  lines.push('');

  for (const [piece, perInput] of [...merged].sort(([a], [b]) => a.localeCompare(b))) {
    const columns = new Set();
    for (const perColumn of perInput.values()) {
      for (const column of perColumn.keys()) columns.add(column);
    }
    const sortedColumns = [...columns].sort((a, b) => {
      const aWebpp = a.startsWith('webpp/');
      const bWebpp = b.startsWith('webpp/');
      if (aWebpp !== bWebpp) return aWebpp ? -1 : 1;
      return a.localeCompare(b);
    });

    lines.push(`## ${piece}`);
    lines.push('');
    lines.push(`| input | ${sortedColumns.join(' | ')} |`);
    lines.push(`| --- | ${sortedColumns.map(() => '---:').join(' | ')} |`);
    for (const [input, perColumn] of [...perInput].sort(([a], [b]) => a.localeCompare(b))) {
      const row = sortedColumns.map((column) => {
        const ns = perColumn.get(column);
        return ns === undefined ? '' : formatNs(ns);
      });
      lines.push(`| ${input} | ${row.join(' | ')} |`);
    }
    lines.push('');
  }

  const text = `${lines.join('\n')}\n`;
  if (outFile) {
    writeFileSync(outFile, text);
    console.error(`wrote ${outFile}`);
  } else {
    process.stdout.write(text);
  }
}

main();
