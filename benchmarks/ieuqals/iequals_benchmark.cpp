#include "../benchmark.hpp"
#include <strings.h>

// clang-format off
#include webpp_include(strings/to_case)
#include webpp_include(strings/iequals)
// clang-format on

#if __has_include(<boost/algorithm/string/predicate.hpp>)
#  include <boost/algorithm/string/predicate.hpp>
#endif

#if __has_include(<boost/beast/core/string.hpp>)
#  include <boost/beast/core/string.hpp>
#  define BEAST_IEQUALS
#endif

using namespace webpp;





[[nodiscard]] constexpr bool iequal_tolower_all_the_way(istl::StringViewfiable auto&& _str1,
                                                        istl::StringViewfiable auto&& _str2) noexcept {
    using str1_type  = decltype(_str1);
    using str2_type  = decltype(_str2);
    using str1_t     = stl::remove_cvref_t<str1_type>;
    using str2_t     = stl::remove_cvref_t<str2_type>;
    using char_type  = istl::char_type_of<str1_t>;
    using char_type2 = istl::char_type_of<str2_t>;
    static_assert(
      stl::is_same_v<char_type, char_type2>,
      "The specified strings do not have the same character type, we're not able to compare them with this algorithm.");

    auto _size = size(_str1);
    if (_size != size(_str2))
        return false;

    auto str1 = istl::string_viewify(_str1);
    auto str2 = istl::string_viewify(_str2);


    if constexpr (istl::String<str1_t> && istl::String<str1_t> && stl::is_rvalue_reference_v<str1_type> &&
                  stl::is_rvalue_reference_v<str2_type>) {
        ascii::to_lower(_str1);
        ascii::to_lower(_str2);
        return _str1 == _str2;
    } else if constexpr (istl::String<str1_t> && stl::is_rvalue_reference_v<str1_type>) {
        ascii::to_lower(_str1);
        return _str1 == ascii::to_lower_copy(_str2, _str1.get_allocator());
    } else if constexpr (istl::String<str2_t> && stl::is_rvalue_reference_v<str2_type>) {
        ascii::to_lower(_str2);
        return ascii::to_lower_copy(_str1, _str2.get_allocator()) == _str2;
    } else {
        return stl::equal(str1.cbegin(), str1.cend(), str2.cbegin(), [](auto&& c1, auto&& c2) {
            return c1 == c2 || ascii::to_lower_copy(c1) == ascii::to_lower_copy(c2);
        });
    }
}





[[nodiscard]] constexpr bool simple_for_loop(istl::StringViewfiable auto&& _str1,
                                             istl::StringViewfiable auto&& _str2) noexcept {
    using str1_type  = decltype(_str1);
    using str2_type  = decltype(_str2);
    using str1_t     = stl::remove_cvref_t<str1_type>;
    using str2_t     = stl::remove_cvref_t<str2_type>;
    using char_type  = istl::char_type_of<str1_t>;
    using char_type2 = istl::char_type_of<str2_t>;
    static_assert(
      stl::is_same_v<char_type, char_type2>,
      "The specified strings do not have the same character type, we're not able to compare them with this algorithm.");

    auto _size = size(_str1);
    if (_size != size(_str2))
        return false;

    auto*       it1     = istl::string_data(_str1);
    auto*       it2     = istl::string_data(_str2);
    const auto* it1_end = it1 + _size;
    for (; it1 != it1_end; ++it1, ++it2) {
        if (*it1 != *it2) {
            auto ch1_lowered = ascii::to_lower_copy(*it1);
            auto ch2_lowered = ascii::to_lower_copy(*it2);
            if (ch1_lowered != ch2_lowered)
                return false;
        }
    }
    return true;
}



