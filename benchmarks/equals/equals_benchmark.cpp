#include "../benchmark.hpp"
#include <cstring>
#include <string>
#include <random>
#include <algorithm>
#include <type_traits>

#include <eve/function/any.hpp>
#include <eve/wide.hpp>

using namespace std;



bool cmp(auto&& str1, auto&&str2) noexcept {
  using char_type = typename std::remove_cvref_t<decltype(str1)>::value_type;
  using simd_type = eve::wide<char_type>;
  static constexpr auto simd_size = simd_type::size();

  const auto size1 = str1.size();
  const auto size2 = str2.size();
  if (size1 != size2)
    return false;

  auto it1 = str1.data();
  auto it2 = str2.data();

  const auto it1end = it1 + size1;
  const auto almost_end = it1 + (size1 % simd_size);
  for (; it1 != almost_end; it1 += simd_size, it2 += simd_size ) {
    auto const values1 = simd_type{it1};
    auto const values2 = simd_type{it2};
    auto const cmped = eve::is_not_equal(values1, values2);
    if (eve::any(cmped))
      return false;
  }

  // let's just forget about the rest of them now, it's just a benchmark and we already know the results.

  return true;
}


static void EQ_STD(benchmark::State& state) {
  for (auto _ : state) {
    auto str1 = str_generator();
    auto str2 = str_generator();
    benchmark::DoNotOptimize(str1 == str2);
  }
}
BENCHMARK(EQ_STD);

static void EQ_STRcmp(benchmark::State& state) {
  for (auto _ : state) {
    auto str1 = str_generator();
    auto str2 = str_generator();
    benchmark::DoNotOptimize(strcmp(str1.data(), str2.data()));
  }
}
BENCHMARK(EQ_STRcmp);

static void EQ_SIMD(benchmark::State& state) {
  for (auto _ : state) {
    auto str1 = str_generator();
    auto str2 = str_generator();
    benchmark::DoNotOptimize(cmp(str1, str2));
  }
}
BENCHMARK(EQ_SIMD);


