#include "../../webpp/unicode/idna.hpp"
#include "../benchmark.hpp"
#include "../common_utils_pch.hpp"
#include "./ada-validity.hpp"

static auto strs = str32_array_generator<1000>();

static void WebppValidityCriteria(benchmark::State& state) {
    for (auto _ : state) {
        for (auto const str : strs) {
            auto rhs = webpp::unicode::idna::is_label_valid(str.begin(), str.end());
            benchmark::DoNotOptimize(rhs);
        }
    }
}

BENCHMARK(WebppValidityCriteria);

static void AdaValidityCriteria(benchmark::State& state) {
    for (auto _ : state) {
        for (auto const str : strs) {
            auto rhs = ada::idna::is_label_valid(str);
            benchmark::DoNotOptimize(rhs);
        }
    }
}

BENCHMARK(AdaValidityCriteria);
