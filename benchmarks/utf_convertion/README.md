# UTF conversions benchmark

The version 2 does more things, obviously; and also the other implementation that I found somewhere, is optimizing for
ASCII strings, but they don't seem to squeeze much out of it.

Clang 18.1.8:

```
clang++ -std=c++23 -isystem /usr/local/include -L/usr/local/lib -lpthread -lbenchmark_main -lbenchmark -flto -Ofast -DNDEBUG -march=native -mtune=native utf_conversion_benchmark.cpp
2024-12-21T03:59:42-10:00
Running ./a.out
Run on (32 X 1370.7 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 2048 KiB (x16)
  L3 Unified 36864 KiB (x1)
Load Average: 1.17, 1.27, 1.28
------------------------------------------------------------------
Benchmark                        Time             CPU   Iterations
------------------------------------------------------------------
UTFConv_v1                   16192 ns        16184 ns        43201
UTFConv_v1_variant           18646 ns        18638 ns        37570
UTFConv_v2                   14719 ns        14713 ns        47801
UTFConv_v2_variant           20259 ns        20250 ns        34527
UTFConv_OtherImpl            16196 ns        16159 ns        43395
UTFConv_v1_ascii              3682 ns         3680 ns       189649
UTFConv_v2_ascii              6426 ns         6421 ns       108535
UTFConv_OtherImpl_ascii       6410 ns         6408 ns       108967
```

g++ (GCC) 14.2.1 20240910:

```
g++ -std=c++23 -isystem /usr/local/include -L/usr/local/lib -lpthread -lbenchmark_main -lbenchmark -flto -Ofast -DNDEBUG -march=native -mtune=native utf_conversion_benchmark.cpp
2024-12-21T03:59:31-10:00
Running ./a.out
Run on (32 X 4602.67 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 2048 KiB (x16)
  L3 Unified 36864 KiB (x1)
Load Average: 1.30, 1.30, 1.29
------------------------------------------------------------------
Benchmark                        Time             CPU   Iterations
------------------------------------------------------------------
UTFConv_v1                   15658 ns        15636 ns        44860
UTFConv_v1_variant           19684 ns        19674 ns        35629
UTFConv_v2                   16626 ns        16619 ns        41914
UTFConv_v2_variant           23569 ns        23560 ns        29711
UTFConv_OtherImpl            16651 ns        16642 ns        41946
UTFConv_v1_ascii              3240 ns         3236 ns       217270
UTFConv_v2_ascii              7260 ns         7255 ns        96711
UTFConv_OtherImpl_ascii       7280 ns         7269 ns        99537
```


----

```bash
$ make; ./a.out --benchmark_min_time=3s

g++ -std=c++23 -isystem /usr/local/include -L/usr/local/lib -lpthread -lbenchmark_main -lbenchmark -flto -Ofast -DNDEBUG -march=native -mtune=native utf_conversion_benchmark.cpp
2024-12-21T04:01:59-10:00
Running ./a.out
Run on (32 X 1515.99 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 2048 KiB (x16)
  L3 Unified 36864 KiB (x1)
Load Average: 0.54, 0.99, 1.17
------------------------------------------------------------------
Benchmark                        Time             CPU   Iterations
------------------------------------------------------------------
UTFConv_v1                   15361 ns        15353 ns       273867
UTFConv_v1_variant           19581 ns        19573 ns       215039
UTFConv_v2                   16988 ns        16978 ns       246694
UTFConv_v2_variant           23369 ns        23350 ns       179951
UTFConv_OtherImpl            16797 ns        16790 ns       248423
UTFConv_v1_ascii              3204 ns         3202 ns      1292673
UTFConv_v2_ascii              7269 ns         7266 ns       578137
UTFConv_OtherImpl_ascii       7241 ns         7235 ns       577496
```