[[nodiscard]] constexpr bool simpler_for_loop(istl::StringViewfiable auto&& _str1,
                                              istl::StringViewfiable auto&& _str2) noexcept {
    using str1_type  = decltype(_str1);
    using str2_type  = decltype(_str2);
    using str1_t     = stl::remove_cvref_t<str1_type>;
    using str2_t     = stl::remove_cvref_t<str2_type>;
    using char_type  = istl::char_type_of<str1_t>;
    using char_type2 = istl::char_type_of<str2_t>;
    static_assert(
      stl::is_same_v<char_type, char_type2>,
      "The specified strings do not have the same character type, we're not able to compare them with this algorithm.");

    auto _size = size(_str1);
    if (_size != size(_str2))
        return false;

    auto*       it1     = istl::string_data(_str1);
    auto*       it2     = istl::string_data(_str2);
    const auto* it1_end = it1 + _size;
    for (; it1 != it1_end; ++it1, ++it2) {
        if (*it1 != *it2) {
            auto ch1_lowered = ascii::to_lower_copy(*it1);
            auto ch2_lowered = ascii::to_lower_copy(*it2);
            if (ch1_lowered != ch2_lowered)
                return false;
        }
    }
    return true;
}


/**
 * Check if two strings are equal case-insensitively
 */
[[nodiscard]] constexpr bool iequals_simd(istl::StringViewfiable auto&& _str1,
                                          istl::StringViewfiable auto&& _str2) noexcept {
    using str1_type  = decltype(_str1);
    using str2_type  = decltype(_str2);
    using str1_t     = stl::remove_cvref_t<str1_type>;
    using str2_t     = stl::remove_cvref_t<str2_type>;
    using char_type  = istl::char_type_of<str1_t>;
    using char_type2 = istl::char_type_of<str2_t>;
    static_assert(
      stl::is_same_v<char_type, char_type2>,
      "The specified strings do not have the same character type, we're not able to compare them with this algorithm.");

    auto _size = size(_str1);
    if (_size != size(_str2))
        return false;

    auto*       it1     = istl::string_data(_str1);
    auto*       it2     = istl::string_data(_str2);
    const auto* it1_end = it1 + _size;

#ifdef WEBPP_EVE
    using simd_type  = webpp::eve::wide<char_type>;
    using simd_utype = webpp::eve::wide<stl::make_unsigned_t<char_type>>;

    constexpr auto simd_size = simd_type::size();
    if (_size > simd_size) {
        const auto*      almost_end = it1_end - (_size % simd_size);
        const simd_utype big_a{'A'};
        const simd_utype diff{'a' - 'A'};
        for (; it1 != almost_end; it1 += simd_size, it2 += simd_size) {
            const auto values1  = webpp::eve::bit_cast(simd_type{it1}, webpp::eve::as_<simd_utype>());
            const auto values2  = webpp::eve::bit_cast(simd_type{it2}, webpp::eve::as_<simd_utype>());
            const auto equality = webpp::eve::is_not_equal(values1, values2);
            if (webpp::eve::any(equality)) {
                const auto val1_lowered = webpp::eve::logical_not(
                  webpp::eve::if_else(webpp::eve::is_less(webpp::eve::sub(values1, big_a), 25),
                                      webpp::eve::add(values1, diff), values1));
                const auto val2_lowered = webpp::eve::logical_not(
                  webpp::eve::if_else(webpp::eve::is_less(webpp::eve::sub(values1, big_a), 25),
                                      webpp::eve::add(values1, diff), values1));
                const auto equality2 = webpp::eve::is_not_equal(val1_lowered, val2_lowered);
                if (webpp::eve::any(equality2)) {
                    return false;
                }
            }
        }
        // do the rest
        it1 -= simd_size;
        it2 -= simd_size;
    }
#endif
    for (; it1 != it1_end; ++it1, ++it2) {
        if (*it1 != *it2) {
            // compiler seems to be able to optimize this better than us
            auto ch1_lowered = ascii::to_lower_copy(*it1);
            auto ch2_lowered = ascii::to_lower_copy(*it2);
            if (ch1_lowered != ch2_lowered)
                return false;
        }
    }
    return true;
}


static void IEQ_Strcasecmp(benchmark::State& state) {
    for (auto _ : state) {
        std::string str   = str_generator();
        auto        istr  = str;
        auto        istr2 = webpp::ascii::to_lower_copy(istr);
        auto res = ::strcasecmp(istr.data(), istr2.data());
        benchmark::DoNotOptimize(res == 0);
        benchmark::DoNotOptimize(istr);
        benchmark::DoNotOptimize(istr2);
    }
}
BENCHMARK(IEQ_Strcasecmp);

