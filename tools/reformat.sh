#!/usr/bin/env bash

if [ -f /usr/bin/clang-format ]; then
    formatter="/usr/bin/clang-format";
else
    formatter="/usr/bin/env clang-format";
fi

formatter="$formatter git --no-pager diff --name-only --staged"

function reformat {
  git ls-files $@ | grep -E "\.(hpp|cpp)$"  | xargs "$formatter"
}

function reformat_staged {
  git --no-pager diff --name-only --staged | grep -E "\.(hpp|cpp)$"  | xargs "$formatter"
}

case $1 in
  --staged)
    shift;
    reformat_staged
    ;;
  *)
    reformat;
    ;;
esac
