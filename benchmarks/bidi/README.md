# Unicode Bidirectional Benchmark

GCC 14.2.1:
```
$ make; ./a.out
g++ -std=c++23 -isystem /usr/local/include -L/usr/local/lib -lpthread -lbenchmark_main -lbenchmark -flto -Ofast -DNDEBUG -march=native -mtune=native bidi_benchmark.cpp
2025-03-28T17:10:48-10:00
Running ./a.out
Run on (32 X 6000 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 2048 KiB (x16)
  L3 Unified 36864 KiB (x1)
Load Average: 2.12, 2.27, 2.32
-----------------------------------------------------------
Benchmark                 Time             CPU   Iterations
-----------------------------------------------------------
BidiBinarySearch    5614498 ns      5611363 ns          125
BidiTableLookup      656082 ns       655816 ns         1041

[ 2s215 | Mar 28 05:10PM ]
```

Clang 19.1.7:
```
$ make clang; ./a.out
clang++ -std=c++23 -isystem /usr/local/include -L/usr/local/lib -lpthread -lbenchmark_main -lbenchmark -flto -Ofast -DNDEBUG -march=native -mtune=native bidi_benchmark.cpp
clang++: warning: argument '-Ofast' is deprecated; use '-O3 -ffast-math' for the same behavior, or '-O3' to enable only conforming optimizations [-Wdeprecated-ofast]
2025-03-28T17:10:57-10:00
Running ./a.out
Run on (32 X 6000 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 2048 KiB (x16)
  L3 Unified 36864 KiB (x1)
Load Average: 2.24, 2.30, 2.32
-----------------------------------------------------------
Benchmark                 Time             CPU   Iterations
-----------------------------------------------------------
BidiBinarySearch    4929691 ns      4926069 ns          142
BidiTableLookup      818178 ns       817601 ns          856
```

Table lookup clearly wins, and that's to be expected.