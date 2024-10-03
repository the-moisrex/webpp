#include "../../webpp/std/string_view.hpp"
#include "../../webpp/strings/iequals.hpp"
#include "../../webpp/strings/to_case.hpp"
#include "../benchmark.hpp"
#include "./iiequals_impl.hpp"

#include <array>
#include <string>
#include <strings.h>


#if __has_include(<boost/algorithm/string/predicate.hpp>)
#    include <boost/algorithm/string/predicate.hpp>
#endif

#if __has_include(<boost/beast/core/string.hpp>)
#    include <boost/beast/core/string.hpp>
#    define BEAST_IEQUALS
#endif

using namespace webpp;

static auto strs = str_array_generator<1000>();

[[nodiscard]] constexpr bool iequal_tolower_all_the_way(istl::StringViewifiable auto&& _str1,
                                                        istl::StringViewifiable auto&& _str2) noexcept {
    using str1_type  = decltype(_str1);
    using str2_type  = decltype(_str2);
    using str1_t     = stl::remove_cvref_t<str1_type>;
    using str2_t     = stl::remove_cvref_t<str2_type>;
    using char_type  = istl::char_type_of_t<str1_t>;
    using char_type2 = istl::char_type_of_t<str2_t>;
    static_assert(stl::is_same_v<char_type, char_type2>,
                  "The specified strings do not have the same character type, we're not able to compare them "
                  "with this algorithm.");

    auto _size = size(_str1);
    if (_size != size(_str2)) {
        return false;
    }

    auto str1 = istl::string_viewify(_str1);
    auto str2 = istl::string_viewify(_str2);


    if constexpr (istl::String<str1_t> && istl::String<str1_t> && stl::is_rvalue_reference_v<str1_type> &&
                  stl::is_rvalue_reference_v<str2_type>)
    {
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

[[nodiscard]] constexpr bool simple_for_loop(istl::StringViewifiable auto&& _str1,
                                             istl::StringViewifiable auto&& _str2) noexcept {
    using str1_type  = decltype(_str1);
    using str2_type  = decltype(_str2);
    using str1_t     = stl::remove_cvref_t<str1_type>;
    using str2_t     = stl::remove_cvref_t<str2_type>;
    using char_type  = istl::char_type_of_t<str1_t>;
    using char_type2 = istl::char_type_of_t<str2_t>;
    static_assert(stl::is_same_v<char_type, char_type2>,
                  "The specified strings do not have the same character type, we're not able to compare them "
                  "with this algorithm.");

    auto _size = size(_str1);
    if (_size != size(_str2)) {
        return false;
    }

    auto*       it1     = istl::string_data(_str1);
    auto*       it2     = istl::string_data(_str2);
    auto const* it1_end = it1 + _size;
    for (; it1 != it1_end; ++it1, ++it2) {
        if (*it1 != *it2) {
            auto ch1_lowered = ascii::to_lower_copy(*it1);
            auto ch2_lowered = ascii::to_lower_copy(*it2);
            if (ch1_lowered != ch2_lowered) {
                return false;
            }
        }
    }
    return true;
}

[[nodiscard]] constexpr bool simpler_for_loop(istl::StringViewifiable auto&& _str1,
                                              istl::StringViewifiable auto&& _str2) noexcept {
    using str1_type  = decltype(_str1);
    using str2_type  = decltype(_str2);
    using str1_t     = stl::remove_cvref_t<str1_type>;
    using str2_t     = stl::remove_cvref_t<str2_type>;
    using char_type  = istl::char_type_of_t<str1_t>;
    using char_type2 = istl::char_type_of_t<str2_t>;
    static_assert(stl::is_same_v<char_type, char_type2>,
                  "The specified strings do not have the same character type, we're not able to compare them "
                  "with this algorithm.");

    auto _size = size(_str1);
    if (_size != size(_str2)) {
        return false;
    }

    auto*       it1     = istl::string_data(_str1);
    auto*       it2     = istl::string_data(_str2);
    auto const* it1_end = it1 + _size;
    for (; it1 != it1_end; ++it1, ++it2) {
        if (*it1 != *it2) {
            auto ch1_lowered = ascii::to_lower_copy(*it1);
            auto ch2_lowered = ascii::to_lower_copy(*it2);
            if (ch1_lowered != ch2_lowered) {
                return false;
            }
        }
    }
    return true;
}

/**
 * Check if two strings are equal case-insensitively
 */
[[nodiscard]] constexpr bool iequals_simd(istl::StringViewifiable auto&& _str1,
                                          istl::StringViewifiable auto&& _str2) noexcept {
    using str1_type  = decltype(_str1);
    using str2_type  = decltype(_str2);
    using str1_t     = stl::remove_cvref_t<str1_type>;
    using str2_t     = stl::remove_cvref_t<str2_type>;
    using char_type  = istl::char_type_of_t<str1_t>;
    using char_type2 = istl::char_type_of_t<str2_t>;
    static_assert(stl::is_same_v<char_type, char_type2>,
                  "The specified strings do not have the same character type, we're not able to compare them "
                  "with this algorithm.");

    auto _size = size(_str1);
    if (_size != size(_str2)) {
        return false;
    }

    auto*       it1     = istl::string_data(_str1);
    auto*       it2     = istl::string_data(_str2);
    auto const* it1_end = it1 + _size;

#ifdef WEBPP_EVE_OLD
    using simd_type  = webpp::eve::wide<char_type>;
    using simd_utype = webpp::eve::wide<stl::make_unsigned_t<char_type>>;

    constexpr auto simd_size = simd_type::size();
    if (_size > simd_size) {
        auto const*      almost_end = it1_end - (_size % simd_size);
        simd_utype const big_a{'A'};
        simd_utype const diff{'a' - 'A'};
        for (; it1 != almost_end; it1 += simd_size, it2 += simd_size) {
            auto const values1  = webpp::eve::bit_cast(simd_type{it1}, webpp::eve::as_<simd_utype>());
            auto const values2  = webpp::eve::bit_cast(simd_type{it2}, webpp::eve::as_<simd_utype>());
            auto const equality = webpp::eve::is_not_equal(values1, values2);
            if (webpp::eve::any(equality)) {
                auto const val1_lowered = webpp::eve::logical_not(webpp::eve::if_else(
                  webpp::eve::is_less(webpp::eve::sub(values1, big_a), 25),
                  webpp::eve::add(values1, diff),
                  values1));
                auto const val2_lowered = webpp::eve::logical_not(webpp::eve::if_else(
                  webpp::eve::is_less(webpp::eve::sub(values1, big_a), 25),
                  webpp::eve::add(values1, diff),
                  values1));
                auto const equality2    = webpp::eve::is_not_equal(val1_lowered, val2_lowered);
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
            if (ch1_lowered != ch2_lowered) {
                return false;
            }
        }
    }
    return true;
}

static void IEQ_Strcasecmp(benchmark::State& state) {
    int index = 0;
    for (auto _ : state) {
        auto const& str   = strs[index++ % strs.size()];
        auto        istr  = str;
        auto        istr2 = webpp::ascii::to_lower_copy(istr);
        auto        res   = ::strcasecmp(istr.data(), istr2.data());
        benchmark::DoNotOptimize(res == 0);
        benchmark::DoNotOptimize(istr);
        benchmark::DoNotOptimize(istr2);
    }
}

BENCHMARK(IEQ_Strcasecmp);

static void IEQ_Strncasecmp(benchmark::State& state) {
    int index = 0;
    for (auto _ : state) {
        auto const& str   = strs[index++ % strs.size()];
        auto        istr  = str;
        auto        istr2 = webpp::ascii::to_lower_copy(istr);
        auto        res   = ::strncasecmp(istr.data(), istr2.data(), istr.size());
        benchmark::DoNotOptimize(res == 0);
        benchmark::DoNotOptimize(istr);
        benchmark::DoNotOptimize(istr2);
    }
}

BENCHMARK(IEQ_Strncasecmp);

static void IEQ_Default(benchmark::State& state) {
    int index = 0;
    for (auto _ : state) {
        auto const& str   = strs[index++ % strs.size()];
        auto        istr  = str;
        auto        istr2 = webpp::ascii::to_lower_copy(istr);
        auto        res   = webpp::ascii::iequals(istr, istr2);
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(istr);
        benchmark::DoNotOptimize(istr2);
    }
}

BENCHMARK(IEQ_Default);

static void IEQ_DefaultLowered(benchmark::State& state) {
    int index = 0;
    for (auto _ : state) {
        auto const& str   = strs[index++ % strs.size()];
        auto        istr  = str;
        auto        istr2 = webpp::ascii::to_lower_copy(istr);
        auto        res   = webpp::ascii::iequals<webpp::ascii::char_case_side::second_lowered>(istr, istr2);
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(istr);
        benchmark::DoNotOptimize(istr2);
    }
}

BENCHMARK(IEQ_DefaultLowered);

static void IEQ_SIMD(benchmark::State& state) {
    int index = 0;
    for (auto _ : state) {
        auto const& str   = strs[index++ % strs.size()];
        auto        istr  = str;
        auto        istr2 = webpp::ascii::to_lower_copy(istr);
        auto        res   = iequals_simd(istr, istr2);
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(istr);
        benchmark::DoNotOptimize(istr2);
    }
}

BENCHMARK(IEQ_SIMD);

static void IEQ_ToLowerAllTheWay(benchmark::State& state) {
    int index = 0;
    for (auto _ : state) {
        auto const& str   = strs[index++ % strs.size()];
        auto        istr  = str;
        auto        istr2 = webpp::ascii::to_lower_copy(istr);
        benchmark::DoNotOptimize(iequal_tolower_all_the_way(istr, istr2));
        benchmark::DoNotOptimize(istr);
        benchmark::DoNotOptimize(istr2);
    }
}

BENCHMARK(IEQ_ToLowerAllTheWay);

static void IEQ_SimpleForLoop(benchmark::State& state) {
    int index = 0;
    for (auto _ : state) {
        auto const& str   = strs[index++ % strs.size()];
        auto        istr  = str;
        auto        istr2 = webpp::ascii::to_lower_copy(istr);
        benchmark::DoNotOptimize(simple_for_loop(istr, istr2));
        benchmark::DoNotOptimize(istr);
        benchmark::DoNotOptimize(istr2);
    }
}

BENCHMARK(IEQ_SimpleForLoop);

static void IEQ_SimplerForLoop(benchmark::State& state) {
    int index = 0;
    for (auto _ : state) {
        auto const& str   = strs[index++ % strs.size()];
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
    int index = 0;
    for (auto _ : state) {
        auto const& str   = strs[index++ % strs.size()];
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
    int index = 0;
    for (auto _ : state) {
        auto const& str   = strs[index++ % strs.size()];
        auto        istr  = str;
        auto        istr2 = webpp::ascii::to_lower_copy(istr);
        benchmark::DoNotOptimize(boost::beast::iequals(istr, istr2));
        benchmark::DoNotOptimize(istr);
        benchmark::DoNotOptimize(istr2);
    }
}

BENCHMARK(IEQ_Boost_Beast);
#endif


static void IEQ_IgnoreSpecialCharacters(benchmark::State& state) {
    int index = 0;
    for (auto _ : state) {
        auto const& str   = strs[index++ % strs.size()];
        auto        istr  = str;
        auto        istr2 = webpp::ascii::to_lower_copy(istr);
        auto        res   = test::iiequals<true, std::string_view, std::string_view>(istr, istr2);
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(istr);
        benchmark::DoNotOptimize(istr2);
    }
}

BENCHMARK(IEQ_IgnoreSpecialCharacters);

static void IEQ_IgnoreSpecialCharacters2(benchmark::State& state) {
    int index = 0;
    for (auto _ : state) {
        auto const& str   = strs[index++ % strs.size()];
        auto        istr  = str;
        auto        istr2 = webpp::ascii::to_lower_copy(istr);
        auto        res   = test2::iiequals<true, std::string_view, std::string_view>(istr, istr2);
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(istr);
        benchmark::DoNotOptimize(istr2);
    }
}

BENCHMARK(IEQ_IgnoreSpecialCharacters2);

static void IEQ_IgnoreSpecialCharacters3(benchmark::State& state) {
    int index = 0;
    for (auto _ : state) {
        auto const& str   = strs[index++ % strs.size()];
        auto        istr  = str;
        auto        istr2 = webpp::ascii::to_lower_copy(istr);
        auto        res   = test3::iiequals<true, std::string_view, std::string_view>(istr, istr2);
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(istr);
        benchmark::DoNotOptimize(istr2);
    }
}

BENCHMARK(IEQ_IgnoreSpecialCharacters3);
