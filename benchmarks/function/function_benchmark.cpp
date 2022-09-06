#include "../../core/include/webpp/std/functional.hpp"
#include "../benchmark.hpp"

using namespace webpp;

static void STLFunctionCall(benchmark::State& state) {
    std::function<int()> func = [i = 0]() mutable {
        return ++i;
    };
    for (auto _ : state) {
        benchmark::DoNotOptimize(func());
    }
}
BENCHMARK(STLFunctionCall);

static void ISTLFunctionCall(benchmark::State& state) {
    istl::function<int()> func = [i = 0]() mutable {
        return ++i;
    };
    for (auto _ : state) {
        benchmark::DoNotOptimize(func());
    }
}
BENCHMARK(ISTLFunctionCall);
