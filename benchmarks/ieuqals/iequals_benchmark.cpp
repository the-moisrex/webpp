#include "../benchmark.hpp"

// clang-format off
#include webpp_include(utils/strings)
// clang-format on

#include <boost/algorithm/string/predicate.hpp>

using namespace webpp;





[[nodiscard]] constexpr bool iequal_tolower_all_the_way(istl::ConvertibleToStringView auto&& _str1,
                                                        istl::ConvertibleToStringView auto&& _str2) noexcept {
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

    auto str1 = istl::to_string_view(_str1);
    auto str2 = istl::to_string_view(_str2);


    if constexpr (istl::String<str1_t> && istl::String<str1_t> && stl::is_rvalue_reference_v<str1_type> &&
                  stl::is_rvalue_reference_v<str2_type>) {
        to_lower(_str1);
        to_lower(_str2);
        return _str1 == _str2;
    } else if constexpr (istl::String<str1_t> && stl::is_rvalue_reference_v<str1_type>) {
        to_lower(_str1);
        return _str1 == to_lower_copy(_str2, _str1.get_allocator());
    } else if constexpr (istl::String<str2_t> && stl::is_rvalue_reference_v<str2_type>) {
        to_lower(_str2);
        return to_lower_copy(_str1, _str2.get_allocator()) == _str2;
    } else {
        return stl::equal(str1.cbegin(), str1.cend(), str2.cbegin(), [](auto&& c1, auto&& c2) {
            return c1 == c2 || to_lower_copy(c1) == to_lower_copy(c2);
        });
    }
}





[[nodiscard]] constexpr bool simple_for_loop(istl::ConvertibleToStringView auto&& _str1,
                                             istl::ConvertibleToStringView auto&& _str2) noexcept {
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
            auto ch1_lowered = to_lower_copy(*it1);
            auto ch2_lowered = to_lower_copy(*it2);
            if (ch1_lowered != ch2_lowered)
                return false;
        }
    }
    return true;
}



[[nodiscard]] constexpr bool simpler_for_loop(istl::ConvertibleToStringView auto&& _str1,
                                              istl::ConvertibleToStringView auto&& _str2) noexcept {
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
            auto ch1_lowered = to_lower_copy(*it1);
            auto ch2_lowered = to_lower_copy(*it2);
            if (ch1_lowered != ch2_lowered)
                return false;
        }
    }
    return true;
}


/**
 * Check if two strings are equal case-insensitively
 */
[[nodiscard]] constexpr bool iequals_simd(istl::ConvertibleToStringView auto&& _str1,
                                          istl::ConvertibleToStringView auto&& _str2) noexcept {
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
            auto ch1_lowered = to_lower_copy(*it1);
            auto ch2_lowered = to_lower_copy(*it2);
            if (ch1_lowered != ch2_lowered)
                return false;
        }
    }
    return true;
}


static void IEQ_Default(benchmark::State& state) {
    std::string str   = str_generator();
    auto        istr  = str;
    auto        istr2 = webpp::to_lower_copy(istr);
    for (auto _ : state) {
        auto res = webpp::iequals(istr, istr2);
        benchmark::DoNotOptimize(res);
    }
}
BENCHMARK(IEQ_Default);

static void IEQ_SIMD(benchmark::State& state) {
    std::string str   = str_generator();
    auto        istr  = str;
    auto        istr2 = webpp::to_lower_copy(istr);
    for (auto _ : state) {
        auto res = iequals_simd(istr, istr2);
        benchmark::DoNotOptimize(res);
    }
}
BENCHMARK(IEQ_SIMD);

static void IEQ_ToLowerAllTheWay(benchmark::State& state) {
    std::string str   = str_generator();
    auto        istr  = str;
    auto        istr2 = webpp::to_lower_copy(istr);
    for (auto _ : state) {
        benchmark::DoNotOptimize(iequal_tolower_all_the_way(istr, istr2));
    }
}
BENCHMARK(IEQ_ToLowerAllTheWay);



static void IEQ_SimpleForLoop(benchmark::State& state) {
    std::string str   = str_generator();
    auto        istr  = str;
    auto        istr2 = webpp::to_lower_copy(istr);
    for (auto _ : state) {
        benchmark::DoNotOptimize(simple_for_loop(istr, istr2));
    }
}
BENCHMARK(IEQ_SimpleForLoop);


static void IEQ_SimplerForLoop(benchmark::State& state) {
    std::string str   = str_generator();
    auto        istr  = str;
    auto        istr2 = webpp::to_lower_copy(istr);
    for (auto _ : state) {
        benchmark::DoNotOptimize(simpler_for_loop(istr, istr2));
    }
}
BENCHMARK(IEQ_SimplerForLoop);




static void IEQ_Boost(benchmark::State& state) {
    std::string str   = str_generator();
    auto        istr  = str;
    auto        istr2 = webpp::to_lower_copy(istr);
    for (auto _ : state) {
        benchmark::DoNotOptimize(boost::iequals(istr, istr2));
    }
}
BENCHMARK(IEQ_Boost);
