# SIMD string == equals
Based on this results, seems like `strcmp` and `std::string::opreator==` either use __SIMD__ inside their implementations
or GCC and Clang are able to vectorize those algorithms at the optimization level which leaves us with not implementing our
own vectorized versions of those algorithms (even though we've already implemented it in the benchmarks).

Even though I already knew this result, I was curious if I could match the strcmp's implementation with eve proejct or not.

Results on my noisy machine:

```
Run on (8 X 3400 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 1.58, 1.94, 1.83
---------------------------------------------------------------------------
Benchmark                                 Time             CPU   Iterations
---------------------------------------------------------------------------
EQ_STD                               332927 ns       332653 ns         2082
EQ_STRcmp                            335086 ns       334724 ns         2092
EQ_SIMD                              334547 ns       334265 ns         2090
```