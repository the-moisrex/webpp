#!/bin/bash

if [ -f /usr/bin/clang-format ]; then
    echo "Reformating hpp/cpp files with clang-format.";
    git ls-files | grep -E "\.(hpp|cpp)$" | xargs /usr/bin/clang-format -i
else
    echo "/usr/bin/clang-format was not found.";
fi

# todo: reformat CMakePresets.json file as well

