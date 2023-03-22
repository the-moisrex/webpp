#include "../../webpp/convert/casts.hpp"
#include "../benchmark.hpp"

using namespace webpp;

static void StrToInt_to_int(benchmark::State& state) {
    const auto   strs = str_array_generator<200>(5, "123456789");
    unsigned int i    = 0;
    for (auto _ : state) {
        const int res = webpp::to_int(strs[i++ % strs.size()]);
        benchmark::DoNotOptimize(res);
    }
}
BENCHMARK(StrToInt_to_int);

static void StrToInt_stoi(benchmark::State& state) {
    const auto   strs = str_array_generator<200>(5, "123456789");
    unsigned int i    = 0;
    for (auto _ : state) {
        const int res = std::stoi(strs[i++ % strs.size()]);
        benchmark::DoNotOptimize(res);
    }
}
BENCHMARK(StrToInt_stoi);

static void StrToInt_to_uint64(benchmark::State& state) {
    const auto   strs = str_array_generator<200>(10, "123456789");
    unsigned int i    = 0;
    for (auto _ : state) {
        const auto res = webpp::to_uint64(strs[i++ % strs.size()]);
        benchmark::DoNotOptimize(res);
    }
}
BENCHMARK(StrToInt_to_uint64);

static void StrToInt_stoull(benchmark::State& state) {
    const auto   strs = str_array_generator<200>(10, "123456789");
    unsigned int i    = 0;
    for (auto _ : state) {
        const auto res = std::stoull(strs[i++ % strs.size()]);
        benchmark::DoNotOptimize(res);
    }
}
BENCHMARK(StrToInt_stoull);
