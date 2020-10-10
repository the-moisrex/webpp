// Created by moisrex on 10/10/20.

#ifndef WEBPP_IEQUALS_HPP
#define WEBPP_IEQUALS_HPP

#include "./to_case.hpp"

namespace webpp::ascii {

    enum struct char_case { lowered, uppered, unknown };

    enum struct char_case_side {
        first_lowered,
        second_lowered,
        both_lowered,
        first_uppered,
        second_uppered,
        both_uppered,
        both_unknown
    };

    [[nodiscard]] static constexpr char_case_side char_case_to_side(char_case c) noexcept {
        switch (c) {
            case char_case::lowered: return char_case_side::both_lowered;
            case char_case::uppered: return char_case_side::both_uppered;
            case char_case::unknown: return char_case_side::both_unknown;
        }
    }

    [[nodiscard]] static constexpr char_case_side char_case_to_side(char_case a, char_case b) noexcept {
        switch(a) {
            case char_case::lowered:
                switch(b) {
                    case char_case::lowered:
                        return char_case_side::both_lowered;
                    default:
                        return char_case_side::first_lowered;
                }
            case char_case::uppered:
                switch (b) {
                    case char_case::uppered:
                        return char_case_side::both_uppered;
                    default:
                        return char_case_side::first_uppered;
                }
            case char_case::unknown:
                switch (b) {
                    case char_case::lowered:
                        return char_case_side::second_lowered;
                    case char_case::uppered:
                        return char_case_side::second_uppered;
                    case char_case::unknown:
                        return char_case_side::both_unknown;
                }
        }
    }
    /**
     * Compare two chars case-insensitively.
     * todo: benchmark this
     */
    template <char_case_side Side = char_case_side::both_unknown, istl::CharType CharT>
    [[nodiscard]] static constexpr bool iequals(CharT a, CharT b) noexcept {
        if constexpr (char_case_side::both_lowered == Side || char_case_side::both_uppered == Side) {
            return a == b;
        } else if constexpr (char_case_side::first_lowered == Side) {
            return a == b || a == to_lower_copy(b);
        } else if constexpr (char_case_side::first_uppered == Side) {
            return a == b || a == to_upper_copy(b);
        } else if constexpr (char_case_side::second_lowered == Side) {
            return a == b || to_lower_copy(a) == b;
        } else if constexpr (char_case_side::second_uppered == Side) {
            return a == b || to_upper_copy(a) == b;
        } else {
            if (a == b)
                return true;
            to_lower(a);
            to_lower(b);
            return a == b;
        }
    }

    template <char_case_side Side = char_case_side::both_unknown, istl::CharType CharT>
    [[nodiscard]] static constexpr bool iequals(istl::ConvertibleToStringView auto&& a, CharT b) noexcept {
        if (a.size() != 1)
            return false;
        return iequals<Side, CharT>(a[0], b);
    }

    template <char_case_side Side = char_case_side::both_unknown, istl::CharType CharT>
    [[nodiscard]] static constexpr bool iequals(CharT a, istl::ConvertibleToStringView auto&& b) noexcept {
        if (a.size() != 1)
            return false;
        return iequals<Side, CharT>(a, b[0]);
    }

