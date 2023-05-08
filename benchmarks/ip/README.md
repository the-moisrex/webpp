# inet_pton benchmarks (IPv4 and IPv6)

### Parsing Prefix shows an improvement to use inet_pton

v2 is using my implementation of inet_pton4, while v1 is using my old implementation.

G++ results:
```
./a.out --benchmark_min_time=5 --benchmark_filter="prefix"
2023-05-08T06:59:13-08:00
Running ./a.out
Run on (8 X 3210.82 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 6.69, 6.28, 4.34
-----------------------------------------------------------------
Benchmark                       Time             CPU   Iterations
-----------------------------------------------------------------
ipv4_prefix_parsing_v1       45.3 ns         45.1 ns    173741697
ipv4_prefix_parsing_v2       41.1 ns         40.9 ns    174487456
```

Clang results:

```
./a.out --benchmark_min_time=5 --benchmark_filter="prefix"
2023-05-08T07:01:46-08:00
Running ./a.out
Run on (8 X 3247.71 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 2.95, 5.25, 4.27
-----------------------------------------------------------------
Benchmark                       Time             CPU   Iterations
-----------------------------------------------------------------
ipv4_prefix_parsing_v1       52.6 ns         52.4 ns    137399814
ipv4_prefix_parsing_v2       32.8 ns         32.7 ns    219151299
```
