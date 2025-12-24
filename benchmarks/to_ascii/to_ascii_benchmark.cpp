#include "../../webpp/unicode/to_ascii.hpp"
#include "../benchmark.hpp"

using namespace webpp;

auto const strs32 = str32_array_generator<1000>();
auto const strs8 = str8_array_generator<1000>();

static void ToASCII_UTF32_to_UTF32(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto str = strs32.at(index++ % strs32.size());
        auto ascii = unicode::idna::to_ascii<std::u32string>(str);
        benchmark::DoNotOptimize(ascii);
    }
}
BENCHMARK(ToASCII_UTF32_to_UTF32);

static void ToASCII_UTF8_to_UTF32(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto str = strs8.at(index++ % strs8.size());
        auto ascii = unicode::idna::to_ascii<std::u32string>(str);
        benchmark::DoNotOptimize(ascii);
    }
}
BENCHMARK(ToASCII_UTF8_to_UTF32);

static void ToASCII_UTF8_to_UTF8(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto str = strs8.at(index++ % strs8.size());
        auto ascii = unicode::idna::to_ascii<std::u8string>(str);
        benchmark::DoNotOptimize(ascii);
    }
}
BENCHMARK(ToASCII_UTF8_to_UTF8);

static void ToASCII_UTF32_to_UTF8(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto str = strs32.at(index++ % strs32.size());
        auto ascii = unicode::idna::to_ascii<std::u8string>(str);
        benchmark::DoNotOptimize(ascii);
    }
}
BENCHMARK(ToASCII_UTF32_to_UTF8);

