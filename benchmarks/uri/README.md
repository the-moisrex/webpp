# URI Parsing benchmarks

V2 is winning.

g++ (GCC) 13.2.1 20230801:

```
2023-10-25T16:19:22-08:00
Running ./a.out
Run on (8 X 3261.37 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 7.93, 7.59, 4.52
------------------------------------------------------
Benchmark            Time             CPU   Iterations
------------------------------------------------------
URIDecodeV1       3536 ns         3529 ns       200401
URIDecodeV2       2624 ns         2618 ns       265918
```

clang version 16.0.6:
```
2023-10-25T16:19:12-08:00
Running ./a.out
Run on (8 X 3304.12 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 9.20, 7.82, 4.55
------------------------------------------------------
Benchmark            Time             CPU   Iterations
------------------------------------------------------
URIDecodeV1       4469 ns         4461 ns       158874
URIDecodeV2       1954 ns         1949 ns       356964
```
