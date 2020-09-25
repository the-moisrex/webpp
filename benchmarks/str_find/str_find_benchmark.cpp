#include "../benchmark.hpp"
#include <cstring>
#include <string>
#include <random>
#include <algorithm>
#include <type_traits>

#include <eve/function/any.hpp>
#include <eve/wide.hpp>
#include <eve/function/extract.hpp>

using namespace std;



bool find_char(auto&& str, auto&&ch) noexcept {
  using char_type = typename std::remove_cvref_t<decltype(str)>::value_type;
  using ch_type = std::remove_cvref_t<decltype(ch)>;
  static_assert(std::is_same_v<char_type, ch_type>, "The specified char type is not the same as the string's char type.");

  using simd_type = eve::wide<char_type>;
  static constexpr auto simd_size = simd_type::size();

  const auto _size = str.size();

  auto it = str.data();

  const auto it1end = it + _size;
  const auto almost_end = it + (_size % simd_size);
  for (; it != almost_end; it += simd_size) {
    auto const values1 = simd_type{it};
    auto const cmped = eve::is_not_equal(values1, ch);
    if (eve::any(cmped))
      return false;
  }

  for (it -= simd_size; it != it1end; ++it)
      if (*it != ch)
          return false;

  return true;
}

#include <string_view>
#include <eve/wide.hpp>
#include <eve/function/any.hpp>



std::size_t find_str(std::string_view str1, std::string_view str2) noexcept {
    using char_type = typename std::remove_cvref_t<decltype(str1)>::value_type;
    using simd_type = eve::wide<char_type>;
    using string_type = std::string_view;
    static constexpr auto simd_size = simd_type::size();

    const auto size1 = str1.size();
    const auto size2 = str2.size();
    if (size2 > size1)
        return string_type::npos;

    const auto it1start = str1.data();
    const auto it2start = str2.data();
    const auto it1end = it1start + size1;
    const auto it2end = it2start + size2;
    const auto last_possible_position = size1 - size2;
    const auto finishline_size (last_possible_position % simd_size);
    const auto almost_end = it1end - finishline_size;

    auto it1 = str1.data();
    auto it2 = str2.data();

    goto the_rest;
    if (size1 < simd_size || size2 < simd_size)
        goto the_rest;


    for (; it1 != almost_end; it1 += simd_size) {
        // let's find the first char (needle) in the haystack
        auto const values1 = simd_type{it1};
        auto const values2 = simd_type{it2[0]};
        auto const cmped = eve::is_equal(values1, values2);

        // todo: is this part optimize-able?
        if (eve::any(cmped)) {
            const auto items = cmped.bits();
            const auto item_end = items.end();
            const auto item_start = items.begin();
            for (auto item_it = item_start; item_it != item_end; ++item_it) {
                if (!*item_it)
                    continue;

                const auto len = item_it - item_start;
                const auto new_finishline_size = finishline_size - len;
                if (new_finishline_size <= 0) {
                    it1 += len;
                    goto the_rest;
                }
                const auto new_almost_end = it2end - new_finishline_size;
                auto it1_rest = it1 + len + 1; // we found the first one, not it's time for the rest
                auto it2_rest = it2 + 1; // we checked the first char above, now it's time for the rest

                for (; it1_rest != new_almost_end; it1_rest += simd_size, it2_rest += simd_size) {
                    const auto next_values1_set = simd_type{it1_rest};
                    const auto next_values2_set = simd_type{it2_rest};
                    const auto res = eve::is_not_equal(next_values1_set, next_values2_set);
                    if (eve::any(res)) {
                        goto cnt; // continue with the outer, outer loop
                    }
                }

                it1 = it1_rest;
                it2 = it2_rest;
                goto the_rest; // check the rest of the strings

            }
        }

        cnt:; // for when the inner loop wants to break out of it's inner loop and continue with the outer loop
    }

    it1 -= simd_size;

    the_rest:
    const auto last_possible_end = it1start - last_possible_position;
    decltype(it1) found = nullptr;
    for (; it1 != last_possible_end; ++it1) {
        if (*it1 == *it2) {
            if (it2 == it2start) {
                found = it1;
            } else if (it2 == it2end - 1) {
                return found - it1start;
            }
            ++it2;
        } else {
            found = nullptr;
        }
    }


    return string_type::npos;
}


std::size_t find_str_simple(std::string_view str1, std::string_view str2) noexcept {
    using char_type = typename std::remove_cvref_t<decltype(str1)>::value_type;
    using simd_type = eve::wide<char_type>;
    using string_type = std::string_view;
    static constexpr auto simd_size = simd_type::size();

    const auto size1 = str1.size();
    const auto size2 = str2.size();
    if (size2 > size1)
        return string_type::npos;

    const auto it1start = str1.data();
    const auto it2start = str2.data();
    const auto it1end = it1start + size1;
    const auto it2end = it2start + size2;
    const auto last_possible_position = size1 - size2;

    auto it1 = str1.data();
    auto it2 = str2.data();

    const auto last_possible_end = it1start - last_possible_position;
    decltype(it1) found = nullptr;
    for (; it1 != last_possible_end; ++it1) {
        if (*it1 == *it2) {
            if (it2 == it2start) {
                found = it1;
            } else if (it2 == it2end - 1) {
                return found - it1start;
            }
            ++it2;
        } else {
            found = nullptr;
        }
    }


    return string_type::npos;
}


static void StrFind_FindCharString(benchmark::State& state) {
  for (auto _ : state) {
    auto str1 = str_generator();
    benchmark::DoNotOptimize(str1.find('a'));
  }
}
BENCHMARK(StrFind_FindCharString);

static void StrFind_FindCharSIMD(benchmark::State& state) {
  for (auto _ : state) {
    auto str1 = str_generator();
    benchmark::DoNotOptimize(find_char(str1, 'a'));
  }
}
BENCHMARK(StrFind_FindCharSIMD);

//////////////////// String /////////////////////////////

static void StrFind_FindStringString(benchmark::State& state) {
    for (auto _ : state) {
        auto str1 = str_generator();
        benchmark::DoNotOptimize(str1.find("abc"));
    }
}
BENCHMARK(StrFind_FindCharString);

static void StrFind_FindStringSimple(benchmark::State& state) {
    for (auto _ : state) {
        auto str1 = str_generator();
        benchmark::DoNotOptimize(find_str_simple(str1, "abc"));
    }
}
BENCHMARK(StrFind_FindCharSIMD);

static void StrFind_FindStringSIMD(benchmark::State& state) {
    for (auto _ : state) {
        auto str1 = str_generator();
        benchmark::DoNotOptimize(find_str(str1, "abc"));
    }
}
BENCHMARK(StrFind_FindCharSIMD);


