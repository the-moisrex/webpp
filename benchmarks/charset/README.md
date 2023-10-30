# Charset benchmarks

Charmap clearly wins; I'm not sure why ada-url implementers used such a way instead of doing a simple table lookup.


#### With 5 characters:

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


#### With 5000 characters

GCC: 
```
./a.out --benchmark_min_time=3s                                                                                                      dev ✱
2023-10-29T22:50:25-08:00
Running ./a.out
Run on (8 X 2593.98 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 2.03, 2.27, 2.65
--------------------------------------------------------------------
Benchmark                          Time             CPU   Iterations
--------------------------------------------------------------------
AdaAutoVectorizedSolution        243 ns          242 ns     17182056
WebppSimpleCharsetSearch         211 ns          210 ns     19864824
WebppSimpleCharmapSearch         210 ns          209 ns     20098174
```

Clang:

```
./a.out --benchmark_min_time=3s                                                                                                      dev ✱
2023-10-29T22:51:33-08:00
Running ./a.out
Run on (8 X 2593.98 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 2.31, 2.37, 2.66
--------------------------------------------------------------------
Benchmark                          Time             CPU   Iterations
--------------------------------------------------------------------
AdaAutoVectorizedSolution        228 ns          228 ns     18493106
WebppSimpleCharsetSearch         212 ns          211 ns     20800836
WebppSimpleCharmapSearch         201 ns          201 ns     20870057
```
