#include "../benchmark.hpp"

#include <array>

using namespace std;

static constexpr char const token_char_map[]{
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\1\0\1\1\1\1\1\0\0\1\1\0\1\1\0\1\1\1\1\1\1\1\1\1\1\0\0\0\0\0\0"
  "\0\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\0\0\0\1\1"
  "\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\0\1\0\1\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"};

static constexpr bool is_token(char item) {
    return static_cast<bool>(token_char_map[static_cast<unsigned char>(item)]);
}

static constexpr array<bool, 256> bool_token_char_map{
  {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
   false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
   false, false, false, true, false, true, true, true, true, true, false, false, true, true, false,
   true, true, false, true, true, true, true, true, true, true, true, true, true, false, false,
   false, false, false, false, false, true, true, true, true, true, true, true, true, true, true,
   true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
   true, false, false, false, true, true, true, true, true, true, true, true, true, true, true,
   true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
   true, true, true, false, true, false, true, false, false, false, false, false, false, false, false,
   false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
   false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
   false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
   false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
   false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
   false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
   false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
   false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
   false}
};

static constexpr bool bool_is_token(char item) {
    return token_char_map[static_cast<unsigned char>(item)];
}

static void BoolTokenMap(benchmark::State& state) {
    auto str = str_generator();
    for (auto _ : state) {
        for (auto ch : str) {
            benchmark::DoNotOptimize(bool_is_token(ch));
        }
    }
}

BENCHMARK(BoolTokenMap);

static void CharTokenMap(benchmark::State& state) {
    auto str = str_generator();
    for (auto _ : state) {
        for (auto ch : str) {
            benchmark::DoNotOptimize(is_token(ch));
        }
    }
}

BENCHMARK(CharTokenMap);
