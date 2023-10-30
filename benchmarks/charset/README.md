# Charset benchmarks

Charmap clearly wins; I'm not sure why ada-url implementers used such a way instead of doing a simple table lookup.

g++ (GCC) 13.2.1 20230801

```
./a.out --benchmark_min_time=3s                                                                                                  dev ✚ ✱ ◼
2023-10-29T21:58:04-08:00
Running ./a.out
Run on (8 X 2893.22 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 2.72, 3.56, 4.27
--------------------------------------------------------------------
Benchmark                          Time             CPU   Iterations
--------------------------------------------------------------------
AdaAutoVectorizedSolution       31.2 ns         31.1 ns    133553547
WebppSimpleCharsetSearch        14.2 ns         14.1 ns    295092731
WebppSimpleCharmapSearch        11.4 ns         11.3 ns    352648803
```

clang version 16.0.6

```
./a.out --benchmark_min_time=3s                                                                                                  dev ✚ ✱ ◼
2023-10-29T21:58:47-08:00
Running ./a.out
Run on (8 X 3193.28 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 3.31, 3.59, 4.25
--------------------------------------------------------------------
Benchmark                          Time             CPU   Iterations
--------------------------------------------------------------------
AdaAutoVectorizedSolution       28.0 ns         27.9 ns    148106244
WebppSimpleCharsetSearch        9.49 ns         9.47 ns    497421246
WebppSimpleCharmapSearch        8.63 ns         8.60 ns    475509707
```
