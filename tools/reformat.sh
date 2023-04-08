#!/usr/bin/env bash
# shellcheck disable=SC2086
# shellcheck disable=SC2068

if [ -f /usr/bin/clang-format ]; then
    formatter="/usr/bin/clang-format"
else
    formatter="/usr/bin/env clang-format"
fi

formatter="$formatter -i --verbose --Wno-error=unknown"

function reformat {
    git ls-files $@ | grep -E "\.(hpp|cpp)$" | xargs $formatter
}

function reformat_staged {
    git --no-pager diff --name-only --staged | grep -E "\.(hpp|cpp)$" | xargs $formatter
}

function reformat_pre_commit {
    files=$(git --no-pager diff-index --cached --name-only HEAD | grep -E "\.(hpp|cpp)$")
    if [ -n "$files" ]; then
        echo "$files" | xargs $formatter
        echo "$files" | xargs -l git add
    else
        echo "Reformatting is not required."
    fi
}

function auto_reformat {
    git clang-format
}

case $1 in
    --auto)
        auto_reformat
        ;;
    --staged)
        shift
        reformat_staged
        ;;
    --pre-commit)
        shift
        reformat_pre_commit
        ;;
    *)
        reformat $@
        ;;
esac
