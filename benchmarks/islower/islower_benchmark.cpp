#include "../benchmark.hpp"
// clang-format off
#include webpp_include(strings/istring)
// clang-format on

#include <algorithm>

using namespace std;
using namespace webpp;

static void ISLower_Simple(benchmark::State& state) {
  auto is_lower = [] (auto&& str) constexpr noexcept {
    auto it = str.data();
    const auto _end = it + str.size();
    for (; it != _end; ++it) {
      if (!std::islower(*it)) {
        return false;
      }
    }
    return true;
  };
  for (auto _ : state) {
    auto str = str_generator();
    benchmark::DoNotOptimize(is_lower(str));
  }
}
BENCHMARK(ISLower_Simple);

static void ISLower_IString(benchmark::State& state) {
    for (auto _ : state) {
        auto str = str_generator();
        auto istr = istring<decltype(str)>{str};
        benchmark::DoNotOptimize(istr.is_ascii_lower());
    }
}
BENCHMARK(ISLower_IString);


