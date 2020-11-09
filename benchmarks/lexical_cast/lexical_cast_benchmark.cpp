#include "../benchmark.hpp"
#include "../../core/include/webpp/convert/lexical_cast.hpp"

#if __has_include(<boost/lexical_cast.hpp>)
#include <boost/lexical_cast.hpp>
#define has_boost
#endif

using namespace webpp;

const auto the_ints = int_generator();

static void LexicalCast_Default(benchmark::State& state) {
  const auto ints = the_ints;
  int i = 0;
    for (auto _ : state) {
      benchmark::DoNotOptimize(lexical::cast<std::string>(ints[i++ % ints.size()]));
    }
}
BENCHMARK(LexicalCast_Default);

#ifdef has_boost
static void LexicalCast_Boost(benchmark::State& state) {
  const auto ints = the_ints;
  int i = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(boost::lexical_cast<std::string>(ints[i++ % ints.size()]));
  }
}
BENCHMARK(LexicalCast_Boost);
#endif

static void LexicalCast_ToString(benchmark::State& state) {
  const auto ints = the_ints;
  int i = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(std::to_string(ints[i++ % ints.size()]));
  }
}
BENCHMARK(LexicalCast_ToString);

