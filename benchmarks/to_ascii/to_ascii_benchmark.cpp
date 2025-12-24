#include "../../webpp/unicode/to_ascii.hpp"
#include "../benchmark.hpp"

#if __has_include("./ada_idna/singleheader/ada_idna.h")
#    include "./ada_idna/singleheader/ada_idna.h"
#endif

using namespace webpp;

auto const strs32 = str32_array_generator<1000>();
auto const strs8  = str8_array_generator<1000>();

static void ToASCII_UTF32_to_UTF32(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto str   = strs32.at(index++ % strs32.size());
        auto ascii = unicode::idna::to_ascii<std::u32string>(str);
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_UTF32_to_UTF32);

static void ToASCII_UTF8_to_UTF32(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto str   = strs8.at(index++ % strs8.size());
        auto ascii = unicode::idna::to_ascii<std::u32string>(str);
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_UTF8_to_UTF32);

static void ToASCII_UTF8_to_UTF8(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto str   = strs8.at(index++ % strs8.size());
        auto ascii = unicode::idna::to_ascii<std::u8string>(str);
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_UTF8_to_UTF8);

static void ToASCII_UTF32_to_UTF8(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto str   = strs32.at(index++ % strs32.size());
        auto ascii = unicode::idna::to_ascii<std::u8string>(str);
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_UTF32_to_UTF8);

#ifdef HAS_ADA_IDNA
// Ada-url/idna benchmarks
static void ToASCII_Ada_UTF8_to_UTF8(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto str   = strs8.at(index++ % strs8.size());
        auto ascii = ada::idna::to_ascii(std::string_view(reinterpret_cast<char const*>(str.data()), str.length()));
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_Ada_UTF8_to_UTF8);
#endif
