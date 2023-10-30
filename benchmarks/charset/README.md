# Charset benchmarks

Charmap clearly wins; I'm not sure why ada-url implementers used such a way instead of doing a simple table lookup.

Not so much interestingly the charset version loses completely.

Bitmap version is a bit behind the charmap version; as of the time that this benchmark was conducted, `std::bitset` 
is not constexpr in the STL implementations used in this benchmark, so that might affect the benchmark; But also,
bitmap does bit-manipulations, so that might be the cause of this.

#### With 10 characters:

g++ (GCC) 13.2.1 20230801

```
./a.out --benchmark_min_time=3s
2023-10-30T05:53:44-08:00
Running ./a.out
Run on (8 X 3201.65 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 1.64, 2.91, 3.04
--------------------------------------------------------------------
Benchmark                          Time             CPU   Iterations
--------------------------------------------------------------------
AdaAutoVectorizedSolution       22.6 ns         22.6 ns    186376912
WebppSimpleCharsetSearch        25.2 ns         25.2 ns    163516315
WebppSimpleCharmapSearch        13.2 ns         13.1 ns    320400204
WebppSimpleBitmapSearch         13.6 ns         13.6 ns    305338332
```

clang version 16.0.6

```
./a.out --benchmark_min_time=3s
2023-10-30T05:53:10-08:00
Running ./a.out
Run on (8 X 3248.83 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 2.09, 3.13, 3.12
--------------------------------------------------------------------
Benchmark                          Time             CPU   Iterations
--------------------------------------------------------------------
AdaAutoVectorizedSolution       21.9 ns         21.9 ns    192440944
WebppSimpleCharsetSearch        20.2 ns         20.2 ns    209551601
WebppSimpleCharmapSearch        11.8 ns         11.8 ns    358754110
WebppSimpleBitmapSearch         13.0 ns         13.0 ns    324838030
```


#### With 5000 characters

GCC: 
```
 ./a.out --benchmark_min_time=3s
2023-10-30T05:55:18-08:00
Running ./a.out
Run on (8 X 2594.23 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 1.52, 2.50, 2.89
--------------------------------------------------------------------
Benchmark                          Time             CPU   Iterations
--------------------------------------------------------------------
AdaAutoVectorizedSolution        234 ns          234 ns     18025264
WebppSimpleCharsetSearch         245 ns          245 ns     17182933
WebppSimpleCharmapSearch         222 ns          222 ns     18956118
WebppSimpleBitmapSearch          223 ns          223 ns     18686717
```

Clang:

```
./a.out --benchmark_min_time=3s
2023-10-30T05:56:17-08:00
Running ./a.out
Run on (8 X 3400 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 1.74, 2.40, 2.82
--------------------------------------------------------------------
Benchmark                          Time             CPU   Iterations
--------------------------------------------------------------------
AdaAutoVectorizedSolution        207 ns          207 ns     20237923
WebppSimpleCharsetSearch         222 ns          221 ns     19055441
WebppSimpleCharmapSearch         203 ns          202 ns     20762960
WebppSimpleBitmapSearch          202 ns          201 ns     20885804
```
