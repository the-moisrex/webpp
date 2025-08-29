# Unicode's Canonical Reordering Benchmark

gcc (GCC) 15.2.1 20250813:
```asm
------------------------------------------------------------------------------------
Benchmark                                          Time             CPU   Iterations
------------------------------------------------------------------------------------
BM_AsciiUTF8_Iterator/8                         2.11 ns         2.10 ns    300426165
BM_AsciiUTF8_Iterator/64                        2.11 ns         2.10 ns    332628097
BM_AsciiUTF8_Iterator/512                       2.10 ns         2.10 ns    333881919
BM_AsciiUTF8_Iterator/4096                      2.10 ns         2.10 ns    334322605
BM_AsciiUTF8_Iterator/8192                      2.10 ns         2.10 ns    334105912
BM_AsciiUTF8_CanonicalReorder/8                 8.30 ns         8.29 ns     83874834
BM_AsciiUTF8_CanonicalReorder/64                56.8 ns         56.7 ns     12337376
BM_AsciiUTF8_CanonicalReorder/512                373 ns          373 ns      1875237
BM_AsciiUTF8_CanonicalReorder/4096              2918 ns         2914 ns       241330
BM_AsciiUTF8_CanonicalReorder/8192              5800 ns         5793 ns       120907
BM_CombiningUTF32_Iterator/8                    75.0 ns         74.9 ns      9266575
BM_CombiningUTF32_Iterator/64                    454 ns          453 ns      1520680
BM_CombiningUTF32_Iterator/512                  3295 ns         3290 ns       217697
BM_CombiningUTF32_Iterator/4096                26665 ns        26623 ns        26392
BM_CombiningUTF32_Iterator/8192                52889 ns        52811 ns        13712
BM_CombiningUTF32_Iterator_NoCopy/8             33.9 ns         33.8 ns     20324983
BM_CombiningUTF32_Iterator_NoCopy/64             283 ns          283 ns      2453612
BM_CombiningUTF32_Iterator_NoCopy/512           2297 ns         2294 ns       305645
BM_CombiningUTF32_Iterator_NoCopy/4096         18173 ns        18147 ns        38890
BM_CombiningUTF32_Iterator_NoCopy/8192         37087 ns        37036 ns        19106
BM_CombiningUTF32_CanonicalReorder/8            24.1 ns         24.0 ns     29012252
BM_CombiningUTF32_CanonicalReorder/64            151 ns          151 ns      4635388
BM_CombiningUTF32_CanonicalReorder/512          1123 ns         1122 ns       619363
BM_CombiningUTF32_CanonicalReorder/4096         9544 ns         9531 ns        72919
BM_CombiningUTF32_CanonicalReorder/8192        19288 ns        19263 ns        36952
BM_CombiningUTF32_Simple/8                      25.7 ns         25.7 ns     27168058
BM_CombiningUTF32_Simple/64                      164 ns          164 ns      4279832
BM_CombiningUTF32_Simple/512                    1298 ns         1296 ns       543166
BM_CombiningUTF32_Simple/4096                  10482 ns        10469 ns        65843
BM_CombiningUTF32_Simple/8192                  21097 ns        21071 ns        33440
BM_UTF8toUTF32_Iterator/8                       1.05 ns         1.05 ns    668355959
BM_UTF8toUTF32_Iterator/64                      1.05 ns         1.05 ns    663865175
BM_UTF8toUTF32_Iterator/512                     1.05 ns         1.05 ns    667343270
BM_UTF8toUTF32_Iterator/4096                    1.05 ns         1.05 ns    667999058
BM_UTF8toUTF32_Iterator/8192                    1.05 ns         1.05 ns    669934358
BM_NoAlloc_Iterator/8                           42.9 ns         42.8 ns     16419200
BM_NoAlloc_Iterator/64                           334 ns          334 ns      2061666
BM_NoAlloc_Iterator/512                         2860 ns         2857 ns       245651
BM_NoAlloc_Iterator/4096                       22076 ns        22042 ns        30118
BM_NoAlloc_Iterator/8192                       48071 ns        48001 ns        14204
BM_NoAlloc_CanonicalReorder_WithCopy/8          35.2 ns         35.2 ns     19875540
BM_NoAlloc_CanonicalReorder_WithCopy/64          305 ns          305 ns      2329726
BM_NoAlloc_CanonicalReorder_WithCopy/512        2403 ns         2400 ns       291887
BM_NoAlloc_CanonicalReorder_WithCopy/4096      19535 ns        19507 ns        35949
BM_NoAlloc_CanonicalReorder_WithCopy/8192      39105 ns        39041 ns        17858
BM_NoAlloc_CanonicalReorder_NoCopy/8            23.6 ns         23.6 ns     29670559
BM_NoAlloc_CanonicalReorder_NoCopy/64            204 ns          203 ns      3445557
BM_NoAlloc_CanonicalReorder_NoCopy/512          1536 ns         1534 ns       455097
BM_NoAlloc_CanonicalReorder_NoCopy/4096        12384 ns        12368 ns        56892
BM_NoAlloc_CanonicalReorder_NoCopy/8192        24737 ns        24704 ns        28343
```


