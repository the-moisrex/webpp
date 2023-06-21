#include "../../webpp/convert/lexical_cast.hpp"
#include "../benchmark.hpp"

#if __has_include(<boost/lexical_cast.hpp>)
#    include <boost/lexical_cast.hpp>
#    define has_boost
#endif


#if __has_include(<fmt/format.h>)
#    include <fmt/compile.h>
#    include <fmt/format.h>
#    define has_fmt
#endif

using namespace webpp;

const auto the_ints = int_generator();

static void LexicalCast_Default(benchmark::State& state) {
    const auto ints = the_ints;
    int        i    = 0;
    for (auto _ : state) {
        auto res = lexical::cast<std::string>(ints[i++ % ints.size()]);
        benchmark::DoNotOptimize(res);
    }
}
BENCHMARK(LexicalCast_Default);

#ifdef has_boost
static void LexicalCast_Boost(benchmark::State& state) {
    const auto ints = the_ints;
    int        i    = 0;
    for (auto _ : state) {
        auto res = boost::lexical_cast<std::string>(ints[i++ % ints.size()]);
        benchmark::DoNotOptimize(res);
    }
}
BENCHMARK(LexicalCast_Boost);
#endif

static void LexicalCast_ToString(benchmark::State& state) {
    const auto ints = the_ints;
    int        i    = 0;
    for (auto _ : state) {
        auto res = std::to_string(ints[i++ % ints.size()]);
        benchmark::DoNotOptimize(res);
    }
}
BENCHMARK(LexicalCast_ToString);

#ifdef has_fmt
static void LexicalCast_FMT(benchmark::State& state) {
    const auto ints = the_ints;
    int        i    = 0;
    for (auto _ : state) {
        auto res = ::fmt::format("{}", ints[i++ % ints.size()]);
        benchmark::DoNotOptimize(res);
    }
}
BENCHMARK(LexicalCast_FMT);

static void LexicalCast_FMT_Compile(benchmark::State& state) {
    const auto ints = the_ints;
    int        i    = 0;
    for (auto _ : state) {
        auto res = ::fmt::format("{}", ints[i++ % ints.size()]);
        benchmark::DoNotOptimize(res);
    }
}
BENCHMARK(LexicalCast_FMT_Compile);

static void LexicalCast_FMT_FormatTo(benchmark::State& state) {
    const auto ints = the_ints;
    int        i    = 0;
    for (auto _ : state) {
        std::string str;
        ::fmt::format_to(std::back_inserter(str), "{}", ints[i++ % ints.size()]);
        benchmark::DoNotOptimize(str);
    }
}
BENCHMARK(LexicalCast_FMT_FormatTo);


static void LexicalCast_FMT_vformat(benchmark::State& state) {
    const auto ints = the_ints;
    int        i    = 0;
    for (auto _ : state) {
        auto res = ::fmt::vformat("{}", ::fmt::make_format_args(ints[i++ % ints.size()]));
        benchmark::DoNotOptimize(res);
    }
}
BENCHMARK(LexicalCast_FMT_vformat);

#endif
