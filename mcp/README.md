# Web++ Project MCP Server

`webpp-project` is a repository-local MCP server for AI assistants working on Web++. It exposes safe, project-aware
tools instead of generic shell access.

## What It Knows

- The real Web++ layout under `webpp/`, `tests/`, `examples/`, `benchmarks/`, `sdk/`, and `cmake/`
- Component documentation for HTTP, URI, Unicode, traits/allocators, I/O, storage, middleware, and the SDK
- Configure, build, and test presets from `CMakePresets.json`
- The mapping from `tests/*_test.cpp` files to focused `test-*` targets
- Detection of missing or stale focused test presets
- Safe repository search, file reads, Git status/diff, configuration, builds, and CTest execution

`run_test_target` and `run_all_tests` build targets discovered from current test sources. They remain usable when the
manually enumerated focused presets have drifted from `tests/`.

The server rejects reads outside the repository, Git internals, build output, dependencies, environment files, and
common private-key formats. Commands run without a shell and are restricted to `git`, `rg`, `cmake`, and `ctest`.

## Setup

Install dependencies once:

```sh
npm --prefix mcp install
```

Type-check the server:

```sh
npm --prefix mcp run check
```

The root `.mcp.json` starts the server for clients that support project MCP configuration. The project root defaults to
the parent of `mcp/`; it can be overridden with `PROJECT_ROOT`.

Run it directly over stdio with:

```sh
npm --prefix mcp start
```
