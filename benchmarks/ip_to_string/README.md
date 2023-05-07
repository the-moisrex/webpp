## GLibc inet_ntop4 vs my implementation

My version wins by a lot; but the code is not pretty.

```
2023-05-04T13:13:01-08:00
Running ./a.out
Run on (8 X 2294.69 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 3.30, 2.84, 2.75
-------------------------------------------------------
Benchmark             Time             CPU   Iterations
-------------------------------------------------------
IP2StrGlibc       33253 ns        33086 ns        20603
IP2StrManual       2169 ns         2157 ns       342514
```


This benchmark has a HeisenBug in it; I don't think the `IPv6ToStrManual` is a good implementation, but it has
some good ideas in it.

```
2023-05-04T14:59:49-08:00
Running /webpp/benchmarks/ip_to_string/a.out
Run on (8 X 2494.22 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 2.64, 3.43, 3.57
----------------------------------------------------------
Benchmark                Time             CPU   Iterations
----------------------------------------------------------
IPv4ToStrGlibc       22727 ns        22598 ns        31080
IPv4ToStrManual       1138 ns         1136 ns       598129
IPv6ToStrGlibc        1210 ns         1203 ns       567275
IPv6ToStrManual        103 ns          103 ns      6890777  <--- Segfault here
```

Apple (BSD) version included:

```
2023-05-05T07:29:49-08:00
Running /webpp/benchmarks/ip_to_string/a.out
Run on (8 X 3400 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x8)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 4.84, 3.96, 3.30
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------
Benchmark                Time             CPU   Iterations
----------------------------------------------------------
IPv4ToStrApple       22148 ns        22096 ns        30505
IPv4ToStrGlibc       25561 ns        25503 ns        27001
IPv4ToStrManual       1235 ns         1232 ns       556210
IPv6ToStrApple         803 ns          801 ns       887275
IPv6ToStrGlibc        1376 ns         1372 ns       507648
IPv6ToStrManual        119 ns          119 ns      5852012  <--- Segfault here
```


### Version 2 is the fastest

Honestly, I don't understand why these 2 great libraries have managed to write the least performant algorithms
they possibly could!

```
./a.out --benchmark_min_time=5                           
2023-05-06T13:47:35-08:00
Running ./a.out
Run on (8 X 3400 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 5.02, 6.10, 5.86
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------
Benchmark                  Time             CPU   Iterations
------------------------------------------------------------
IPv4ToStrApple         15626 ns        15605 ns       454591
IPv4ToStrGlibc         18112 ns        18084 ns       388725
IPv4ToStrManual          920 ns          919 ns      7522516
IPv6ToStrApple           599 ns          598 ns     11804761
IPv6ToStrGlibc          1053 ns         1051 ns      6939659
IPv6ToStrManualV2        248 ns          247 ns     28022775
```


Also: v4 tests are slower than v6 tests because they do more tests, don't compare v4s to v6es.


### inet_ntop6 v3

```
./a.out --benchmark_min_time=5 --benchmark_filter=6
2023-05-07T08:21:28-08:00
Running ./a.out
Run on (8 X 3193.18 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 2.69, 4.47, 5.01
------------------------------------------------------------
Benchmark                  Time             CPU   Iterations
------------------------------------------------------------
IPv6ToStrApple         11144 ns        11082 ns       635092
IPv6ToStrGlibc         32045 ns        31816 ns       224492
IPv6ToStrManualV2       5521 ns         5496 ns      1272118
IPv6ToStrManualV3       9718 ns         9686 ns       713139
```

I don't know why it's slower than v2!!!
