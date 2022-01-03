// Created by moisrex on 10/10/20.

#ifndef WEBPP_IEQUALS_HPP
#define WEBPP_IEQUALS_HPP

#include "./size.hpp"
#include "./to_case.hpp"

#ifdef WEBPP_EVE
#    include <eve/algo/equal.hpp>
#endif


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
        switch (a) {
            case char_case::lowered:
                switch (b) {
                    case char_case::lowered: return char_case_side::both_lowered;
                    default: return char_case_side::first_lowered;
                }
            case char_case::uppered:
                switch (b) {
                    case char_case::uppered: return char_case_side::both_uppered;
                    default: return char_case_side::first_uppered;
                }
            case char_case::unknown:
                switch (b) {
                    case char_case::lowered: return char_case_side::second_lowered;
                    case char_case::uppered: return char_case_side::second_uppered;
                    case char_case::unknown: return char_case_side::both_unknown;
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
    [[nodiscard]] static constexpr bool iequals(istl::StringViewifiable auto&& a, CharT b) noexcept {
        if (a.size() != 1)
            return false;
        return iequals<Side, CharT>(a[0], b);
    }

    template <char_case_side Side = char_case_side::both_unknown, istl::CharType CharT>
    [[nodiscard]] static constexpr bool iequals(CharT a, istl::StringViewifiable auto&& b) noexcept {
        if (a.size() != 1)
            return false;
        return iequals<Side, CharT>(a, b[0]);
    }

    /**
     * Check if two strings are equal case-insensitively
     * todo: update with the new eve project tools; example: https://godbolt.org/z/qzjsG4Prd
     */
    template <char_case_side Side = char_case_side::both_unknown>
    [[nodiscard]] static inline bool iequals(istl::StringViewifiable auto&& _str1,
                                             istl::StringViewifiable auto&& _str2) noexcept {
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
            return istl::string_viewify(_str1) == istl::string_viewify(_str2);
        } else {



#ifdef WEBPP_EVE

            using uchar_type = stl::make_unsigned_t<char_type>;
            // converting them to uint8_t; because our to upper algorithm relies on unsigned integers.
            auto* f1 = reinterpret_cast<uchar_type const*>(istl::string_data(_str1));
            auto* l1 = f1 + size(_str1);
            auto* f2 = reinterpret_cast<uchar_type const*>(istl::string_data(_str2));

            using simd_utype = eve::wide<uchar_type>;

            struct equal_checker {

                const uchar_type alphabet_length = 'z' - 'a';
                const uchar_type a_A_offset      = 'a' - 'A';

                constexpr auto to_upper(simd_utype c) const noexcept {
                    // eve::sub[condition](a, b) is an equivalent to `eve::if_else(condition, a - b, a)`
                    // but it will also use masked instructions when those are avaliable.

                    // Traditional `to_upper` is written like 'a' <= c && c <= 'z`.
                    // However, we can utilize underflow(overflow?) when subtracting unsigned numbers.
                    // c - 'a' <= 'z' -'a' if and only if 'a' <= c && c <= 'z'

                    return eve::sub[(c - 'a') <= alphabet_length](c, a_A_offset);
                }

                constexpr auto to_lower(simd_utype c) const noexcept {
                    return eve::add[(c - 'A') <= alphabet_length](c, a_A_offset);
                }

                constexpr auto operator()(simd_utype a, simd_utype b) const noexcept {
                    if constexpr (char_case_side::first_uppered == Side) {
                        return a == to_upper(b);
                    } else if constexpr (char_case_side::second_uppered == Side) {
                        return to_upper(a) == b;
                    } else if constexpr (char_case_side::first_lowered == Side) {
                        return a == to_lower(b);
                    } else if constexpr (char_case_side::second_lowered == Side) {
                        return to_lower(a) == b;
                    } else if constexpr (char_case_side::both_uppered == Side ||
                                         char_case_side::both_lowered == Side) {
                        return a == b;
                    } else {
                        return to_lower(a) == to_lower(b);
                    }
                }
            };

            return eve::algo::equal(eve::algo::as_range(f1, l1), f2, equal_checker{});
#else
            auto*       it1     = istl::string_data(_str1);
            auto*       it2     = istl::string_data(_str2);
            const auto* it1_end = it1 + _size;

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
#endif
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
