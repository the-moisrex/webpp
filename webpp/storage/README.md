# Storage system

## Embedded Files

Embedded files are files that the build system puts it in the output binary executable.

## Normal File read/writes
Read and write files; features:

- Searches embedded files first

## Caching

The idea of caching is carefully designed. Ideas:

- **Storage Gates**: the way the caches are store; e.g.: in a directory, in memory, in a file, in a database, ...
  - **Parent Storage Gates**: not yet implemented.
- **Caching Strategies**: everything else about caching; e.g.: time-based, First-in-first-out, Persistent, ...

