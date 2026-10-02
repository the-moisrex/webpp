---
name: webpp-build-test
description: Configure, build, and run Web++ tests in this repo — derive test target names from tests/*_test.cpp, detect stale or missing focused CMake presets, build and run one test or the full suite without trusting the manually enumerated tests preset. Use whenever verifying a change with CMake/CTest.
---

# Build and test Web++ (CMake/CTest)

Presets live in `CMakePresets.json`; CI configures with `cmake --preset dev-default`.

## Test target naming (from `tests/CMakeLists.txt`)

- `tests/foo_bar_test.cpp` → target `test-foo-bar` (strip `_test.cpp`, `_` → `-`, prefix `test-`).
- `tests/foo_fuzz.cpp` → target `fuzz-foo` (Clang only, needs `FUZZ_TESTS=ON`).
- Every test source is also compiled into the aggregate `webpp-tests` executable.

Enumerate current targets:

```sh
ls tests/*_test.cpp | sed 's|.*/||; s|_test\.cpp$||; s|_|-|g; s|^|test-|'
```

## Preset drift (check before trusting a `test-*` preset)

`CMakePresets.json` enumerates focused build/test presets manually, so it lags behind `tests/`. As of the last
check: 5 focused build presets missing (`test-cgi-headers`, `test-cgi-request`, `test-dynamic-scoping`,
`test-header-fields`, `test-middlewares`) and 5 stale (`test-extensions`, `test-header-accept-encoding`,
`test-traits`, `test-unicode-algos`, `test-ustring`). Re-run detection instead of trusting that list:

```sh
python3 - <<'EOF'
import json, pathlib
build = {p["name"] for p in json.load(open("CMakePresets.json")).get("buildPresets", [])
         if not p.get("hidden") and p.get("name", "").startswith("test-")}
derived = {"test-" + p.stem[: -len("_test")].replace("_", "-")
           for p in pathlib.Path("tests").glob("*_test.cpp")}
print("missing focused build presets:", sorted(derived - build))
print("stale focused build presets:", sorted(build - derived))
EOF
```

If a focused preset is missing or stale, use the direct build/ctest commands below instead — never edit
`CMakePresets.json` just to run one test.

## Configure

```sh
cmake --preset dev-default
```

- `dev-default` inherits `default`: `binaryDir` = `${sourceDir}/build`, Debug, C++23, `WEBPP_DEV=ON`.
- **`WEBPP_DEV=ON` is required** — it gates `add_subdirectory(tests ...)`. Symptom if the `build/` cache was
  configured without it: `ctest --test-dir build -N` reports `Total Tests: 0` and no `test-*` targets exist.
  Fix: re-run `cmake --preset dev-default` (check with `grep WEBPP_DEV build/CMakeCache.txt`).

## Run one focused test

When a focused build preset exists (verify with `ctest --list-presets` / `cmake --list-presets`):

```sh
cmake --build --preset test-uri
ctest --preset tests -R '^test-uri$' --output-on-failure
```

When it does not (or the preset is stale) — build the target directly in the preset's build dir, then filter
CTest by the same name (`build/` for `dev-default`):

```sh
cmake --build build --target test-uri
ctest --test-dir build -R '^test-uri$' --output-on-failure
```

## Run the full suite

The `tests` build preset's target list is manually enumerated and can be stale. Prefer deriving targets from
the sources:

```sh
cmake --build build --target $(ls tests/*_test.cpp | sed 's|.*/||; s|_test\.cpp$||; s|_|-|g; s|^|test-|')
ctest --test-dir build --output-on-failure
```

CI-style alternative (fine when preset drift was just checked):
`cmake --build --preset tests && ctest --preset tests --output-on-failure`.

## Discover what exists

```sh
cmake --list-presets    # configure presets
ctest --list-presets    # test presets (one per focused test)
```

## Rules

- Derive target names from `tests/*_test.cpp`, never from memory or a stale preset list.
- Report preset drift you find (missing/stale `test-*` presets) instead of silently working around it —
  `CMakePresets.json` preset lists are maintained manually.
- Never claim a build or test passed without its real exit status; on failure include the actual compiler/CTest
  output.
