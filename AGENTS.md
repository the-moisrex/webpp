# Web++ AI Contributor Guide

This file is the canonical repository instruction set for AI coding assistants.

## Project Identity

Web++ is an evolving, cross-platform C++ web framework. It provides HTTP abstractions, static and dynamic routing,
protocol adapters, URI/Unicode processing, traits and allocator support, storage, I/O, and related utilities.

Do not describe or treat this repository as a CMS. The project does not use C++ modules and has no `src/` tree.
Library implementation is predominantly template- and header-based under `webpp/`, while CMake exposes it as the
`webpp` static target.

Some prose documentation explicitly says that it is incomplete or outdated. Resolve conflicts in this order:

1. Tests and the current implementation
2. `CMakeLists.txt`, `CMakePresets.json`, and CI
3. Component README files
4. Root-level prose, old examples, and `todo.md`

Never "fix" code merely to make it agree with stale prose.

## Repository Map

- `webpp/`: public library headers and implementation
  - `http/`, `headers/`, `cgi/`, `fcgi/`, and `protocol/`: HTTP models, routing, and protocol integration
  - `uri/`, `unicode/`, and `ip/`: standards-sensitive parsing and normalization
  - `traits/`, `memory/`, and `std/`: customization, allocator propagation, STL aliases/polyfills, and iSTL helpers
  - `io/`, `async/`, `socket/`, and `concurrency/`: low-level and asynchronous facilities
  - `storage/`, `db/`, `json/`, `crypto/`, `views/`, and `middleware/`: framework services
- `tests/`: unit tests (`*_test.cpp`), fuzz targets (`*_fuzz.cpp`), shared test support, and standards fixtures
- `examples/`: runnable integration examples
- `benchmarks/`: focused performance experiments; do not treat benchmark code as the public API
- `sdk/`: the `wpp` and `wsdk` developer tools
- `cmake/`, `CMakeLists.txt`, and `CMakePresets.json`: build configuration and dependency setup
- `docs/` and component `README.md` files: project documentation
- `mcp/`: the repository-local `webpp-project` MCP server for AI assistants

When adding or removing a public header, update `ALL_SOURCES_SHORT` in `webpp/CMakeLists.txt`.

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
  and nearby tests before changing it. Keep pinned-standard behavior unless the task explicitly requests an update.
- Do not hand-edit generated Unicode tables or imported fixtures without also preserving their generation/update path.

## Code Style

- Follow `.clang-format` (4-space indentation, 120-column limit) and the applicable `.clang-tidy` configuration.
- Match naming in the subsystem being edited. The codebase commonly uses `snake_case` for types, functions, variables,
  and aliases, and PascalCase for concepts and test suites. Do not impose an unrelated naming scheme or perform broad
  renames.
- Keep the existing `WEBPP_*` header guards and namespace-closing comments in public headers.
- Add Doxygen-style documentation for new public APIs when the surrounding interface is documented.
- Comments and public documentation added to source files must be in English.
- Keep includes minimal and consistent with nearby headers. Do not bypass `webpp::stl`/polyfill boundaries casually.
- Prefer focused changes. Do not combine requested work with unrelated cleanup.

## Required Workflow

Use the repository-local `webpp-project` MCP server when it is available:

1. Call `project_overview`, then `git_status`.
2. Read the relevant implementation, tests, and component docs. `read_project_docs` and `search_project` are preferred
   for repository discovery.
3. Inspect the closest analogous implementation before editing.
4. Make the smallest coherent change and add or update the closest focused test.
5. Use `list_cmake_presets`, `run_cmake_configure`, `run_cmake_build`, `run_ctest`, `run_test_target`, or
   `run_all_tests` for verification.
6. Finish with `git_status` and `git_diff`, and review every changed line.

If the MCP server is unavailable, use the equivalent local commands and explicitly report that fallback. Never skip
inspection or verification because the MCP connection is missing.

Preserve pre-existing user changes. Build directories, downloaded dependencies, IDE state, and generated artifacts
must not be committed.

## Build and Test

The development configuration used by CI is:

```sh
cmake --preset dev-default
```

For a focused test, derive the target from its source file:

```text
tests/uri_test.cpp -> test-uri
tests/http_parser_test.cpp -> test-http-parser
```

When that focused preset exists, build and run only that target:

```sh
cmake --build --preset test-uri
ctest --preset tests -R '^test-uri$' --output-on-failure
```

`tests/CMakeLists.txt` discovers current test sources, while the focused presets in `CMakePresets.json` are enumerated
manually and can lag behind. Use MCP `list_test_targets` to detect gaps. MCP `run_test_target` deliberately builds the
actual CMake target and therefore also works for a current test without a dedicated preset.

For the complete unit-test suite, prefer MCP `run_all_tests`; it builds all current `*_test.cpp` targets instead of
trusting a potentially stale enumeration. The CI-intended preset flow is:

```sh
cmake --build --preset tests
ctest --preset tests --output-on-failure
```

If that build preset reports a missing target, do not hide the failure. Use `run_all_tests` (or direct targets from
`list_test_targets`) and report the preset drift separately.

Use the named GCC, Clang, release, examples, or benchmark presets only when relevant to the change. Fuzz targets are
Clang-only when `FUZZ_TESTS` is enabled. A benchmark run is performance evidence, not a correctness test.

Always run real verification after code or build-system changes. Documentation-only and AI-configuration changes must
still run their own format, parse, type-check, or smoke validation. Do not claim a command passed without its real exit
status. If dependencies, compiler support, network access, or time prevents a check, report the exact command and
reason.

## Review and Handoff

Before finishing:

- Confirm the diff contains only intended files.
- State what changed and why.
- List the exact verification commands and results.
- Call out unverified platforms, standards edge cases, or compatibility risks.
- Do not commit, push, or open a pull request unless the user asks.
