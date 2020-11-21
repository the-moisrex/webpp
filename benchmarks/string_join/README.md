# String Join

The current result seems to be a tie; this is good because that's what we want, we want the `join`'s abstractions not slow it down.

```
2020-11-20T16:13:57-10:00
Running ./a.out
Run on (8 X 3400 MHz CPU s)
CPU Caches:
L1 Data 32 KiB (x4)
L1 Instruction 32 KiB (x4)
L2 Unified 256 KiB (x4)
L3 Unified 6144 KiB (x1)
Load Average: 2.04, 1.97, 1.52
--------------------------------------------------------
Benchmark              Time             CPU   Iterations
--------------------------------------------------------
StrJoin_Webpp     307711 ns       305153 ns         2307
StrJoin_STD       307257 ns       305990 ns         2254
```
