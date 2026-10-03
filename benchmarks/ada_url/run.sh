#!/usr/bin/env bash
# Build and run every ada_url benchmark executable, writing one JSON file per ada ref.
#
# Usage:
#   ./run.sh                # build + run everything (results/<ref>.json)
#   ./run.sh --no-build     # run the existing executables only
#   ./run.sh --quick        # 1 repetition, 0.1s per benchmark (smoke run)
#
# Environment:
#   REPS       (default 3)      --benchmark_repetitions
#   MIN_TIME   (default 0.5s)   --benchmark_min_time
#   BUILD_DIR  (default <repo>/build)
#   JOBS       (default nproc)
#
# The first executable runs the full suite (webpp + ada); the remaining ones only run
# the ada rows (--benchmark_filter), since webpp numbers are identical in every binary.
# Merge everything with:
#   node tools/merge-benchmark-results.mjs benchmarks/ada_url/results

set -euo pipefail
cd "$(dirname "$0")"

repo_root="$(cd ../.. && pwd)"
build_dir="${BUILD_DIR:-$repo_root/build}"
reps="${REPS:-3}"
min_time="${MIN_TIME:-0.5s}"
jobs="${JOBS:-$(nproc)}"

no_build=0
quick=0
for arg in "$@"; do
    case "$arg" in
        --no-build) no_build=1 ;;
        --quick) quick=1 ;;
        *)
            echo "unknown argument: $arg" >&2
            exit 2
            ;;
    esac
done
if [ "$quick" -eq 1 ]; then
    reps=1
    min_time=0.1s
fi

trim() {
    local s="$1"
    s="${s%%#*}"
    s="${s#"${s%%[![:space:]]*}"}"
    s="${s%"${s##*[![:space:]]}"}"
    printf '%s' "$s"
}

ids=()
while IFS= read -r line || [ -n "$line" ]; do
    ref="$(trim "$line")"
    [ -z "$ref" ] && continue
    ids+=("$(printf '%s' "$ref" | tr -c 'A-Za-z0-9' '_')")
done < versions.txt

if [ "$no_build" -eq 0 ]; then
    echo "== building: ${ids[*]}"
    cmake --build "$build_dir" --target $(printf 'url-bench-%s ' "${ids[@]}") -j "$jobs"
fi

mkdir -p results
first=1
for id in "${ids[@]}"; do
    exe="$build_dir/url-bench-$id"
    if [ ! -x "$exe" ]; then
        echo "missing executable: $exe (run without --no-build)" >&2
        exit 1
    fi
    args=(
        --benchmark_out="results/$id.json"
        --benchmark_out_format=json
        --benchmark_repetitions="$reps"
        --benchmark_report_aggregates_only=true
        --benchmark_min_time="$min_time"
    )
    if [ "$first" -eq 0 ]; then
        args+=(--benchmark_filter='.*/ada/.*')
    fi
    echo "== running url-bench-$id -> results/$id.json"
    "$exe" "${args[@]}"
    first=0
done

echo
echo "results/ written. Merge with:"
echo "  node $repo_root/tools/merge-benchmark-results.mjs benchmarks/ada_url/results"
