# Charset benchmarks

Charmap clearly wins; I'm not sure why ada-url implementers used such a way instead of doing a simple table lookup.

Not so much interestingly the charset version loses completely.

#### With 10 characters:

g++ (GCC) 13.2.1 20230801

```
./a.out --benchmark_min_time=3s
2023-10-30T05:20:23-08:00
Running ./a.out
Run on (8 X 2993.05 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 2.11, 2.32, 2.23
--------------------------------------------------------------------
Benchmark                          Time             CPU   Iterations
--------------------------------------------------------------------
AdaAutoVectorizedSolution       25.7 ns         25.7 ns    174780063
WebppSimpleCharsetSearch        28.3 ns         28.2 ns    148920655
WebppSimpleCharmapSearch        15.9 ns         15.9 ns    265948817
```

clang version 16.0.6

```
./a.out --benchmark_min_time=3s
2023-10-30T05:21:30-08:00
Running ./a.out
Run on (8 X 2793.56 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 2.13, 2.26, 2.22
--------------------------------------------------------------------
Benchmark                          Time             CPU   Iterations
--------------------------------------------------------------------
AdaAutoVectorizedSolution       24.1 ns         24.1 ns    178401363
WebppSimpleCharsetSearch        24.7 ns         24.7 ns    179125943
WebppSimpleCharmapSearch        13.7 ns         13.7 ns    305770003
```


#### With 5000 characters

GCC: 
```
./a.out --benchmark_min_time=3s
2023-10-30T05:18:36-08:00
Running ./a.out
Run on (8 X 3047.1 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 1.30, 2.41, 2.25
--------------------------------------------------------------------
Benchmark                          Time             CPU   Iterations
--------------------------------------------------------------------
AdaAutoVectorizedSolution        207 ns          207 ns     20615851
WebppSimpleCharsetSearch         221 ns          220 ns     19212692
WebppSimpleCharmapSearch         200 ns          199 ns     21167772
```

Clang:

```
./a.out --benchmark_min_time=3s
2023-10-30T05:19:27-08:00
Running ./a.out
Run on (8 X 3192.63 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 1.54, 2.30, 2.22
--------------------------------------------------------------------
Benchmark                          Time             CPU   Iterations
--------------------------------------------------------------------
AdaAutoVectorizedSolution        194 ns          193 ns     21914989
WebppSimpleCharsetSearch         210 ns          210 ns     19944187
WebppSimpleCharmapSearch         190 ns          189 ns     22543021
```
