# URI Parsing benchmarks

V3 is winning; but in clang, v2 and v3 are pretty much the same.

g++ (GCC) 13.2.1 20230801:

```
2023-10-25T17:14:29-08:00
Running ./a.out
Run on (8 X 3167.99 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 2.26, 1.62, 1.20
------------------------------------------------------
Benchmark            Time             CPU   Iterations
------------------------------------------------------
URIDecodeV1       3809 ns         3801 ns       185952
URIDecodeV2       2610 ns         2607 ns       267337
URIDecodeV3       2482 ns         2480 ns       280884
```

clang version 16.0.6:
```
2023-10-25T17:15:07-08:00
Running ./a.out
Run on (8 X 3218.91 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 1.66, 1.54, 1.19
------------------------------------------------------
Benchmark            Time             CPU   Iterations
------------------------------------------------------
URIDecodeV1       4570 ns         4564 ns       162005
URIDecodeV2       2063 ns         2059 ns       341169
URIDecodeV3       2069 ns         2066 ns       339707
```
