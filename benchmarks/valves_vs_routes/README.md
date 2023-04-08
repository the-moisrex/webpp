## Valves Vs. Routes

I did not expect for the valves to win this easily. It's amazing how much time I spent
coding the static routes and yet here we are, the valves are much better.


```
2023-04-07T21:02:57-08:00
Running ./a.out
Run on (8 X 3400 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 12.33, 12.33, 9.64
---------------------------------------------------------
Benchmark               Time             CPU   Iterations
---------------------------------------------------------
ValveBench           1.53 ns         1.53 ns    504587228
RouteBench            112 ns          111 ns      5988546
ValveCallBench      0.759 ns        0.759 ns    906909368
RouteCallBench        109 ns          109 ns      6458261

```

```
2023-04-07T21:08:40-08:00
Running ./a.out
Run on (8 X 3284.74 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 3.33, 7.36, 8.25
-----------------------------------------------------------
Benchmark                 Time             CPU   Iterations
-----------------------------------------------------------
ValveBench             1.40 ns         1.40 ns    552680071
RouteBench              109 ns          109 ns      6495120
ValveCallBench        0.695 ns        0.695 ns    984053788
RouteCallBench          105 ns          105 ns      6655415
ValveStringBench       44.1 ns         44.0 ns     16195484
RouteStringBench        149 ns          149 ns      4634254
```
