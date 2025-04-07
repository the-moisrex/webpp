# Unicode's Validity Criteria Benchmark

Clang version 19.1.7:

```bash
/validity_criteria ❯❯❯ ./a.out --benchmark_min_time=3s
2025-04-07T03:50:19-10:00
Running ./a.out
Run on (32 X 6000 MHz CPU s)
CPU Caches:
L1 Data 48 KiB (x16)
L1 Instruction 32 KiB (x16)
L2 Unified 2048 KiB (x16)
L3 Unified 36864 KiB (x1)
Load Average: 1.81, 2.33, 2.08
----------------------------------------------------------------
Benchmark                      Time             CPU   Iterations
----------------------------------------------------------------
WebppValidityCriteria   12854956 ns     12846816 ns          326
AdaValidityCriteria     98309569 ns     98190802 ns           43
► [ 0 ] [ 10s996 | Apr 07 03:50AM ]
```

gcc (GCC) 14.2.1 20250207:

```bash
/validity_criteria ❯❯❯ ./a.out --benchmark_min_time=3s
2025-04-07T03:50:39-10:00
Running ./a.out
Run on (32 X 6000 MHz CPU s)
CPU Caches:
L1 Data 48 KiB (x16)
L1 Instruction 32 KiB (x16)
L2 Unified 2048 KiB (x16)
L3 Unified 36864 KiB (x1)
Load Average: 1.81, 2.29, 2.07
----------------------------------------------------------------
Benchmark                      Time             CPU   Iterations
----------------------------------------------------------------
WebppValidityCriteria   32226241 ns     32159718 ns          130
AdaValidityCriteria    110736212 ns    110660905 ns           38
► [ 0 ] [ 10s032 | Apr 07 03:50AM ]
```

Clearly Web++'s version wins, though, Validity Criteria is on going to be run for most URLs, so ada-url intentionally
have tried to keep things simple with save some binary size.
