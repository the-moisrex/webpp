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

##### 2024 Update
GCC:
```
2024-07-11T14:08:36-10:00
Running ./a.out
Run on (32 X 4740.98 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 2048 KiB (x16)
  L3 Unified 36864 KiB (x1)
Load Average: 1.13, 1.66, 1.85
--------------------------------------------------------------------------------
Benchmark                                      Time             CPU   Iterations
--------------------------------------------------------------------------------
WebppSchemePort                             1.80 ns         1.79 ns    384244616
WebppSchemePort2                            3.19 ns         3.18 ns    217761347
WebppSchemePort2IgnoreNewlinesAndTabs       4.30 ns         4.30 ns    158234399
AdaSchemePort                               2.42 ns         2.42 ns    289347575
WebppV1SerializeIPv4                        4.95 ns         4.94 ns    141669234
WebppV1OptimizedSerializeIPv4               5.90 ns         5.89 ns    118249427
WebppV2OptimizedSerializeIPv4               5.19 ns         5.18 ns    133290528
WebppV3OptimizedSerializeIPv4               8.36 ns         8.35 ns     84254173
WebppSerializeIPv4                          4.98 ns         4.97 ns    137779043
AdaSerializeIPv4                            7.76 ns         7.75 ns     88713469
AdaSerializeIPv6                            14.7 ns         14.7 ns     47475947
WebppSerializeIPv6                          19.0 ns         19.0 ns     36900642
WebppSerializeIPv6Append                    19.2 ns         19.2 ns     36635540
WebppV1SerializeIPv6Optimized               20.8 ns         20.8 ns     33691091
AdaCCC                                      7.84 ns         7.83 ns     89760290
WebppCCC                                    7.84 ns         7.83 ns     89362178
AdaDecompose                                25.4 ns         25.3 ns     27043731
WebppDecomposeUTF32                         57.7 ns         57.6 ns     12109128
WebppDecomposeUTF8                          61.5 ns         61.4 ns     11190026
WebppDecomposeToUTF32Iterator                111 ns          111 ns      6277616
WebppDecomposeToUTF32                       50.7 ns         50.6 ns     10000000
WebppDecomposeToUTF8                        68.9 ns         68.8 ns     10454304
WebppDecomposeToUTF8Iterator                77.7 ns         77.6 ns      9058927
WebppDecomposeToUTF8FromUTF32               55.5 ns         55.5 ns     12412958
WebppDecomposeToUTF8FromUTF32Iterator       70.8 ns         70.7 ns      9980300
AdaDecomposeTo1CodePiont                    3.93 ns         3.93 ns    174973579
WebppDecomposeTo1CodePiont                  1.22 ns         1.22 ns    548249204
AdaDecomposeLength                          19.5 ns         19.5 ns     35994305
WebppDecomposeLength                        9.83 ns         9.82 ns     71280791
```

Clang:

