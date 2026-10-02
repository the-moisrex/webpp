---
name: whatwg-url-specs
description: Debug Web++ URL/URI behavior against the authoritative WHATWG URL Standard using the repo-local tools/whatwg-url-specs command. Use when a URL test fails, parser/serializer/host/IDNA behavior looks wrong, or you need the exact spec section, algorithm steps, parser state, or definition behind a URI requirement.
---

# Debugging URLs with the WHATWG URL Standard

When Web++ URI behavior disagrees with expectations (failing `tests/uri*_test.cpp` cases, routing/normalization
bugs, serializer round-trip failures), the authority is the WHATWG URL Standard. `tools/whatwg-url-specs` fetches,
caches, and queries it so you can read the exact normative steps instead of guessing.

## Debugging workflow

1. Reproduce: run the failing focused test (see the `webpp-build-test` skill), note the exact input/output mismatch.
2. Locate the spec part: find the section/algorithm/state in the map below, or run
   `tools/whatwg-url-specs list` (full tree, ~345 lines) / `tools/whatwg-url-specs --algorithm list` (72 algorithms).
3. Read the normative text: `tools/whatwg-url-specs <query>` prints it as Markdown.
4. Compare step-by-step with the implementation (`webpp/uri/parser/*.hpp`) and the tests
   (`tests/uri_test.cpp`, `tests/uri_whatwg_test.cpp`, `tests/uri_host_authority_test.cpp`,
   `tests/structured_uri_test.cpp`).
5. Treat discrepancies per the pinned-standard rule below: tests + implementation win unless the task is to update
   the pinned snapshot.

## Spec map (from `list`, depth <= 2)

```text
▪ Abstract
▪ Table of Contents
▪ Goals
▪ 1.Infrastructure
  ▪ 1.1.Writing
  ▪ 1.2.Parsers
  ▪ 1.3.Percent-encoded bytes
▪ 2.Security considerations
▪ 3.Hosts (domains and IP addresses)
  ▪ 3.1.Host representation
  ▪ 3.2.Host miscellaneous
  ▪ 3.3.IDNA
  ▪ 3.4.Host writing
  ▪ 3.5.Host parsing
  ▪ 3.6.Host serializing
  ▪ 3.7.Host equivalence
▪ 4.URLs
  ▪ 4.1.URL representation
  ▪ 4.2.URL miscellaneous
  ▪ 4.3.URL writing
  ▪ 4.4.URL parsing
  ▪ 4.5.URL serializing
  ▪ 4.6.URL equivalence
  ▪ 4.7.Origin
  ▪ 4.8.URL rendering
    ▪ 4.8.1.Simplify non-human-readable or irrelevant components
    ▪ 4.8.2.Elision
    ▪ 4.8.3.Internationalization and special characters
▪ 5.application/x-www-form-urlencoded
  ▪ 5.1.application/x-www-form-urlencodedparsing
  ▪ 5.2.application/x-www-form-urlencodedserializing
  ▪ 5.3.Hooks
▪ 6.API
  ▪ 6.1.URL class
  ▪ 6.2.URLSearchParams class
  ▪ 6.3.URL APIs elsewhere
▪ Acknowledgments
▪ Intellectual property rights
▪ Index
  ▪ Terms defined by this specification
  ▪ Terms defined by reference
▪ References
  ▪ Normative References
  ▪ Non-Normative References
▪ IDL Index
```

## Queries that map to common bugs

All verified against the cached spec (exit 0):

| Symptom area | Query |
| --- | --- |
| Whole-parser behavior | `url-parsing`, `basic URL parser`, `URL parser` |
| Wrong scheme/relative handling | `path-state`, `cannot-be-a-base-url-path-state` |
| Host/IDNA/domain issues | `host parsing`, `IDNA`, `domain parser ToASCII`, `opaque-host parser` |
| IP literal handling | `IPv4 parser`, `IPv6 parser` |
| Serialization/normalization | `URL writing`, `origin` |
| Percent-encoding | `percent-encoded bytes` |
| Form/query encoding | `application/x-www-form-urlencoded`, `urlencoded parser` |
| Path shortening (`..`, dot segments) | `shorten a url` |

Parser machine states are indexed definitions too, e.g. `scheme-start-state`, `authority-state`, `host-state`,
`port-state`, `file-state`, `path-start-state`, `query-state`, `fragment-state` — query any of them by id.

### Useful flags

```sh
tools/whatwg-url-specs list                # full section/definition/algorithm tree
tools/whatwg-url-specs --algorithm list    # algorithms only
tools/whatwg-url-specs --algorithm host parser   # restrict a query to algorithms
tools/whatwg-url-specs --html url-parsing # raw HTML instead of Markdown
tools/whatwg-url-specs --verbose ...       # cache/parse progress on stderr
tools/whatwg-url-specs --clean-cache       # drop cached spec (fresh download next run)
```

## Matching rules (so queries return what you expect)

- `https://url.spec.whatwg.org/#some-id` selects that exact id.
- Plain queries match case-insensitively against ids first, then titles; exact beats fuzzy, so prefer exact ids
  from `list` (`path-state`, not `path state`).
- Multiple queries in one run are merged and de-duplicated, order preserved.
- No query defaults to the `url-parsing` section.
- Exit `1` + `ERROR: No sections found ...` on stderr = refine the query; exit `0` = found.

## Pinned-standard caveat

`webpp/uri/README.md` pins the URL Standard to a commit snapshot (25 March 2024); this tool reads the **live**
cached spec (<https://url.spec.whatwg.org/>). If live text and the pinned snapshot disagree, the pinned snapshot
plus the existing tests/implementation win unless the task explicitly updates the pinned standard.
