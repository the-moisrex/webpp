# Interleaving bits benchmark

There are multiple implementations of **interleaving bits** in [Bit Twiddling Hacks](https://graphics.stanford.edu/~seander/bithacks.html#InterleaveTableObvious).


GCC 14.1.1:

```
2024-08-02T13:02:30-10:00
Running ./a.out
Run on (32 X 6000 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 2048 KiB (x16)
  L3 Unified 36864 KiB (x1)
Load Average: 1.46, 1.36, 1.25
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------
Benchmark                  Time             CPU   Iterations
------------------------------------------------------------
obvious_way             4.42 ns         4.42 ns    159941082
table_lookup_way       0.594 ns        0.593 ns   1175142795
multiply_way           0.729 ns        0.728 ns    961803843
magic_numbers_way       6.36 ns         6.35 ns    110044113
```

Clang 18.1.8:
```
2024-08-02T13:05:13-10:00
Running ./a.out
Run on (32 X 6000 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 2048 KiB (x16)
  L3 Unified 36864 KiB (x1)
Load Average: 1.07, 1.26, 1.23
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------
Benchmark                  Time             CPU   Iterations
------------------------------------------------------------
obvious_way             2.98 ns         2.97 ns    234825142
table_lookup_way       0.613 ns        0.613 ns   1137160332
multiply_way           0.729 ns        0.728 ns    962041816
magic_numbers_way       1.53 ns         1.53 ns    458450797
```


Clearly table lookup wins.