```
2024-07-11T14:09:38-10:00
Running ./a.out
Run on (32 X 800 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 2048 KiB (x16)
  L3 Unified 36864 KiB (x1)
Load Average: 1.07, 1.55, 1.80
--------------------------------------------------------------------------------
Benchmark                                      Time             CPU   Iterations
--------------------------------------------------------------------------------
WebppSchemePort                             1.22 ns         1.22 ns    574163097
WebppSchemePort2                            4.20 ns         4.20 ns    167165982
WebppSchemePort2IgnoreNewlinesAndTabs       4.85 ns         4.84 ns    144426822
AdaSchemePort                               2.44 ns         2.43 ns    287503536
WebppV1SerializeIPv4                        4.98 ns         4.97 ns    140731266
WebppV1OptimizedSerializeIPv4               5.97 ns         5.95 ns    117251519
WebppV2OptimizedSerializeIPv4               5.60 ns         5.59 ns    125194362
WebppV3OptimizedSerializeIPv4               5.93 ns         5.92 ns    118249897
WebppSerializeIPv4                          6.54 ns         6.53 ns    107081359
AdaSerializeIPv4                            4.99 ns         4.98 ns    140585724
AdaSerializeIPv6                            15.1 ns         15.1 ns     45099012
WebppSerializeIPv6                          21.9 ns         21.9 ns     32196680
WebppSerializeIPv6Append                    22.1 ns         22.1 ns     31770324
WebppV1SerializeIPv6Optimized               20.0 ns         19.9 ns     35095576
AdaCCC                                      8.67 ns         8.66 ns     81698003
WebppCCC                                    7.88 ns         7.87 ns     88817154
AdaDecompose                                22.7 ns         22.6 ns     30861967
WebppDecomposeUTF32                         48.7 ns         48.7 ns     14331772
WebppDecomposeUTF8                          74.6 ns         74.5 ns      9463550
WebppDecomposeToUTF32Iterator                106 ns          106 ns      6515126
WebppDecomposeToUTF32                       62.7 ns         62.6 ns     11014504
WebppDecomposeToUTF8                        70.8 ns         70.7 ns     10159202
WebppDecomposeToUTF8Iterator                75.7 ns         75.5 ns      9228264
WebppDecomposeToUTF8FromUTF32               54.2 ns         54.1 ns     13043494
WebppDecomposeToUTF8FromUTF32Iterator       67.6 ns         67.5 ns     10278803
AdaDecomposeTo1CodePiont                   0.896 ns        0.894 ns    777793930
WebppDecomposeTo1CodePiont                  1.27 ns         1.26 ns    555233881
AdaDecomposeLength                          19.5 ns         19.5 ns     35681879
WebppDecomposeLength                        9.93 ns         9.91 ns     70452471
```

### Decomposition

```
2024-07-10T22:04:24-10:00
Running ./a.out
Run on (32 X 5500 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 2048 KiB (x16)
  L3 Unified 36864 KiB (x1)
Load Average: 1.29, 1.96, 1.69
--------------------------------------------------------------------------------
Benchmark                                      Time             CPU   Iterations
--------------------------------------------------------------------------------
AdaDecompose                                26.4 ns         26.4 ns     26597817
WebppDecomposeUTF32                         53.8 ns         53.8 ns     13036580
WebppDecomposeUTF8                          65.3 ns         65.3 ns     10755879
WebppDecomposeToUTF32Iterator                116 ns          116 ns      6063044
WebppDecomposeToUTF32                       52.4 ns         52.4 ns     13394856
WebppDecomposeToUTF8                        63.2 ns         63.1 ns     11173791
WebppDecomposeToUTF8Iterator                72.7 ns         72.6 ns      9677571
WebppDecomposeToUTF8FromUTF32               55.9 ns         55.8 ns     12635134
WebppDecomposeToUTF8FromUTF32Iterator       60.5 ns         60.4 ns     11213052
AdaDecomposeTo1CodePiont                    1.64 ns         1.64 ns    427260528
WebppDecomposeTo1CodePiont                  1.29 ns         1.28 ns    544459154
AdaDecomposeLength                          19.5 ns         19.5 ns     35987936
WebppDecomposeLength                        12.4 ns         12.4 ns     65611096
```

### Canonical Combining Class (CCC)

```
-----------------------------------------------------
Benchmark           Time             CPU   Iterations
-----------------------------------------------------
AdaCCC           8.58 ns         8.56 ns     81663568
WebppCCC         7.90 ns         7.88 ns     89083547
```

We have the same performance in `ccc_of` and `get_ccc`; if we use `mask`s, we'll lose a couple of nanoseconds.
I've disabled masks in CCC, it has two effects:

1. Makes the `ccc_indices` table shorter since we'd only need 16bit instead of 32bit
2. Makes the `ccc_values` larger since we no longer have masks thus `mask=0` does not exists

The combination of these two effects have canceled each other.

