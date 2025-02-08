# inet_ntop6_size

gcc (GCC) 14.2.1 20250128:
```
g++ -std=c++23 -isystem /usr/local/include -L/usr/local/lib -lpthread -lbenchmark_main -lbenchmark -flto -Ofast -DNDEBUG -march=native inet_ntop6_size_benchmark.cpp
2025-02-07T22:20:22-10:00
Running ./a.out
Run on (32 X 6000 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 2048 KiB (x16)
  L3 Unified 36864 KiB (x1)
Load Average: 1.81, 2.46, 2.24
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------
Benchmark            Time             CPU   Iterations
------------------------------------------------------
IPV6Size_v1       2920 ns         2913 ns       242471
IPV6Size_v2       2459 ns         2457 ns       287703
IPV6Size_v3       3355 ns         3353 ns       221919
IPV6Size_v4       2787 ns         2784 ns       252730
IPV6Size_v5       2791 ns         2788 ns       251178
IPV6Size_v6       2838 ns         2834 ns       248369
IPV6Size_v7       2849 ns         2845 ns       244781
IPV6Size_v8       2842 ns         2840 ns       248805
```