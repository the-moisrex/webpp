# inet_ntop6_size

gcc (GCC) 14.2.1 20250128:
```
g++ -std=c++23 -isystem /usr/local/include -L/usr/local/lib -lpthread -lbenchmark_main -lbenchmark -flto -Ofast -DNDEBUG -march=native inet_ntop6_size_benchmark.cpp
2025-02-08T13:12:52-10:00
Running ./a.out
Run on (32 X 6000 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 2048 KiB (x16)
  L3 Unified 36864 KiB (x1)
Load Average: 2.75, 2.69, 2.26
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
--------------------------------------------------------------
Benchmark                    Time             CPU   Iterations
--------------------------------------------------------------
IPV6Size_v1                662 ns          662 ns       931841
IPV6Size_v2                474 ns          474 ns      1473494
IPV6Size_v3                786 ns          786 ns       890860
IPV6Size_v4                501 ns          501 ns      1407873
IPV6Size_v5                503 ns          502 ns      1382336
IPV6Size_v6                459 ns          459 ns      1524406
IPV6Size_v7                444 ns          443 ns      1576501
IPV6Size_v8                474 ns          473 ns      1498980
IPV6Size_v9                672 ns          670 ns      1086219
IPV6Size_v10               571 ns          571 ns      1217083
IPV6Size_v11               632 ns          632 ns      1102085
IPV6Size_v12               958 ns          958 ns       722842
IPV6Size_v13               507 ns          506 ns      1393714
IPV6Size_v14               463 ns          463 ns      1509304
IPV6Size_v15               716 ns          715 ns       984187
IPV6Size_v16               543 ns          543 ns      1289236
IPV6Size_badV17            870 ns          869 ns       806148
IPV6Size_StringSize       1940 ns         1939 ns       360144
```


clang version 19.1.7
```
clang++ -std=c++23 -isystem /usr/local/include -L/usr/local/lib -lpthread -lbenchmark_main -lbenchmark -flto -Ofast -DNDEBUG -march=native inet_ntop6_size_benchmark.cpp
clang++: warning: argument '-Ofast' is deprecated; use '-O3 -ffast-math' for the same behavior, or '-O3' to enable only conforming optimizations [-Wdeprecated-ofast]
In file included from inet_ntop6_size_benchmark.cpp:1:
In file included from ./../../webpp/ip/ipv6.hpp:5:
In file included from ./../../webpp/ip/../strings/append.hpp:4:
In file included from ./../../webpp/ip/../strings/../std/string.hpp:7:
./../../webpp/ip/../strings/../std/../std/type_traits.hpp:804:22: warning: pack indexing is a C++2c extension [-Wc++26-extensions]
  804 |         using type = T...[sizeof...(T) - 1];
      |                      ^
1 warning generated.
2025-02-08T13:13:48-10:00
Running ./a.out
Run on (32 X 6000 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 2048 KiB (x16)
  L3 Unified 36864 KiB (x1)
Load Average: 2.63, 2.66, 2.27
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
--------------------------------------------------------------
Benchmark                    Time             CPU   Iterations
--------------------------------------------------------------
IPV6Size_v1                911 ns          910 ns       767953
IPV6Size_v2                713 ns          712 ns       986161
IPV6Size_v3                698 ns          697 ns      1004511
IPV6Size_v4                824 ns          824 ns       839728
IPV6Size_v5                814 ns          814 ns       861550
IPV6Size_v6                753 ns          752 ns       928623
IPV6Size_v7               1008 ns         1007 ns       687222
IPV6Size_v8                719 ns          719 ns       994422
IPV6Size_v9                556 ns          556 ns      1266878
IPV6Size_v10               610 ns          610 ns      1157498
IPV6Size_v11               627 ns          626 ns      1136885
IPV6Size_v12               804 ns          804 ns       873322
IPV6Size_v13               515 ns          514 ns      1361504
IPV6Size_v14               515 ns          515 ns      1243677
IPV6Size_v15               524 ns          523 ns      1333864
IPV6Size_v16               543 ns          542 ns      1277986
IPV6Size_badV17            762 ns          761 ns       919315
IPV6Size_StringSize       1967 ns         1965 ns       360643
```

We're going with `v14` for now.