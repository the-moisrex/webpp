#include "../benchmark.hpp"

#include <algorithm>
#include <bitset>
#include <cstring>
#include <eve/function/any.hpp>
#include <eve/wide.hpp>
#include <random>
#include <string>
#include <type_traits>

using namespace std;

bool is_alpha_bools(char c) {
    static bool constexpr tab[] = {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 16
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 32
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 48
      0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 64
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, // 80
      0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 96
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, // 112
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 128
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 144
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 160
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 176
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 192
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 208
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 224
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // 240
    };
    return tab[c];
}

char is_alpha(char c) {
    static char constexpr tab[] = {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 16
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 32
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 48
      0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 64
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, // 80
      0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 96
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, // 112
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 128
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 144
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 160
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 176
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 192
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 208
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 224
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // 240
    };
    return tab[static_cast<unsigned char>(c)];
}

bool is_alpha_bitset(char c) {
    static const std::bitset<255> tab{"0000000000000000000000000000000000000000000000000000000000000000"
                                      "0111111111111111111111111110000001111111111111111111111111100000"
                                      "0000000000000000000000000000000000000000000000000000000000000000"
                                      "0000000000000000000000000000000000000000000000000000000000000000"};
    return tab[static_cast<std::size_t>(c)];
}

bool is_alpha_arithmetic(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool is_alpha_arithmetic3(char c) {
    return (unsigned(c & ~(1 << 5)) - 'A') <= ('Z' - 'A');
}


bool is_alpha_arithmetic2(char c) {
    static constexpr uint64_t t     = 0b1111111111111111111111111100000011111111111111111111111111000000;
    auto const                count = c - 'A';
    return ((t & (uint64_t(1) << count)) >> count) == 0;
}

bool is_alpha_simd(auto&& str) noexcept {
    using char_type                 = typename std::remove_cvref_t<decltype(str)>::value_type;
    using simd_type                 = eve::wide<char_type>;
    using simd_utype                = eve::wide<std::make_unsigned_t<char_type>>;
    static constexpr auto simd_size = simd_type::size();
    using simd64_t                  = eve::wide<uint64_t, eve::fixed<simd_size>>;

    const auto _size = str.size();
    auto it = str.data();
    const auto* it_end = it + _size;

    const auto              almost_end = it + (_size % simd_size);
    static const simd64_t t{0b1111111111111111111111111100000011111111111111111111111111000000};
    static const simd64_t ones{1};
    static const simd_utype big_a{'A'};
    for (; it != almost_end; it += simd_size) {
        const auto values = eve::bit_cast(simd_type{it}, eve::as_<simd_utype>());
        const auto counts = eve::sub(values, big_a);
        const auto shifted = eve::shl(ones, counts);
        const auto anded = eve::bit_and(t, shifted);
        const auto shifted_again = eve::shr(anded, count);
        const auto res = eve::is_equal(shifted_again, 0);
        if (eve::any(res)) {
            return false;
        }
    }

    it -= simd_size;
    for (; it != it_end; ++it) {
        if (!is_alpha_arithmetic2(*it)) {
            return false;
        }
    }
    return true;
}


const auto is_alpha_strs =
  str_array_generator<256>(10000, std::string_view{"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"});

#define func(name, algo)                                                      \
    static void IsAlpha_##name(benchmark::State& state) {                     \
        auto          strs = is_alpha_strs;                                   \
        unsigned char i    = 0;                                               \
        for (auto _ : state) {                                                \
            benchmark::DoNotOptimize(strs);                                   \
            benchmark::DoNotOptimize(i);                                      \
            benchmark::DoNotOptimize(strs[i % strs.size()]);                  \
            auto              str   = strs[i++ % strs.size()];                \
            auto*             it    = str.data();                             \
            std::size_t const _size = str.size();                             \
            char const*       _end  = str.data() + _size;                     \
            for (; it != _end; ++it) {                                        \
                benchmark::DoNotOptimize(is_##algo(*it));                     \
            }                                                                 \
        }                                                                     \
    }                                                                         \
    BENCHMARK(IsAlpha_##name);                                                \
                                                                              \
    bool string_##algo(auto&& str) {                                          \
        auto*             it    = str.data();                                 \
        std::size_t const _size = str.size();                                 \
        char const*       _end  = str.data() + _size;                         \
        for (; it != _end; ++it) {                                            \
            if (!is_##algo(*it))                                              \
                return false;                                                 \
        }                                                                     \
        return true;                                                          \
    }                                                                         \
                                                                              \
    static void IsAlpha_string_##name(benchmark::State& state) {              \
        auto          strs = is_alpha_strs;                                   \
        unsigned char i    = 0;                                               \
        for (auto _ : state) {                                                \
            benchmark::DoNotOptimize(strs);                                   \
            benchmark::DoNotOptimize(i);                                      \
            benchmark::DoNotOptimize(strs[i % strs.size()]);                  \
            benchmark::DoNotOptimize(string_##algo(strs[i++ % strs.size()])); \
        }                                                                     \
    }                                                                         \
    BENCHMARK(IsAlpha_string_##name);

func(lookup, alpha)

  func(lookup_bools, alpha_bools)

    func(bitset, alpha_bitset)

      func(arithmetic, alpha_arithmetic)

        func(arithmetic2, alpha_arithmetic2)

          func(arithmetic3, alpha_arithmetic3)





static void IsAlpha_string_SIMD(benchmark::State& state) {
    auto          strs = is_alpha_strs;
    unsigned char i    = 0;
    for (auto _ : state) {
        benchmark::DoNotOptimize(strs);
        benchmark::DoNotOptimize(i);
        benchmark::DoNotOptimize(strs[i % strs.size()]);
        benchmark::DoNotOptimize(is_alpha_simd(strs[i++ % strs.size()]));
    }
}
BENCHMARK(IsAlpha_string_SIMD);

