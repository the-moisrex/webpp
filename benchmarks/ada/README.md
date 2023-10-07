# Ada URL vs Web++'s implementation of URL

As of writing this, ada's url parser has more features than our implementation. In an effort to better our impl, we're
going to benchmark our features against theirs.


GCC:

```
2023-10-07T07:33:24-08:00
Running ./a.out
Run on (8 X 3176.74 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 2.37, 4.32, 4.17
----------------------------------------------------------
Benchmark                Time             CPU   Iterations
----------------------------------------------------------
WebppSchemePort       10.6 ns         10.5 ns     70043092
AdaSchemePort         12.6 ns         12.5 ns     46552076
```

Clang:

```
2023-10-07T07:32:37-08:00
Load Average: 2.60, 4.65, 4.27
----------------------------------------------------------
Benchmark                Time             CPU   Iterations
----------------------------------------------------------
WebppSchemePort       6.24 ns         6.23 ns    115267454
AdaSchemePort         8.74 ns         8.67 ns     83124377
```
