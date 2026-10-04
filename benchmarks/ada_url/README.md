# webpp vs ada URL benchmarks

Comparison suite for [#437](https://github.com/webpp/webpp/issues/437): webpp's URI
parser measured against [ada-url](https://github.com/ada-url/ada) piece by piece,
across multiple ada versions checked out side by side. Only entry points that are
actually callable from outside each library are compared (see
[Comparability notes](#comparability-notes)).

## Layout

| File | Purpose |
| --- | --- |
| `versions.txt` | ada refs to benchmark, one per line (`main`, `v4.0.0`, `v2.9.2`) |
| `setup.sh` | idempotent: bare clone + detached worktree per ref under `.deps/` (gitignored) |
| `CMakeLists.txt` | per-ref `ExternalProject` (ada's own CMake, Release, install) + `url-bench-<id>` |
| `inputs.hpp` | corpora (slug + value), shared verbatim by both sides |
| `adapters/webpp_adapter.hpp` | the exact webpp entry points being measured |
| `adapters/ada_adapter.hpp` | the exact ada entry points being measured |
| `*_bench.cpp` | registration only; every benchmark name is `piece/impl/config/input` |
| `run.sh` | builds all executables and runs them, one JSON per ref under `results/` (gitignored) |
| `../../tools/merge-benchmark-results.mjs` | merges `results/*.json` into markdown tables |

`benchmarks/ada/` (the older copy-paste suite) is untouched and unrelated.

## Build and run

```sh
cmake --preset dev-default -DWEBPP_URL_COMPARE=ON
./benchmarks/ada_url/run.sh                 # build all refs + run (REPS=3, MIN_TIME=0.5s)
./benchmarks/ada_url/run.sh --quick         # smoke run: 1 rep, 0.1s per benchmark
./benchmarks/ada_url/run.sh --no-build      # run existing executables only
node tools/merge-benchmark-results.mjs benchmarks/ada_url/results   # markdown tables
```

The first configure runs `setup.sh` (network: clones ada into `.deps/`), then each ref
is built by `ExternalProject` using ada's own CMake (Release, `-DADA_TESTING=OFF`,
installed into a per-ref prefix). Meta target: `cmake --build build --target ada-url-all`.

## Naming contract

- Benchmark names are exactly four slash-separated parts: `piece/impl/config/input`
  (registered at runtime from `bench_common.hpp`).
- The ada ref is *not* part of the name; it comes from the results file name
  (`results/<ref-id>.json`), so one column set exists per ref.
- Every binary contains both webpp and ada rows; `run.sh` runs the full suite once
  (first ref) and filters to `.*/ada/.*` for the remaining refs, because the webpp
  numbers are identical in every binary.
- The merge script prefers the `_median`/`/median` aggregate rows (falling back to
  `_mean`, then to its own median of raw runs) and emits one table per piece with
  columns `webpp/<config>` and `ada@<ref>/<config>`.

## Flag parity

Both sides of every benchmark are compiled into the same executable with the same
suite flags:

- TUs (webpp code is header-only, so its code *is* TU code): `-O3 -DNDEBUG -ffast-math
  -ffunction-sections -fdata-sections`, plus `-march=native -mtune=native` with
  `NATIVE_ARCH`, linked with `-Wl,--gc-sections`.
- ada's static libraries are built by ada's own CMake in `Release` (its own default
  optimization flags); the code *called from* the benchmark TUs on the ada side is
  header code compiled with the same flags as webpp's.
- **No LTO on either side.** LTO would inline across library boundaries differently
  per ada version and turn the comparison into a whole-program-link experiment; the
  suite compares the libraries as users get them.
- The `dev-default` preset injects `_GLIBCXX_DEBUG`/`_GLIBCXX_SANITIZE_VECTOR` etc.
  globally; they land on both sides equally, but absolute times are therefore not
  comparable to a stripped release build.
- `-DWEBPP_URL_BENCH_SANITIZE=ON` (configure-time option) replaces the suite flags with
  `-O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined` on both sides, links
  with the sanitizers, and keeps asserts on. It is for finding memory bugs only:
  **never publish numbers from a sanitize build.**

## What is measured

- `full_parse` — whole-URL parse + href accessors: webpp `owning_standard`,
  `owning_strict`, `owning_loose`, `structured_standard` vs ada `url` and
  `url_aggregator`. webpp's `u32_view` config is excluded: it does not compile at
  current HEAD (the tests have it commented out for the same reason).
- `scheme`, `host`, `path`, `query`, `fragment`, `port`, `credentials`, `ipv4`,
  `ipv6` — per-piece rows: direct piece parser (webpp only) and the public setter
  (both sides).
- `domain_to_ascii` — webpp `domain_to_ascii` vs `ada::unicode::to_ascii`.
- `ipv4` extra configs — webpp `pton` (strict RFC `inet_pton4`) and `host_ipv4`
  (WHATWG host-IPv4 parser); ada `try_parse_ipv4_fast` (ada >= v4.0.0 only).
- `serialize_ipv4` / `serialize_ipv6` — webpp `inet_ntop4/6` vs `ada::serializers`.
- `percent_encode` / `percent_decode` — query percent-encode set on both sides.
- `build` — full public setter chain (user/password/host/path/query/fragment/port)
  applied per iteration on a URL parsed once outside the loop.
- `serialize_href` — href serialization: webpp `as_string()` vs `ada::url::get_href()`
  (both allocate) and `ada::url_aggregator::get_href()` (string_view, reference only).

## Comparability notes

Semantics were checked against the WHATWG URL Standard (`tools/whatwg-url-specs`)
and browser behavior; the rows are aligned so both sides do the same job:

- **webpp `path()` setter appends** to the existing path, while the WHATWG pathname
  setter empties it first. Every webpp path-setter row therefore calls
  `url.clear_path()` before assigning; ada's `set_pathname()` already replaces.
- **webpp `hostname()` grows the path**: under state override it keeps going into
  path state (the spec's host-state step 3.6 returns there) and appends one `/` per
  call. Host/ipv4/ipv6 setter rows call `url.clear_path()` after each assignment so
  the workload stays constant.
- **Scheme setter input**: webpp's scheme setter needs colon-terminated input
  (`"http:"`), while ada's `set_protocol()` appends the colon itself (the HTML
  protocol-setter rule). The corpus is `"http:"`, `"https:"`, `"ftp:"`, `"wss:"`
  (special → special, valid for both).
- **ada's per-piece parsers are not callable from outside libada**:
  `ada::checkers::is_ipv4`, `ada::helpers::parse_prepared_path`,
  `ada::url_aggregator::parse_path`/`parse_host` are declared `ada_really_inline`
  and their bodies live in ada's `.cpp` units, so GCC rejects any call from an
  external translation unit ("function body not available"). Direct piece rows are
  therefore webpp-only; ada columns exist for setters, whole-URL parsing,
  encoding/decoding, serialization, IDNA, and `try_parse_ipv4_fast` (whose body is
  in the installed `checkers-inl.h` and links fine).
- **`try_parse_ipv4_fast` exists since ada v4.0.0** (and `main`, which reports 4.0.0);
  CMake derives `WEBPP_BENCH_ADA_HAS_IPV4_FAST` from the ref's version string because
  `ada::ADA_VERSION_MAJOR` is a C++ enum constant that `#if` cannot read. v2.9.2 has
  no equivalent column.
- **Percent-encoding corpora are ASCII-only**: webpp's set-driven encoder leaves
  bytes outside the set untouched (non-ASCII passes through raw), while ada also
  UTF-8-encodes non-ASCII code points — including them would measure different
  behavior, not different speed.
- **Percent-decoding**: webpp is driven through `uri::details::decode_percent_encoded`
  with the same loop pattern its host parser uses; ada's `percent_decode` scans from
  the first `%` (passed per its API contract). Both decode every `%XX` sequence in
  the corpus.
- **`serialize_href` asymmetry**: `url_aggregator::get_href()` returns a
  `string_view` into its buffer (sub-nanosecond "cache hit" rows) and is *not*
  comparable to webpp's `as_string()` or `ada::url::get_href()`, which allocate.
  It is reported as its own config for reference.
- Setter corpora use inputs shaped like what a user assigns; the direct-parser
  corpora additionally include invalid inputs (e.g. `port/alpha`, `ipv4/invalid`)
  that only a piece parser is expected to see.

## Results

Machine: x86_64, Intel i9-13900K, GCC 16.2.1, Linux; `REPS=3`, `MIN_TIME=0.5s`,
medians, `-O3 -ffast-math`, `_GLIBCXX_DEBUG` from the dev preset. Re-run
`./run.sh` + the merge script to refresh on your own machine.

Tables are one per piece; columns are `webpp/<config>` and `ada@<ref>/<config>`.

## build

| input | webpp/setters | ada@main/setters | ada@v2_9_2/setters | ada@v4_0_0/setters |
| --- | ---: | ---: | ---: | ---: |
| default | 509.8 ns | 218.5 ns | 246.7 ns | 284.5 ns |

## credentials

| input | webpp/setter | ada@main/setter | ada@v2_9_2/setter | ada@v4_0_0/setter |
| --- | ---: | ---: | ---: | ---: |
| at | 89.7 ns | 23.8 ns | 38.2 ns | 46.7 ns |
| simple | 86.5 ns | 16.2 ns | 20.6 ns | 40.3 ns |
| space | 91.7 ns | 25.7 ns | 39.9 ns | 48.9 ns |

## domain_to_ascii

| input | webpp/direct | ada@main/to_ascii | ada@v2_9_2/to_ascii | ada@v4_0_0/to_ascii |
| --- | ---: | ---: | ---: | ---: |
| ascii | 29.6 ns | 26.4 ns | 42.4 ns | 25.9 ns |
| idn | 155.6 ns | 210.8 ns | 237.5 ns | 199.4 ns |
| idn_long | 184.4 ns | 323.7 ns | 367.7 ns | 292.9 ns |
| punycode | 18.3 ns | 24.4 ns | 221.9 ns | 24.9 ns |

## fragment

| input | webpp/direct | webpp/setter | ada@main/setter | ada@v2_9_2/setter | ada@v4_0_0/setter |
| --- | ---: | ---: | ---: | ---: | ---: |
| empty | 3.6 ns | 34.1 ns | 0.6 ns | 0.6 ns | 0.7 ns |
| encoded | 16.4 ns | 50.8 ns | 13.7 ns | 16.0 ns | 20.5 ns |
| simple | 21.6 ns | 57.0 ns | 15.9 ns | 17.8 ns | 21.8 ns |

## full_parse

| input | webpp/owning_loose | webpp/owning_standard | webpp/owning_strict | webpp/structured_standard | ada@main/url | ada@main/url_aggregator | ada@v2_9_2/url | ada@v2_9_2/url_aggregator | ada@v4_0_0/url | ada@v4_0_0/url_aggregator |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| backslash_path | 61.9 ns | 62.1 ns | 73.7 ns | 110.8 ns | 67.2 ns | 60.5 ns | 87.5 ns | 86.1 ns | 120.0 ns | 95.3 ns |
| dots_path | 89.3 ns | 87.3 ns | 91.2 ns | 145.1 ns | 106.7 ns | 101.7 ns | 108.5 ns | 104.6 ns | 164.2 ns | 147.6 ns |
| file_url | 56.2 ns | 53.7 ns | 49.8 ns | 94.3 ns | 69.5 ns | 71.5 ns | 71.8 ns | 75.3 ns | 72.7 ns | 69.7 ns |
| idn_host | 283.6 ns | 270.2 ns | 270.3 ns | 302.7 ns | 309.8 ns | 352.2 ns | 342.5 ns | 381.3 ns | 310.9 ns | 323.9 ns |
| invalid_host | 61.2 ns | 61.0 ns | 111.0 ns | 79.7 ns | 56.5 ns | 75.1 ns | 64.1 ns | 68.5 ns | 58.9 ns | 65.5 ns |
| ipv4_hex | 87.1 ns | 83.0 ns | 189.8 ns | 111.0 ns | 100.4 ns | 106.6 ns | 100.2 ns | 149.5 ns | 97.2 ns | 124.1 ns |
| ipv6_host | 84.2 ns | 83.2 ns | 71.5 ns | 117.7 ns | 85.8 ns | 90.7 ns | 87.1 ns | 94.0 ns | 94.4 ns | 95.6 ns |
| long_path | 155.7 ns | 153.7 ns | 156.7 ns | 217.7 ns | 45.7 ns | 30.1 ns | 137.8 ns | 123.0 ns | 131.0 ns | 107.1 ns |
| mailto_opaque | 69.7 ns | 70.3 ns | 59.2 ns | 97.8 ns | 81.4 ns | 77.1 ns | 80.0 ns | 75.6 ns | 79.2 ns | 79.0 ns |
| non_special_port | 87.2 ns | 85.5 ns | 79.4 ns | 115.8 ns | 105.0 ns | 136.1 ns | 100.5 ns | 141.2 ns | 104.9 ns | 143.6 ns |
| pct_path | 89.2 ns | 85.4 ns | 92.8 ns | 114.4 ns | 71.2 ns | 76.3 ns | 91.0 ns | 95.4 ns | 111.6 ns | 106.2 ns |
| punycode_host | 63.0 ns | 62.5 ns | 183.1 ns | 87.7 ns | 109.9 ns | 115.3 ns | 307.8 ns | 310.9 ns | 113.9 ns | 117.8 ns |
| query_frag_heavy | 100.3 ns | 97.4 ns | 106.3 ns | 156.4 ns | 57.9 ns | 33.9 ns | 90.4 ns | 79.0 ns | 63.6 ns | 37.4 ns |
| special_basic | 91.2 ns | 92.4 ns | 104.2 ns | 132.5 ns | 62.6 ns | 34.7 ns | 90.2 ns | 81.8 ns | 63.0 ns | 36.7 ns |
| special_nopath | 51.5 ns | 52.2 ns | 63.3 ns | 76.9 ns | 43.2 ns | 34.8 ns | 61.0 ns | 60.3 ns | 37.1 ns | 27.6 ns |
| special_port | 141.8 ns | 138.1 ns | 168.1 ns | 178.8 ns | 42.7 ns | 34.9 ns | 74.1 ns | 97.2 ns | 82.9 ns | 97.2 ns |
| trailing_dot | 56.6 ns | 54.8 ns | 113.8 ns | 80.5 ns | 45.5 ns | 36.7 ns | 70.6 ns | 63.8 ns | 39.7 ns | 29.6 ns |
| user_pass | 117.2 ns | 115.2 ns | 125.2 ns | 140.7 ns | 100.4 ns | 123.1 ns | 104.2 ns | 120.7 ns | 116.1 ns | 133.9 ns |

## host

| input | webpp/direct | webpp/setter | ada@main/setter | ada@v2_9_2/setter | ada@v4_0_0/setter |
| --- | ---: | ---: | ---: | ---: | ---: |
| domain | 30.5 ns | 66.0 ns | 32.3 ns | 28.4 ns | 36.8 ns |
| idn | 224.1 ns | 268.2 ns | 267.9 ns | 294.2 ns | 254.4 ns |
| ipv4 | 45.7 ns | 69.6 ns | 29.9 ns | 41.6 ns | 33.2 ns |
| ipv6 | 24.4 ns | 54.1 ns | 38.8 ns | 36.5 ns | 40.0 ns |
| punycode | 43.1 ns | 76.5 ns | 80.0 ns | 285.2 ns | 83.6 ns |

## ipv4

| input | webpp/host_ipv4 | webpp/pton | webpp/setter | ada@main/setter | ada@main/try_parse_fast | ada@v2_9_2/setter | ada@v4_0_0/setter | ada@v4_0_0/try_parse_fast |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| dotted | 30.0 ns | 3.8 ns | 75.2 ns | 29.7 ns | 3.0 ns | 41.8 ns | 33.3 ns | 3.0 ns |
| hex_octal | 28.4 ns | 0.6 ns | 106.9 ns | 67.9 ns | 0.5 ns | 110.2 ns | 97.5 ns | 0.5 ns |
| invalid | 14.3 ns | 0.9 ns |  |  | 0.8 ns |  |  | 0.8 ns |
| max | 38.4 ns | 4.8 ns | 86.2 ns | 32.4 ns | 3.9 ns | 44.9 ns | 35.1 ns | 3.9 ns |
| short | 17.0 ns | 1.7 ns | 58.4 ns | 56.3 ns | 0.2 ns | 55.3 ns | 59.8 ns | 0.2 ns |

## ipv6

| input | webpp/pton | webpp/setter | ada@main/setter | ada@v2_9_2/setter | ada@v4_0_0/setter |
| --- | ---: | ---: | ---: | ---: | ---: |
| compressed | 26.9 ns | 83.8 ns | 43.7 ns | 47.6 ns | 44.7 ns |
| full | 83.4 ns | 161.6 ns | 63.0 ns | 76.4 ns | 62.2 ns |
| plain | 6.2 ns | 59.7 ns | 39.0 ns | 36.8 ns | 38.8 ns |
| v4mapped | 26.5 ns | 90.9 ns | 50.1 ns | 62.9 ns | 48.6 ns |

## path

| input | webpp/direct | webpp/setter | ada@main/setter | ada@v2_9_2/setter | ada@v4_0_0/setter |
| --- | ---: | ---: | ---: | ---: | ---: |
| backslash | 26.2 ns | 74.7 ns | 36.5 ns | 35.3 ns | 40.9 ns |
| deep | 61.3 ns | 108.6 ns | 24.7 ns | 26.9 ns | 36.4 ns |
| dots | 38.9 ns | 88.6 ns | 50.6 ns | 43.2 ns | 57.8 ns |
| encoded | 39.5 ns | 85.0 ns | 37.2 ns | 39.4 ns | 43.1 ns |
| simple | 15.3 ns | 60.5 ns | 16.8 ns | 14.5 ns | 26.1 ns |
| trailing | 20.2 ns | 61.2 ns | 13.6 ns | 11.7 ns | 22.3 ns |

## percent_decode

| input | webpp/direct | ada@main/direct | ada@v2_9_2/direct | ada@v4_0_0/direct |
| --- | ---: | ---: | ---: | ---: |
| euro | 14.0 ns | 9.5 ns | 14.8 ns | 13.8 ns |
| plain | 8.6 ns | 3.1 ns | 2.9 ns | 3.0 ns |
| space | 5.6 ns | 12.3 ns | 13.2 ns | 12.4 ns |
| utf8 | 11.9 ns | 8.8 ns | 13.5 ns | 13.1 ns |

## percent_encode

| input | webpp/direct | ada@main/direct | ada@v2_9_2/direct | ada@v4_0_0/direct |
| --- | ---: | ---: | ---: | ---: |
| double_pct | 8.8 ns | 3.9 ns | 3.9 ns | 3.9 ns |
| plus_eq | 3.7 ns | 2.6 ns | 2.6 ns | 2.6 ns |
| space_amp | 17.1 ns | 12.4 ns | 16.0 ns | 12.5 ns |
| spaces_only | 12.0 ns | 5.9 ns | 13.9 ns | 6.0 ns |

## port

| input | webpp/direct | webpp/setter | ada@main/setter | ada@v2_9_2/setter | ada@v4_0_0/setter |
| --- | ---: | ---: | ---: | ---: | ---: |
| alpha | 4.1 ns |  |  |  |  |
| alt | 15.7 ns | 50.5 ns | 32.0 ns | 37.4 ns | 41.9 ns |
| custom | 15.9 ns | 50.5 ns | 32.2 ns | 37.4 ns | 40.6 ns |
| empty | 6.1 ns | 35.5 ns | 0.8 ns | 2.4 ns | 0.7 ns |
| overflow | 9.2 ns |  |  |  |  |
| zero | 8.9 ns | 42.0 ns | 25.2 ns | 30.2 ns | 33.7 ns |

## query

| input | webpp/direct | webpp/setter | ada@main/setter | ada@v2_9_2/setter | ada@v4_0_0/setter |
| --- | ---: | ---: | ---: | ---: | ---: |
| empty | 3.8 ns | 31.6 ns | 0.8 ns | 1.0 ns | 0.9 ns |
| encoded | 30.0 ns | 66.4 ns | 19.1 ns | 16.5 ns | 23.2 ns |
| long | 143.1 ns | 197.0 ns | 66.4 ns | 65.3 ns | 74.4 ns |
| simple | 18.9 ns | 53.1 ns | 15.1 ns | 13.9 ns | 21.3 ns |

## scheme

| input | webpp/direct | webpp/setter | ada@main/setter | ada@v2_9_2/setter | ada@v4_0_0/setter |
| --- | ---: | ---: | ---: | ---: | ---: |
| ftp | 8.9 ns | 36.1 ns | 24.8 ns | 22.6 ns | 32.9 ns |
| http | 10.6 ns | 37.3 ns | 25.5 ns | 22.5 ns | 34.1 ns |
| https | 12.0 ns | 39.9 ns | 25.5 ns | 23.3 ns | 33.2 ns |
| wss | 10.1 ns | 35.6 ns | 24.8 ns | 22.6 ns | 33.0 ns |

## serialize_href

| input | webpp/as_string | ada@main/url | ada@main/url_aggregator | ada@v2_9_2/url | ada@v2_9_2/url_aggregator | ada@v4_0_0/url | ada@v4_0_0/url_aggregator |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| idn_host | 15.7 ns | 13.4 ns | 0.2 ns | 29.3 ns | 0.3 ns | 13.7 ns | 0.2 ns |
| ipv6_host | 20.0 ns | 14.8 ns | 0.2 ns | 31.3 ns | 0.3 ns | 14.8 ns | 0.2 ns |
| special_basic | 24.1 ns | 16.5 ns | 0.2 ns | 51.9 ns | 0.3 ns | 16.7 ns | 0.2 ns |
| user_pass | 24.5 ns | 25.5 ns | 0.2 ns | 43.1 ns | 0.3 ns | 25.6 ns | 0.2 ns |

## serialize_ipv4

| input | webpp/ntop | ada@main/serializers | ada@v2_9_2/serializers | ada@v4_0_0/serializers |
| --- | ---: | ---: | ---: | ---: |
| any | 0.8 ns | 1.6 ns | 5.5 ns | 1.6 ns |
| dns | 0.8 ns | 1.6 ns | 7.8 ns | 1.6 ns |
| loopback | 1.5 ns | 2.0 ns | 6.4 ns | 2.0 ns |
| max | 3.9 ns | 3.1 ns | 9.3 ns | 3.1 ns |
| private | 2.4 ns | 2.4 ns | 9.0 ns | 2.4 ns |

## serialize_ipv6

| input | webpp/ntop | ada@main/serializers | ada@v2_9_2/serializers | ada@v4_0_0/serializers |
| --- | ---: | ---: | ---: | ---: |
| any | 6.9 ns | 8.9 ns | 10.1 ns | 8.2 ns |
| compressed_mid | 10.4 ns | 13.2 ns | 17.7 ns | 13.4 ns |
| full | 15.7 ns | 13.4 ns | 20.8 ns | 13.4 ns |
| loopback | 7.4 ns | 11.4 ns | 11.3 ns | 10.7 ns |
| v4mapped | 10.0 ns | 12.0 ns | 14.9 ns | 11.8 ns |
