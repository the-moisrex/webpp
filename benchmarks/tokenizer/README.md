# Benchmark for Manual Parsing vs. String Tokenizer

Unfortunately, in v1 the string_tokenizer is slower than manual implementation that I've done long time ago.

```
g++ -std=c++20 -isystem /usr/local/include -L/usr/local/lib -lpthread -lbenchmark_main -lbenchmark -flto -Ofast -DNDEBUG -march=native -mtune=native tokenizer_benchmark.cpp
2023-05-03T12:07:48-08:00
Running ./a.out
Run on (8 X 2017.76 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 6.12, 4.91, 3.85
------------------------------------------------------------
Benchmark                  Time             CPU   Iterations
------------------------------------------------------------
StrViewParser            229 ns          228 ns      3688433
TokenizerParserV1        452 ns          450 ns      1502743
TokenizerParserV2        328 ns          326 ns      1939593
```
