# liburlparser benchmark


### Against my `uri/uri_string` URI parser:

We're winning here mostly because `liburiparser`'s current implementation uses `std::string`
and we use `std::string_view` by default (which is totally changable).

```
2023-06-28T16:33:02-08:00
Running ./a.out
Run on (8 X 3294.09 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 6144 KiB (x1)
Load Average: 0.91, 1.74, 2.49
--------------------------------------------------------------
Benchmark                    Time             CPU   Iterations
--------------------------------------------------------------
WebppURIString_Host       76.7 ns         76.2 ns      9562358
URIParser_Host             317 ns          315 ns      2235739
WebppURIString_All         182 ns          181 ns      3850017
URIParser_All              740 ns          739 ns       872405
```
