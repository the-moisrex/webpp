# Web++ AI Contributor Guide

This file is the canonical repository instruction set for AI coding assistants.

## Project Identity

Web++ is an evolving, cross-platform C++ web framework. It provides HTTP abstractions, static and dynamic routing,
protocol adapters, URI/Unicode processing, traits and allocator support, storage, I/O, and related utilities. The
project does not use C++ modules and has no `src/` tree. Library implementation is predominantly template- and
header-based under `webpp/`, while CMake exposes it as the `webpp` static target.

Some prose documentation explicitly says that it is incomplete or outdated. Resolve conflicts in this order:

1. Tests and the current implementation
2. `CMakeLists.txt`, `CMakePresets.json`, and CI
3. Component README files
4. Root-level prose, old examples, and `todo.md`

Never "fix" code merely to make it agree with stale prose.

Tie-breakers for the cases the order alone does not settle:

- When tests and the implementation disagree with each other, determine which side violates the pinned or current
  standard (for URI/URL behavior use `tools/whatwg-url-specs`) instead of siding with one by default.
- When refreshed standards fixtures (e.g., `tests/assets/whatwg/urltestdata.json`) encode newer behavior than the
  snapshot pinned in a component README, the tests win; report the pin discrepancy and only update the pin with the
  user's approval.

## Quick Reference

```sh
cmake --preset dev-default                                   # configure once (WEBPP_DEV=ON, build dir: build/)
cmake --build build --target test-uri -j $(nproc)            # build one test: tests/foo_bar_test.cpp -> test-foo-bar
ctest --test-dir build -R '^test-uri$' --output-on-failure   # run it; CI equivalent: ctest --preset tests -R ...
clang-format --dry-run -Werror <changed files>               # format check before finishing
```

Beware: plain `ctest -R` exits `0` when the filter matches nothing, so confirm the test actually ran (`ctest -N -R
'^test-uri$'` lists matches). The test presets set `noTestsAction=error` and exit `8` instead.

## Repository Map

- `webpp/`: public library headers and implementation
  - `http/`, `headers/`, `cgi/`, `fcgi/`, and `protocol/`: HTTP models, routing, and protocol integration
  - `uri/`, `unicode/`, and `ip/`: standards-sensitive parsing and normalization
  - `traits/`, `memory/`, and `std/`: customization, allocator propagation, STL aliases/polyfills, and iSTL helpers
  - `io/`, `async/`, `socket/`, and `concurrency/`: low-level and asynchronous facilities
  - `storage/`, `db/`, `json/`, `crypto/`, `views/`, and `middleware/`: framework services
- `tests/`: unit tests (`*_test.cpp`), fuzz targets (`*_fuzz.cpp`), shared test support, and standards fixtures.
  `tests/assets/whatwg/urltestdata.json` is the vendored WPT URL fixture that `tests/uri_whatwg_test.cpp` mirrors.
- `examples/`: runnable integration examples
- `benchmarks/`: focused performance experiments; do not treat benchmark code as the public API
- `sdk/`: the `wpp` and `wsdk` developer tools
- `cmake/`, `CMakeLists.txt`, and `CMakePresets.json`: build configuration and dependency setup
- `docs/` and component `README.md` files: project documentation
- `.agents/skills/`: agent skills (`whatwg-url-specs` for the WHATWG URL Standard, `webpp-build-test` for CMake/CTest)
- `tools/`: developer scripts, including `whatwg-url-specs` for querying the WHATWG URL Standard

Component documentation map (read the closest one for the subsystem you touch):

| Topic | Files |
| --- | --- |
| Overview | `README.md`, `webpp/README.md`, `docs/index.md` |
| HTTP | `webpp/http/README.md`, `webpp/headers/README.md`, `webpp/http/bodies/README.md` |
| URI | `webpp/uri/README.md` |
| Unicode | `webpp/unicode/README.md`, `webpp/unicode/details/README.md` |
| Traits/STL/memory | `webpp/traits/README.md`, `webpp/std/README.md`, `webpp/memory/README.md` |
| I/O | `webpp/io/README.md` |
| Storage | `webpp/storage/README.md` |
| Middleware | `webpp/middleware/README.md` |
| SDK | `sdk/README.md` |
| Benchmarks | `benchmarks/README.md` |

When adding or removing a public header, update `ALL_SOURCES_SHORT` in `webpp/CMakeLists.txt`.

## WHATWG URL Standard Lookup

`tools/whatwg-url-specs` fetches, caches, and queries the WHATWG URL Standard. Use it when implementing or reviewing
standards-sensitive URI/URL behavior instead of relying on memory/instinct:

```sh
tools/whatwg-url-specs list                # section/definition/algorithm tree
tools/whatwg-url-specs url-parsing         # targeted section or term lookup
```

The `whatwg-url-specs` skill documents the full usage.

## Architecture and Compatibility

- Preserve the existing namespace and directory boundaries. Put code in the narrowest existing subsystem.
- Public library code must remain compatible with the target's C++20 baseline. Tests and GCC development builds may
  use C++23 where their CMake targets already request it. Do not raise the library baseline incidentally.
