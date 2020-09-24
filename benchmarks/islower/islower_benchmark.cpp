#include "../benchmark.hpp"

#include <algorithm>

using namespace std;

static void ISLower_STD(benchmark::State& state) {
  auto str = str_generator();
  auto is_lower = [] (auto&& str) {
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
    benchmark::DoNotOptimize(is_lower(str));
  }
}
BENCHMARK(ISLower_STD);


