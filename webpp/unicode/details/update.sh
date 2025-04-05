#!/bin/bash

curdir="$(realpath "$(dirname "$0")")";
trash_cmd="rm -rf"

if ! command -v node >/dev/null; then
  echo "NodeJS is not installed." >&2;
  exit 1;
fi

if command -v trash >/dev/null; then
  trash_cmd="trash";
fi

function remove() {
  file="$1"
  if [ -f "$file" ]; then
    if $trash_cmd "$file"; then
      echo "Removed: $(basename "$file")";
    fi
  else
    echo "No file: $(basename "$file")";
  fi
}

function run() {
  file="$1"
  log_file="$curdir/$(basename "$file").log"

  if [ ! -f "$file" ]; then
    echo "File not found: $file" >&2;
    exit 1;
  fi

  if [ -f "$log_file" ]; then
    mv "$log_file" "$log_file.backup-${RANDOM}.log"
  fi

  echo "Running $(basename "$file")"
  echo "       Loging it into: $(basename "$log_file")"
  if ! node "$file" > "$log_file" 2>&1; then
    echo "Failed.";
    exit 1;
  fi
  echo
}


# ------------------------------------------------

remove "$curdir/DerivedBidiClass.txt"
remove "$curdir/ReadMe.txt"
remove "$curdir/UnicodeData.txt"
remove "$curdir/DerivedJoiningType.txt"

echo

run "$curdir/generate_bidi_tables.mjs"
run "$curdir/generate_ccc_tables.mjs"
run "$curdir/generate_composition_tables2.mjs"
run "$curdir/generate_decomposition_tables.mjs"
run "$curdir/generate_idna_mapping_tables2.mjs"
run "$curdir/generate_joiners_tables.mjs"
run "$curdir/generate_general_categories_tables.mjs"

echo
echo "Done."

exit 0;