# Ada URL vs Web++'s implementation of URL

As of writing this, ada's url parser has more features than our implementation. In an effort to better our impl, we're
going to benchmark our features against theirs.


GCC:

```
2023-10-07T10:09:33-08:00
Running ./a.out
Run on (8 X 3301.2 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 1.27, 1.13, 1.46
------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations
------------------------------------------------------------------------
WebppSchemePort                     9.45 ns         9.45 ns     73236889
AdaSchemePort                       12.6 ns         12.6 ns     55849600
WebppV1SerializeIPv4                30.8 ns         30.8 ns     22750807
WebppV1OptimizedSerializeIPv4       22.0 ns         22.0 ns     31794079
WebppV2OptimizedSerializeIPv4       21.9 ns         21.9 ns     32114162
WebppSerializeIPv4                  31.1 ns         31.1 ns     22445001
AdaSerializeIPv4                    22.4 ns         22.4 ns     31388955
AdaSerializeIPv6                    44.7 ns         44.6 ns     15783639
WebppSerializeIPv6                  58.5 ns         58.5 ns     11833784
WebppV1SerializeIPv6Optimized       59.7 ns         59.6 ns     11853974
```

Clang:

```
2023-10-07T10:06:44-08:00
Load Average: 0.87, 1.10, 1.52
------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations
------------------------------------------------------------------------
WebppSchemePort                     5.68 ns         5.68 ns    122391310
AdaSchemePort                       9.37 ns         9.35 ns     76592869
WebppV1SerializeIPv4                26.1 ns         26.1 ns     26753124
WebppV1OptimizedSerializeIPv4       19.1 ns         19.1 ns     36785492
WebppV2OptimizedSerializeIPv4       19.4 ns         19.4 ns     35955354
WebppSerializeIPv4                  26.0 ns         26.0 ns     26772427
AdaSerializeIPv4                    20.1 ns         20.0 ns     34847434
AdaSerializeIPv6                    43.5 ns         43.4 ns     16218986
WebppSerializeIPv6                  51.4 ns         51.3 ns     13591171
WebppV1SerializeIPv6Optimized       59.0 ns         58.9 ns     11885098
```