- Prefer the project's `webpp::stl` compatibility aliases for standard-library facilities where surrounding code uses
  them. `webpp::istl` is for Web++ extensions; it must not depend on unrelated Web++ subsystems.
- Preserve concepts-based contracts, traits customization, allocator propagation, and character-type genericity.
  Avoid silently replacing them with hard-coded `std::string`, `char`, global allocation, or runtime polymorphism.
- Preserve compile-time behavior in static routing and other constexpr-heavy code. Use `constexpr`, `consteval`,
  concepts, `static_assert`, `noexcept`, and `[[nodiscard]]` when the semantics justify them, not as decoration.
- Avoid adding virtual dispatch or ownership indirection on performance-sensitive paths without an explicit design
  reason.
- URI, IDNA, Unicode, HTTP, and IP behavior is standards-sensitive. Consult the relevant component README, fixtures,
  and nearby tests before changing it. Keep pinned-standard behavior unless the task explicitly requests an update or
  refreshed tests already encode newer behavior (see the tie-breakers).
- Do not hand-edit generated Unicode tables or imported fixtures without also preserving their generation/update path.

## Code Style

- Follow `.clang-format` (4-space indentation, 120-column limit) and the applicable `.clang-tidy` configuration.
  Run `clang-format --dry-run -Werror` on changed files before finishing.
- Match naming in the subsystem being edited. The codebase commonly uses `snake_case` for types, functions, variables,
  and aliases, and PascalCase for concepts and test suites. Do not impose an unrelated naming scheme or perform broad
  renames.
- Keep the existing `WEBPP_*` header guards and namespace-closing comments in public headers.
- Add Doxygen-style documentation for new public APIs when the surrounding interface is documented.
- Keep includes minimal and consistent with nearby headers. Do not bypass `webpp::stl`/polyfill boundaries casually.
- Prefer focused changes. Do not combine requested work with unrelated cleanup.

## Required Workflow

1. Search first: use ripgrep (or your own search tools) to locate the symbol, behavior, or expectation, then read
   only the relevant ranges. Do not re-read files whose relevant parts are already in your context, and consult the
   component doc map only for the subsystem you touch.
2. Inspect the closest analogous implementation before editing.
3. Make the smallest coherent change and add or update the closest focused test.
4. Verify with the Quick Reference commands (the `webpp-build-test` skill documents presets and drift).
5. If failures appear outside your change, establish a baseline before judging them: `git stash` your change, rebuild
   and rerun the same tests, compare the set of failing test names, then `git stash pop`. Report pre-existing failures
   as a delta; do not fix unrelated failures unless asked.
6. Finish with `git status` and `git diff`, and review every changed line.

Preserve pre-existing user changes. The user may be editing files concurrently: stage by explicit path
(`git add <file>`), never `git add -A` or `git add .`. Build directories, downloaded dependencies, IDE state, and
generated artifacts must not be committed.

## Build and Test

Configure with the CI development configuration:

```sh
cmake --preset dev-default
```

Derive focused test targets from their sources:

```text
tests/uri_test.cpp -> test-uri
tests/http_parser_test.cpp -> test-http-parser
```

Build and run one test:

```sh
cmake --build build --target test-uri -j $(nproc)
ctest --test-dir build -R '^test-uri$' --output-on-failure
```

Focused and CI presets (`cmake --build --preset test-uri`, `ctest --preset tests ...`) work as alternatives; the
`webpp-build-test` skill documents them. The preset lists in `CMakePresets.json` are enumerated manually and can lag
behind current `tests/*_test.cpp` sources — report such drift instead of editing the presets just to run one test.

For the complete unit-test suite, build the targets derived from current sources rather than trusting a stale
enumeration; `-- -k` keeps going past targets that fail to compile, and report those separately:

```sh
cmake --build build --target $(ls tests/*_test.cpp | sed 's|.*/||; s|_test\.cpp$||; s|_|-|g; s|^|test-|') \
  -j $(nproc) -- -k
ctest --test-dir build --output-on-failure
```

Use the named GCC, Clang, release, examples, or benchmark presets only when relevant to the change. Fuzz targets are
Clang-only when `FUZZ_TESTS` is enabled. A benchmark run is performance evidence, not a correctness test.

Always run real verification after code or build-system changes. Documentation-only and AI-configuration changes must
still run their own format, parse, type-check, or smoke validation. Do not claim a command passed without its real
exit status. If dependencies, compiler support, network access, or time prevents a check, report the exact command and
reason.

## Review and Handoff

Before finishing:

- Confirm the diff contains only intended files.
- State what changed and why.
- List the exact verification commands and results.
- Call out unverified platforms, standards edge cases, or compatibility risks.
- Commit messages: short imperative summary, optionally suffixed with `; #issue` (e.g. `Fix ipv6 bug; #774`).
- Do not commit, push, or open a pull request unless the user asks.
