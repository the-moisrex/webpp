# Ada URL vs Web++'s implementation of URL

As of writing this, ada's url parser has more features than our implementation. In an effort to better our impl, we're
going to benchmark our features against theirs.


GCC:

```
2023-10-07T10:23:28-08:00
Running ./a.out
Run on (8 X 3252.65 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 2.73, 2.90, 2.15
------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations
------------------------------------------------------------------------
WebppSchemePort                     9.77 ns         9.76 ns     73380355
AdaSchemePort                       13.7 ns         13.7 ns     51023375
WebppV1SerializeIPv4                34.3 ns         34.3 ns     20351735
WebppV1OptimizedSerializeIPv4       23.8 ns         23.8 ns     29480210
WebppV2OptimizedSerializeIPv4       23.5 ns         23.5 ns     29796549
WebppV3OptimizedSerializeIPv4       29.8 ns         29.7 ns     23817577
WebppSerializeIPv4                  34.4 ns         34.4 ns     20261027
AdaSerializeIPv4                    25.6 ns         25.6 ns     27404095
AdaSerializeIPv6                    49.8 ns         49.7 ns     13802566
WebppSerializeIPv6                  68.9 ns         68.8 ns      9466656
WebppV1SerializeIPv6Optimized       66.5 ns         66.4 ns     10491576
```

Clang:

```
2023-10-07T10:23:59-08:00
Load Average: 1.84, 2.66, 2.09
------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations
------------------------------------------------------------------------
WebppSchemePort                     6.04 ns         6.03 ns    119805564
AdaSchemePort                       9.69 ns         9.68 ns     72773044
WebppV1SerializeIPv4                28.0 ns         28.0 ns     25284775
WebppV1OptimizedSerializeIPv4       19.8 ns         19.8 ns     35617048
WebppV2OptimizedSerializeIPv4       20.0 ns         20.0 ns     35337953
WebppV3OptimizedSerializeIPv4       23.1 ns         23.1 ns     30393151
WebppSerializeIPv4                  28.1 ns         28.0 ns     24963531
AdaSerializeIPv4                    21.2 ns         21.2 ns     33463515
AdaSerializeIPv6                    46.2 ns         46.2 ns     15135847
WebppSerializeIPv6                  57.4 ns         57.4 ns     12221701
WebppV1SerializeIPv6Optimized       66.8 ns         66.7 ns     10504274
```
