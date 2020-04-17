#include "benchmark_pch.h"

static void empty(benchmark::State& state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(_);
    }
}
BENCHMARK(empty);
