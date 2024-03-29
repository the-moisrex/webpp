#include "../benchmark.hpp"
#include "../common_utils_pch.hpp"
#include "./ada_find_char.hpp"
#include "./charset_v1.hpp"

#include <vector>


auto const strs = str_array_generator<200>(5000, "123456789abcdefghijklmopqrstuvwzyz:/?[");

void AdaAutoVectorizedSolution(benchmark::State& state) {
    std::size_t i = 0;
    for (auto _ : state) {
        auto str = strs[i++ % strs.size()];
        auto res = ada_v1::find_next_host_delimiter(str, 0);
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(str);
    }
}

BENCHMARK(AdaAutoVectorizedSolution);


static constexpr auto v1_chars = webpp::charset_v1::charset<char, 4>{':', '/', '?', '['};

void WebppSimpleCharsetSearch(benchmark::State& state) {
    std::size_t i = 0;
    for (auto _ : state) {
        auto str = strs[i++ % strs.size()];
        auto res = v1_chars.find_first_of(str.begin(), str.end());
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(str);
    }
}

BENCHMARK(WebppSimpleCharsetSearch);


static constexpr auto v1_chars_table = webpp::charset_v1::charmap<256U>{':', '/', '?', '['};

void WebppSimpleCharmapSearch(benchmark::State& state) {
    std::size_t i = 0;
    for (auto _ : state) {
        auto str = strs[i++ % strs.size()];
        auto res = v1_chars_table.find_first_of(str.begin(), str.end());
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(str);
    }
}

BENCHMARK(WebppSimpleCharmapSearch);



// bitmap and charmap are the same if bitset doesn't support constexpr stuff
static auto v1_bit_table = webpp::charset_v1::bitmap<256U>{':', '/', '?', '['};

void WebppSimpleBitmapSearch(benchmark::State& state) {
    std::size_t i = 0;
    for (auto _ : state) {
        auto str = strs[i++ % strs.size()];
        auto res = v1_bit_table.find_first_of(str.begin(), str.end());
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(str);
    }
}

BENCHMARK(WebppSimpleBitmapSearch);
