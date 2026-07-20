# Claude Code Instructions for Web++

Read and follow `AGENTS.md` before working in this repository. It is the canonical project guide; this file only adds
Claude-specific operating instructions.

## Repository Context

This is the Web++ C++ web framework, not a CMS. Public implementation is predominantly in `.hpp` files under `webpp/`.
There is no `src/` tree and no C++ modules architecture. Tests are under `tests/` and are mapped to CMake targets such
as `tests/uri_test.cpp` -> `test-uri`.

When prose conflicts with code, tests, CMake, or CI, follow the source-of-truth order in `AGENTS.md`. In particular,
some component documentation openly warns that it is incomplete or outdated.

## Use the Project MCP Server

The repository's `.mcp.json` registers the `webpp-project` server from `mcp/`.

At the start of a coding task:

1. Use `project_overview`.
2. Use `git_status`.
3. Use `read_project_docs` for the affected subsystem.
4. Use `search_project` and `read_project_file` to inspect implementation and tests.

For verification, prefer `list_cmake_presets`, `run_cmake_configure`, `run_test_target`, `run_all_tests`,
`run_cmake_build`, and `run_ctest`. `run_test_target` and `run_all_tests` discover current test files and avoid relying
on stale, manually enumerated test presets. End by checking `git_status` and `git_diff`.

If `webpp-project` is not connected, continue with equivalent shell commands rather than guessing. Mention the missing
connection in the final report.

## Claude Working Rules

- Do not modify files before inspecting the nearest implementation and test.
- Keep changes scoped; do not modernize or rename unrelated code.
- Preserve C++20 compatibility for public library headers and the traits/allocator/character-type contracts.
- Do not replace `webpp::stl` compatibility aliases or `webpp::istl` boundaries without a specific reason.
- Treat URI, Unicode, IDNA, HTTP, and IP changes as standards-sensitive.
- Review the complete diff before reporting completion.
- Never claim build or test success without running the command.
- Do not commit, push, or create a pull request unless explicitly requested.
