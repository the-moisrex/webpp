#include "../benchmark.hpp"

using namespace std;

string str =
  "long string 1 long string 1long string 1long string 1long string 1long "
  "string 1long string 1long string 1long string 1long string 1long string "
  "1long string 1long string 1long string 1long string 1long string 1long "
  "string 1long string 1long string 1long string 1long string 1long string "
  "1long string 1long string 1long string 1long string 1long string 1long "
  "string 1long string 1long string 1long string 1long string 1long string "
  "1long string 1long string 1long string 1long string 1long string 1long "
  "string 1long string 1long string 1long string 1long string 1long string "
  "1long string 1long string 1long string 1long string 1long string 1long "
  "string 1long string 1long string 1long string 1long string 1long string "
  "1long string 1long string 1long string 1long string 1long string 1long "
  "string 1 *";

static void FindStr(benchmark::State& state) {
    str += " oh oh"; // to make sure the string is dynamic and not constexpr
    for (auto _ : state) {
        auto const c = str.find("*");
        benchmark::DoNotOptimize(c);
    }
}

BENCHMARK(FindStr);

static void FindChar(benchmark::State& state) {
    for (auto _ : state) {
        auto const c = str.find('*');
        benchmark::DoNotOptimize(c);
    }
}

BENCHMARK(FindChar);

static void FindStrView(benchmark::State& state) {
    const string_view star{"*"};
    for (auto _ : state) {
        auto const c = str.find(star);
        benchmark::DoNotOptimize(c);
    }
}

BENCHMARK(FindStrView);

// BENCHMARK_MAIN();
