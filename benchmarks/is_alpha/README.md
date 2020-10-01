# Is Alpha

Seems like arithmetic cannot beat lookup table. We are trading space in favor of speed.

Unfortunately lookup table is not implementable with SIMD or if it is, i've been told 
by Joel Falcou and others that it's not a good 
solution since it requires specific size among other reasons.

Results on my machine:

```
./webenchpp --benchmark_filter="IsAlpha" --benchmark_report_aggregates_only=true --benchmark_repetitions=10            
2020-10-01T06:41:55-10:00
Running ./webenchpp
Run on (8 X 3400 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 1.12, 1.29, 1.07
-----------------------------------------------------------------------------
Benchmark                                   Time             CPU   Iterations
-----------------------------------------------------------------------------
IsAlpha_lookup_mean                      3546 ns         3540 ns           10
IsAlpha_lookup_median                    3529 ns         3523 ns           10
IsAlpha_lookup_stddev                    72.0 ns         71.1 ns           10
IsAlpha_string_lookup_mean               5.83 ns         5.82 ns           10
IsAlpha_string_lookup_median             5.85 ns         5.84 ns           10
IsAlpha_string_lookup_stddev            0.101 ns        0.101 ns           10
IsAlpha_lookup_bools_mean                3554 ns         3549 ns           10
IsAlpha_lookup_bools_median              3554 ns         3549 ns           10
IsAlpha_lookup_bools_stddev              23.5 ns         23.3 ns           10
IsAlpha_string_lookup_bools_mean         8.37 ns         8.36 ns           10
IsAlpha_string_lookup_bools_median       8.36 ns         8.35 ns           10
IsAlpha_string_lookup_bools_stddev      0.114 ns        0.114 ns           10
IsAlpha_bitset_mean                     10675 ns        10661 ns           10
IsAlpha_bitset_median                   10672 ns        10657 ns           10
IsAlpha_bitset_stddev                    43.8 ns         43.3 ns           10
IsAlpha_string_bitset_mean               2.87 ns         2.86 ns           10
IsAlpha_string_bitset_median             2.87 ns         2.86 ns           10
IsAlpha_string_bitset_stddev            0.013 ns        0.013 ns           10
IsAlpha_arithmetic_mean                  7399 ns         7388 ns           10
IsAlpha_arithmetic_median                7390 ns         7379 ns           10
IsAlpha_arithmetic_stddev                61.3 ns         60.5 ns           10
IsAlpha_string_arithmetic_mean           6.24 ns         6.24 ns           10
IsAlpha_string_arithmetic_median         6.28 ns         6.27 ns           10
IsAlpha_string_arithmetic_stddev        0.102 ns        0.103 ns           10
IsAlpha_arithmetic2_mean                 8371 ns         8354 ns           10
IsAlpha_arithmetic2_median               8361 ns         8344 ns           10
IsAlpha_arithmetic2_stddev               27.5 ns         27.6 ns           10
IsAlpha_string_arithmetic2_mean          2.05 ns         2.05 ns           10
IsAlpha_string_arithmetic2_median        2.08 ns         2.07 ns           10
IsAlpha_string_arithmetic2_stddev       0.074 ns        0.074 ns           10
IsAlpha_arithmetic3_mean                 8114 ns         8103 ns           10
IsAlpha_arithmetic3_median               8119 ns         8107 ns           10
IsAlpha_arithmetic3_stddev               18.7 ns         18.6 ns           10
IsAlpha_string_arithmetic3_mean          6.98 ns         6.97 ns           10
IsAlpha_string_arithmetic3_median        6.96 ns         6.95 ns           10
IsAlpha_string_arithmetic3_stddev       0.075 ns        0.074 ns           10
```

## My conclusion (which might be wrong)
The lookup table due to its nature, requires more cache, which means it's not going
to perform better in a row of strings but it does perform better when the cache is free
to use.

The arithmetic3 which someone on cpplang's slack proposed and told us that it's 1.3 times better
than table lookup in MSVC but in GCC with all the cool flags, I don't see it beating arithmetic2 which
I wrote and seem to perform better in string mode but not in the other mode.

I'm still not convinced this benchmark is the best benchmark. I don't understand why there's so much difference
in both modes. I suspect some magic optimization is going on there :)

## SIMD
I'm still working on the SIMD version or versions.