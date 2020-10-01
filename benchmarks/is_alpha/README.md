# Is Alpha

My results:

```
2020-10-01T03:16:13-10:00
Running ./webenchpp
Run on (8 X 3400 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 1.65, 1.91, 1.95
----------------------------------------------------------------------
Benchmark                            Time             CPU   Iterations
----------------------------------------------------------------------
IsAlpha_lookup                  152067 ns       151828 ns         4712
IsAlpha_lookup_bools            154340 ns       154064 ns         4592
IsAlpha_lookup_bitset           160845 ns       160594 ns         4311
IsAlpha_arithmetic              158790 ns       158539 ns         4497
IsAlpha_arithmetic2             156812 ns       156545 ns         4492
IsAlpha_string_lookup           149604 ns       149382 ns         4726
IsAlpha_string_lookup_bools     149625 ns       149376 ns         4685
IsAlpha_string_bitset           179639 ns       179318 ns         4679
IsAlpha_string_arithmetic       184845 ns       184518 ns         3874
IsAlpha_string_arithmetic2      150131 ns       149880 ns         4587
```

Seems like arithmetic cannot beat lookup table. We are trading space in favor of speed.

Unfortunately lookup table is not implementable with SIMD or if it is, i've been told by Joel Falcou that it's not a good 
solution since it requires specific size among other reasons.