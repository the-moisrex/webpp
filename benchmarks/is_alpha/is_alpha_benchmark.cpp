#include "../benchmark.hpp"
#include <cstring>
#include <string>
#include <random>
#include <algorithm>
#include <type_traits>
#include <bitset>

#include <eve/function/any.hpp>
#include <eve/wide.hpp>

using namespace std;

bool
is_alpha_bools(char c)
{
    static bool constexpr tab[] = {
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, // 0
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, // 16
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, // 32
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, // 48
      0, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // 64
      1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 0,  0, 0, 0, 0, // 80
      0, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // 96
      1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 0,  0, 0, 0, 0, // 112
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, // 128
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, // 144
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, // 160
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, // 176
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, // 192
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, // 208
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, // 224
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0  // 240
    };
    return tab[c];
}

char
is_alpha(char c)
{
    static char constexpr tab[] = {
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, // 0
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, // 16
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, // 32
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, // 48
      0, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // 64
      1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 0,  0, 0, 0, 0, // 80
      0, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1, // 96
      1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 0,  0, 0, 0, 0, // 112
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, // 128
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, // 144
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, // 160
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, // 176
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, // 192
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, // 208
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, // 224
      0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0  // 240
    };
    return tab[static_cast<unsigned char>(c)];
}

bool
is_alpha_bitset(char c)
{
    static const std::bitset<255> tab {
      "0000000000000000000000000000000000000000000000000000000000000000"
      "0111111111111111111111111110000001111111111111111111111111100000"
      "0000000000000000000000000000000000000000000000000000000000000000"
      "0000000000000000000000000000000000000000000000000000000000000000"
    };
    return tab[static_cast<std::size_t>(c)];
}

bool is_alpha_arithmetic(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool is_alpha_arithmetic2(char c) {
    static constexpr uint64_t t = 0b1111111111111111111111111100000011111111111111111111111111000000;
    return t & (0b1 << (c - 'A'));
}


bool cmp(auto&& str1) noexcept {
  using char_type = typename std::remove_cvref_t<decltype(str1)>::value_type;
  using simd_type = eve::wide<char_type>;
  static constexpr auto simd_size = simd_type::size();

  const auto size1 = str1.size();

  auto it1 = str1.data();

  const auto almost_end = it1 + (size1 % simd_size);
  for (; it1 != almost_end; it1 += simd_size ) {
    auto const values1 = simd_type{it1};
  }

  // let's just forget about the rest of them now, it's just a benchmark and we already know the results.

  return true;
}


static void IsAlpha_lookup(benchmark::State& state) {
  for (auto _ : state) {
      auto str = str_generator();
      for (auto c : str) {
          benchmark::DoNotOptimize(is_alpha(c));
      }
  }
}
BENCHMARK(IsAlpha_lookup);

static void IsAlpha_lookup_bools(benchmark::State& state) {
    for (auto _ : state) {
        auto str = str_generator();
        for (auto c : str) {
            benchmark::DoNotOptimize(is_alpha_bools(c));
        }
    }
}
BENCHMARK(IsAlpha_lookup_bools);

static void IsAlpha_lookup_bitset(benchmark::State& state) {
    for (auto _ : state) {
        auto str = str_generator();
        for (auto c : str) {
            benchmark::DoNotOptimize(is_alpha_bitset(c));
        }
    }
}
BENCHMARK(IsAlpha_lookup_bitset);


static void IsAlpha_arithmetic(benchmark::State& state) {
    for (auto _ : state) {
        auto str = str_generator();
        for (auto c : str) {
            benchmark::DoNotOptimize(is_alpha_arithmetic(c));
        }
    }
}
BENCHMARK(IsAlpha_arithmetic);

static void IsAlpha_arithmetic2(benchmark::State& state) {
    for (auto _ : state) {
        auto str = str_generator();
        for (auto c : str) {
            benchmark::DoNotOptimize(is_alpha_arithmetic2(c));
        }
    }
}
BENCHMARK(IsAlpha_arithmetic2);

#define func(name, algo)\
    bool string_##algo(auto&& str) { \
        char* it = str.data(); \
        std::size_t const _size = str.size();\
        char const* _end = str.data() + _size; \
        for (; it != _end; ++it) {           \
            if (!is_##algo(*it)) \
                return false;\
        }               \
        return true;\
    }                        \
                        \
static void IsAlpha_##name(benchmark::State& state) {\
    for (auto _ : state) {\
        auto str = str_generator();\
        benchmark::DoNotOptimize(string_##algo(str));\
    }\
}\
BENCHMARK(IsAlpha_##name);\

func(string_lookup, alpha)
func(string_lookup_bools, alpha_bools)
func(string_bitset, alpha_bitset)
func(string_arithmetic, alpha_arithmetic)
func(string_arithmetic2, alpha_arithmetic2)
