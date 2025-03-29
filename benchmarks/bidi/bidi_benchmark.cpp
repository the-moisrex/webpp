#include "../../tests/common/bidi.hpp"
#include "../../webpp/unicode/bidi.hpp"
#include "../benchmark.hpp"

static void BidiBinarySearch(benchmark::State& state) {
    for (auto _ : state) {
        for (char32_t cp = 0; cp < 0x10'fffdU + 10; cp += 1) {
            auto rhs = webpp::unicode::tests::find_direction(cp);
            benchmark::DoNotOptimize(rhs);
        }
    }
}

BENCHMARK(BidiBinarySearch);

static void BidiTableLookup(benchmark::State& state) {
    for (auto _ : state) {
        for (char32_t cp = 0; cp < 0x10'fffdU + 10; cp += 1) {
            auto lhs = webpp::unicode::direction_of(cp);
            benchmark::DoNotOptimize(lhs);
        }
    }
}

BENCHMARK(BidiTableLookup);
