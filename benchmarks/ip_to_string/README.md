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
