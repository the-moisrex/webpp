# inet_ntop6_size

gcc (GCC) 14.2.1 20250128:
```
g++ -std=c++23 -isystem /usr/local/include -L/usr/local/lib -lpthread -lbenchmark_main -lbenchmark -flto -Ofast -DNDEBUG -march=native inet_ntop6_size_benchmark.cpp
2025-02-07T22:27:26-10:00
Running ./a.out
Run on (32 X 6000 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 2048 KiB (x16)
  L3 Unified 36864 KiB (x1)
Load Average: 4.55, 2.67, 2.33
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
--------------------------------------------------------------
Benchmark                    Time             CPU   Iterations
--------------------------------------------------------------
IPV6Size_v1               2936 ns         2934 ns       236846
IPV6Size_v2               2442 ns         2440 ns       289943
IPV6Size_v3               3212 ns         3205 ns       223109
IPV6Size_v4               2757 ns         2752 ns       251360
IPV6Size_v5               2886 ns         2883 ns       253253
IPV6Size_v6               2827 ns         2825 ns       248168
IPV6Size_v7               2828 ns         2826 ns       247929
IPV6Size_v8               2826 ns         2824 ns       244691
IPV6Size_StringSize       4492 ns         4487 ns       156482
```