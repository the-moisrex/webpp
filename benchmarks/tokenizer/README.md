# Benchmark for Manual Parsing vs. String Tokenizer

Unfortunately, in v1 the string_tokenizer is slower than manual implementation that I've done long time ago.

```
g++ -std=c++20 -isystem /usr/local/include -L/usr/local/lib -lpthread -lbenchmark_main -lbenchmark -flto -Ofast -DNDEBUG -march=native -mtune=native tokenizer_benchmark.cpp
2023-05-03T07:43:35-08:00
Running ./a.out
Run on (8 X 3266.45 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 2.08, 2.95, 3.45
----------------------------------------------------------
Benchmark                Time             CPU   Iterations
----------------------------------------------------------
StrViewParser          127 ns          127 ns      5633052
TokenizerParser        302 ns          301 ns      2279708
```
