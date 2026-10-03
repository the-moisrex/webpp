#!/usr/bin/env bash
# Maintains the local ada checkouts (git worktrees) used by the ada_url benchmark suite.
#
# Usage:
#   ./setup.sh            # create the missing checkouts for every ref in versions.txt
#   ./setup.sh --update   # fetch upstream and detach every checkout to the latest tip of its ref
#
# Environment:
#   ADA_REPO              # override the upstream repository URL
#
# The refs come from versions.txt (one git ref per line; '#' starts a comment).
# Checkout ids are the CMAKE_MAKE_IDENTIFIER equivalent of the ref ("v4.0.0" -> "v4_0_0"),
# computed with tr so that CMake derives the same id from the same file.

set -euo pipefail
cd "$(dirname "$0")"

repo_url="${ADA_REPO:-https://github.com/ada-url/ada.git}"
deps_dir=".deps"
bare="$deps_dir/ada.git"
update=0
[ "${1:-}" = "--update" ] && update=1

trim() {
    local s="$1"
    s="${s%%#*}"
    s="${s#"${s%%[![:space:]]*}"}"
    s="${s%"${s##*[![:space:]]}"}"
    printf '%s' "$s"
}

mkdir -p "$deps_dir"
if [ ! -d "$bare" ]; then
    echo "cloning: $repo_url -> $bare"
    git clone --bare "$repo_url" "$bare"
elif [ "$update" -eq 1 ]; then
    git --git-dir="$bare" fetch --tags origin
fi

# drop worktree bookkeeping for checkouts that no longer exist on disk
git --git-dir="$bare" worktree prune

while IFS= read -r line || [ -n "$line" ]; do
    ref="$(trim "$line")"
    [ -z "$ref" ] && continue
    id="$(printf '%s' "$ref" | tr -c 'A-Za-z0-9' '_')"
    dest="$deps_dir/ada-$id"
    if [ ! -e "$dest" ]; then
        git --git-dir="$bare" worktree add --detach "$dest" "$ref"
        echo "checkout: $dest <- $ref"
    elif [ "$update" -eq 1 ]; then
        git -C "$dest" checkout --detach "$ref"
        echo "updated:  $dest -> $ref"
    fi
done < versions.txt