    /**
     * Check if two strings are equal case-insensitively
     */
    template <char_case_side Side = char_case_side::both_unknown>
    [[nodiscard]] static inline bool iequals(istl::ConvertibleToStringView auto&& _str1,
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

        if constexpr (char_case_side::both_lowered == Side || char_case_side::both_uppered == Side) {
            return istl::to_string_view(_str1) == istl::to_string_view(_str2);
        } else {

            auto*       it1     = istl::string_data(_str1);
            auto*       it2     = istl::string_data(_str2);
            const auto* it1_end = it1 + _size;

#ifdef WEBPP_EVE
            using simd_type  = eve::wide<char_type>;
            using simd_utype = eve::wide<stl::make_unsigned_t<char_type>>;

            constexpr auto simd_size = simd_type::size();
            if (_size > simd_size) {
                const auto*      almost_end = it1_end - (_size % simd_size);
                const simd_utype big_a{'A'};
                const simd_utype small_a{'a'};
                const simd_utype diff{'a' - 'A'};
                for (; it1 != almost_end; it1 += simd_size, it2 += simd_size) {
                    const auto values1  = eve::bit_cast(simd_type{it1}, eve::as_<simd_utype>());
                    const auto values2  = eve::bit_cast(simd_type{it2}, eve::as_<simd_utype>());
                    const auto equality = eve::is_not_equal(values1, values2);
                    if (eve::any(equality)) {
                        if constexpr (char_case_side::first_lowered == Side) {
                            const auto val2_lowered = eve::if_else(eve::is_less(eve::sub(values2, big_a), 25),
                                                                   eve::add(values2, diff), values2);
                            const auto equality2    = eve::is_not_equal(values1, val2_lowered);
                            if (eve::any(equality2)) {
                                return false;
                            }
                        } else if constexpr (char_case_side::second_lowered == Side) {
                            const auto val1_lowered = eve::if_else(eve::is_less(eve::sub(values1, big_a), 25),
                                                                   eve::add(values1, diff), values1);
                            const auto equality2    = eve::is_not_equal(val1_lowered, values2);
                            if (eve::any(equality2)) {
                                return false;
                            }
                        } else if constexpr (char_case_side::first_uppered == Side) {
                            const auto val2_uppered = eve::if_else(
                              eve::is_less(eve::sub(values2, small_a), 25), eve::sub(values2, diff), values2);
                            const auto equality2 = eve::is_not_equal(values1, val2_uppered);
                            if (eve::any(equality2)) {
                                return false;
                            }
                        } else if constexpr (char_case_side::second_uppered == Side) {
                            const auto val1_uppered = eve::if_else(
                              eve::is_less(eve::sub(values1, small_a), 25), eve::sub(values1, diff), values1);
                            const auto equality2 = eve::is_not_equal(val1_uppered, values2);
                            if (eve::any(equality2)) {
                                return false;
                            }
                        } else {
                            const auto val1_lowered = eve::if_else(eve::is_less(eve::sub(values1, big_a), 25),
                                                                   eve::add(values1, diff), values1);
                            const auto val2_lowered = eve::if_else(eve::is_less(eve::sub(values2, big_a), 25),
                                                                   eve::add(values2, diff), values2);
                            const auto equality2    = eve::is_not_equal(val1_lowered, val2_lowered);
                            if (eve::any(equality2)) {
                                return false;
                            }
                        }
                    }
                }
                // do the rest
                it1 -= simd_size;
                it2 -= simd_size;
            }
            // todo: SIMDify this part as well, you can do better by re-calculating: https://youtu.be/1ir_nEfKQ7A?t=402
#endif
            for (; it1 != it1_end; ++it1, ++it2) {
                if (*it1 != *it2) {
                    // compiler seems to be able to optimize this better than us
                    if constexpr (char_case_side::first_lowered == Side) {
                        auto ch2_lowered = to_lower_copy(*it2);
                        if (*it1 != ch2_lowered)
                            return false;
                    } else if constexpr (char_case_side::second_lowered == Side) {
                        auto ch1_lowered = to_lower_copy(*it1);
                        if (ch1_lowered != *it2)
                            return false;
                    } else if constexpr (char_case_side::first_uppered == Side) {
                        auto ch2_uppered = to_upper_copy(*it2);
                        if (*it1 != ch2_uppered)
                            return false;
                    } else if constexpr (char_case_side::second_uppered == Side) {
                        auto ch1_uppered = to_upper_copy(*it1);
                        if (ch1_uppered == *it2)
                            return false;
                    } else {
                        auto ch1_lowered = to_lower_copy(*it1);
                        auto ch2_lowered = to_lower_copy(*it2);
                        if (ch1_lowered != ch2_lowered)
                            return false;
                    }
                }
            }
            return true;
        }


        //        if constexpr (istl::String<str1_t> && istl::String<str1_t> &&
        //        stl::is_rvalue_reference_v<str1_type> &&
        //                      stl::is_rvalue_reference_v<str2_type>) {
        //            to_lower(_str1);
        //            to_lower(_str2);
        //            return _str1 == _str2;
        //        } else if constexpr (istl::String<str1_t> && stl::is_rvalue_reference_v<str1_type>) {
        //            to_lower(_str1);
        //            return _str1 == to_lower_copy(_str2, _str1.get_allocator());
        //        } else if constexpr (istl::String<str2_t> && stl::is_rvalue_reference_v<str2_type>) {
        //            to_lower(_str2);
        //            return to_lower_copy(_str1, _str2.get_allocator()) == _str2;
        //        } else {
        //            return stl::equal(str1.cbegin(), str1.cend(), str2.cbegin(), [](auto&& c1, auto&& c2) {
        //                return c1 == c2 || to_lower_copy(c1) == to_lower_copy(c2);
        //            });
        //        }
    }

} // namespace webpp::ascii

#endif // WEBPP_IEQUALS_HPP