clang version 20.1.8:
```asm
------------------------------------------------------------------------------------
Benchmark                                          Time             CPU   Iterations
------------------------------------------------------------------------------------
BM_AsciiUTF8_Iterator/8                         2.80 ns         2.80 ns    249639324
BM_AsciiUTF8_Iterator/64                        2.81 ns         2.80 ns    250057899
BM_AsciiUTF8_Iterator/512                       2.81 ns         2.80 ns    250159627
BM_AsciiUTF8_Iterator/4096                      2.81 ns         2.80 ns    250233878
BM_AsciiUTF8_Iterator/8192                      2.80 ns         2.80 ns    249798150
BM_AsciiUTF8_CanonicalReorder/8                 11.2 ns         11.2 ns     63336767
BM_AsciiUTF8_CanonicalReorder/64                63.5 ns         63.4 ns     11141415
BM_AsciiUTF8_CanonicalReorder/512                380 ns          379 ns      1845641
BM_AsciiUTF8_CanonicalReorder/4096              2932 ns         2927 ns       238815
BM_AsciiUTF8_CanonicalReorder/8192              5791 ns         5781 ns       120775
BM_CombiningUTF32_Iterator/8                    77.0 ns         76.9 ns      9192157
BM_CombiningUTF32_Iterator/64                    527 ns          526 ns      1341481
BM_CombiningUTF32_Iterator/512                  3552 ns         3547 ns       197226
BM_CombiningUTF32_Iterator/4096                27828 ns        27786 ns        25510
BM_CombiningUTF32_Iterator/8192                55616 ns        55523 ns        12606
BM_CombiningUTF32_Iterator_NoCopy/8             49.6 ns         49.5 ns     14754610
BM_CombiningUTF32_Iterator_NoCopy/64             410 ns          409 ns      1722819
BM_CombiningUTF32_Iterator_NoCopy/512           3337 ns         3333 ns       210223
BM_CombiningUTF32_Iterator_NoCopy/4096         25655 ns        25623 ns        27460
BM_CombiningUTF32_Iterator_NoCopy/8192         52584 ns        52513 ns        13424
BM_CombiningUTF32_CanonicalReorder/8            18.2 ns         18.2 ns     38600216
BM_CombiningUTF32_CanonicalReorder/64            115 ns          115 ns      6092221
BM_CombiningUTF32_CanonicalReorder/512           925 ns          923 ns       760753
BM_CombiningUTF32_CanonicalReorder/4096         7510 ns         7499 ns        93295
BM_CombiningUTF32_CanonicalReorder/8192        15103 ns        15081 ns        46598
BM_CombiningUTF32_Simple/8                      19.8 ns         19.8 ns     35262316
BM_CombiningUTF32_Simple/64                      129 ns          129 ns      5413641
BM_CombiningUTF32_Simple/512                    1000 ns          999 ns       700869
BM_CombiningUTF32_Simple/4096                   8357 ns         8345 ns        84099
BM_CombiningUTF32_Simple/8192                  16613 ns        16590 ns        42032
BM_UTF8toUTF32_Iterator/8                       2.80 ns         2.80 ns    249864028
BM_UTF8toUTF32_Iterator/64                      2.80 ns         2.80 ns    250850428
BM_UTF8toUTF32_Iterator/512                     2.80 ns         2.80 ns    249782447
BM_UTF8toUTF32_Iterator/4096                    2.80 ns         2.80 ns    249606603
BM_UTF8toUTF32_Iterator/8192                    2.80 ns         2.79 ns    250458485
BM_NoAlloc_Iterator/8                           42.2 ns         42.1 ns     16627018
BM_NoAlloc_Iterator/64                           413 ns          412 ns      1696429
BM_NoAlloc_Iterator/512                         3431 ns         3426 ns       204488
BM_NoAlloc_Iterator/4096                       26104 ns        26070 ns        26683
BM_NoAlloc_Iterator/8192                       52494 ns        52424 ns        13300
BM_NoAlloc_CanonicalReorder_WithCopy/8          22.8 ns         22.7 ns     30740883
BM_NoAlloc_CanonicalReorder_WithCopy/64          183 ns          183 ns      3834914
BM_NoAlloc_CanonicalReorder_WithCopy/512        1516 ns         1514 ns       464662
BM_NoAlloc_CanonicalReorder_WithCopy/4096      12355 ns        12336 ns        56724
BM_NoAlloc_CanonicalReorder_WithCopy/8192      24750 ns        24712 ns        28269
BM_NoAlloc_CanonicalReorder_NoCopy/8            19.9 ns         19.8 ns     35027758
BM_NoAlloc_CanonicalReorder_NoCopy/64            168 ns          168 ns      4082619
BM_NoAlloc_CanonicalReorder_NoCopy/512          1359 ns         1357 ns       512553
BM_NoAlloc_CanonicalReorder_NoCopy/4096        10830 ns        10814 ns        64743
BM_NoAlloc_CanonicalReorder_NoCopy/8192        21677 ns        21648 ns        32430
```