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


### Constexpr friendly
`inet_pton6` used to use `memmove` and `memset`; I changed that to my own algo so it would be constexpr.

The results:

```
./a.out --benchmark_filter="v6_random"
2023-06-13T09:14:38-08:00
Running ./a.out
Run on (8 X 3253.44 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 2.66, 6.05, 6.09
---------------------------------------------------------------------
Benchmark                           Time             CPU   Iterations
---------------------------------------------------------------------
IP_asio_v6_random                61.0 ns         60.8 ns     12870159
IP_webpp_v6_random               51.5 ns         51.5 ns     13885846
IP_webpp_v6_random_normal        48.4 ns         48.3 ns     14232084 <-- new algo
IP_webpp_v6_random_memmove       53.9 ns         53.6 ns     13064038 <-- what it used to be
```

unfortunately for clang, it's the opposite:

```
IP_webpp_v6_random_normal        47.1 ns         47.0 ns     14871149
IP_webpp_v6_random_memmove       44.2 ns         44.2 ns     15820764
```

I changed it again and now it doesn't seem to be the best, but it's cleaner:

```
IP_asio_v6_random                66.4 ns         66.3 ns     12001580
IP_webpp_v6_random               57.0 ns         56.9 ns     12541255
IP_webpp_v6_random_normal        56.2 ns         56.1 ns     12476946 <-- new new algo
IP_webpp_v6_random_manual        55.4 ns         55.3 ns     12059806
IP_webpp_v6_random_memmove       53.2 ns         53.1 ns     12919261
```


### IPv4 Host Parser vs inet_pton4

They are different parsers.

The `webpp_host` are the IPv4 host parsers:

GCC:
```
./a.out --benchmark_filter=v4                                                                                dev ✚ ✱ ◼
2023-12-30T14:47:07-08:00
Running ./a.out
Run on (32 X 5499.53 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 2048 KiB (x16)
  L3 Unified 36864 KiB (x1)
Load Average: 2.02, 2.61, 2.91
---------------------------------------------------------------------------
Benchmark                                 Time             CPU   Iterations
---------------------------------------------------------------------------
IP_asio_v4                             10.3 ns         10.3 ns     67726407
IP_webpp_v4                           0.546 ns        0.546 ns   1281457446
IP_webpp_host_v4                  -->  7.56 ns         7.56 ns     92266146
IP_asio_v4_random                      15.8 ns         15.8 ns     44288629
IP_webpp_v4_random                     12.0 ns         12.0 ns     59213399
IP_webpp_host_v4_random           -->  9.49 ns         9.49 ns     70555190
IP_webpp_v4_random_inet_pton4_v1       12.1 ns         12.1 ns     58103551
IP_webpp_v4_random_inet_pton4_v2       13.4 ns         13.4 ns     52157009
IP_webpp_v4_random_ipv4_v1             16.9 ns         16.9 ns     42716760
IP_webpp_v4_random_ipv4_v2             11.3 ns         11.3 ns     61946371
ipv4_prefix_parsing_v1                 14.8 ns         14.8 ns     47545510
ipv4_prefix_parsing_v2                 12.2 ns         12.2 ns     57685667
```

Clang:

```
./a.out --benchmark_filter=v4                                                                                dev ✚ ✱ ◼
2023-12-30T14:45:04-08:00
Running ./a.out
Run on (32 X 5500.11 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 2048 KiB (x16)
  L3 Unified 36864 KiB (x1)
Load Average: 2.69, 2.91, 3.04
---------------------------------------------------------------------------
Benchmark                                 Time             CPU   Iterations
---------------------------------------------------------------------------
IP_asio_v4                             12.2 ns         12.2 ns     58456527
IP_webpp_v4                            7.31 ns         7.30 ns     95780319
IP_webpp_host_v4                 -->   9.91 ns         9.90 ns     71235842
IP_asio_v4_random                      16.0 ns         16.0 ns     43664800
IP_webpp_v4_random                     12.4 ns         12.4 ns     56222317
IP_webpp_host_v4_random          -->   13.0 ns         13.0 ns     53660340
IP_webpp_v4_random_inet_pton4_v1       12.5 ns         12.5 ns     56104611
IP_webpp_v4_random_inet_pton4_v2       12.7 ns         12.7 ns     54273108
IP_webpp_v4_random_ipv4_v1             18.3 ns         18.2 ns     38370496
IP_webpp_v4_random_ipv4_v2             10.9 ns         10.9 ns     63444557
ipv4_prefix_parsing_v1                 16.5 ns         16.5 ns     42504564
ipv4_prefix_parsing_v2                 10.7 ns         10.7 ns     65534687
```

It seems like the IPv4 HOST Parser is winning in GCC which 
it not losing is completely surprises me since it needs to do more.