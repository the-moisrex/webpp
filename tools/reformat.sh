#!/usr/bin/env bash

if [ -f /usr/bin/clang-format ]; then
    formatter="/usr/bin/clang-format";
else
    formatter="/usr/bin/env clang-format";
fi

git ls-files $@ | grep -E "\.(hpp|cpp)$"  | xargs "$formatter" -i --verbose --Wno-error=unknown