static void IEQ_Strncasecmp(benchmark::State& state) {
    for (auto _ : state) {
        std::string str   = str_generator();
        auto        istr  = str;
        auto        istr2 = webpp::ascii::to_lower_copy(istr);
        auto res = ::strncasecmp(istr.data(), istr2.data(), istr.size());
        benchmark::DoNotOptimize(res == 0);
        benchmark::DoNotOptimize(istr);
        benchmark::DoNotOptimize(istr2);
    }
}
BENCHMARK(IEQ_Strncasecmp);

static void IEQ_Default(benchmark::State& state) {
    for (auto _ : state) {
        std::string str   = str_generator();
        auto        istr  = str;
        auto        istr2 = webpp::ascii::to_lower_copy(istr);
        auto res = webpp::ascii::iequals(istr, istr2);
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(istr);
        benchmark::DoNotOptimize(istr2);
    }
}
BENCHMARK(IEQ_Default);

static void IEQ_DefaultLowered(benchmark::State& state) {
    for (auto _ : state) {
        std::string str   = str_generator();
        auto        istr  = str;
        auto        istr2 = webpp::ascii::to_lower_copy(istr);
        auto res = webpp::ascii::iequals<webpp::ascii::char_case_side::second_lowered>(istr, istr2);
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(istr);
        benchmark::DoNotOptimize(istr2);
    }
}
BENCHMARK(IEQ_DefaultLowered);

static void IEQ_SIMD(benchmark::State& state) {
    for (auto _ : state) {
        std::string str   = str_generator();
        auto        istr  = str;
        auto        istr2 = webpp::ascii::to_lower_copy(istr);
        auto res = iequals_simd(istr, istr2);
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(istr);
        benchmark::DoNotOptimize(istr2);
    }
}
BENCHMARK(IEQ_SIMD);

static void IEQ_ToLowerAllTheWay(benchmark::State& state) {
    for (auto _ : state) {
        std::string str   = str_generator();
        auto        istr  = str;
        auto        istr2 = webpp::ascii::to_lower_copy(istr);
        benchmark::DoNotOptimize(iequal_tolower_all_the_way(istr, istr2));
        benchmark::DoNotOptimize(istr);
        benchmark::DoNotOptimize(istr2);
    }
}
BENCHMARK(IEQ_ToLowerAllTheWay);



static void IEQ_SimpleForLoop(benchmark::State& state) {
    for (auto _ : state) {
        std::string str   = str_generator();
        auto        istr  = str;
        auto        istr2 = webpp::ascii::to_lower_copy(istr);
        benchmark::DoNotOptimize(simple_for_loop(istr, istr2));
        benchmark::DoNotOptimize(istr);
        benchmark::DoNotOptimize(istr2);
    }
}
BENCHMARK(IEQ_SimpleForLoop);


static void IEQ_SimplerForLoop(benchmark::State& state) {
    for (auto _ : state) {
        std::string str   = str_generator();
        auto        istr  = str;
        auto        istr2 = webpp::ascii::to_lower_copy(istr);
        benchmark::DoNotOptimize(simpler_for_loop(istr, istr2));
        benchmark::DoNotOptimize(istr);
        benchmark::DoNotOptimize(istr2);
    }
}
BENCHMARK(IEQ_SimplerForLoop);



#ifdef BOOST_STRING_COMPARE_HPP
static void IEQ_Boost(benchmark::State& state) {
    for (auto _ : state) {
        std::string str   = str_generator();
        auto        istr  = str;
        auto        istr2 = webpp::ascii::to_lower_copy(istr);
        benchmark::DoNotOptimize(boost::iequals(istr, istr2));
        benchmark::DoNotOptimize(istr);
        benchmark::DoNotOptimize(istr2);
    }
}
BENCHMARK(IEQ_Boost);
#endif

#ifdef BEAST_IEQUALS
static void IEQ_Boost_Beast(benchmark::State& state) {
    for (auto _ : state) {
        std::string str   = str_generator();
        auto        istr  = str;
        auto        istr2 = webpp::ascii::to_lower_copy(istr);
        benchmark::DoNotOptimize(boost::beast::iequals(istr, istr2));
        benchmark::DoNotOptimize(istr);
        benchmark::DoNotOptimize(istr2);
    }
}
BENCHMARK(IEQ_Boost_Beast);
#endif
