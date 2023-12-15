// Created by moisrex on 12/21/20.

#ifndef WEBPP_UNICODE_CHAR_TRAITS_HPP
#define WEBPP_UNICODE_CHAR_TRAITS_HPP

#if __has_include(<compare>)
#    include <compare>
#    define has_compare
#endif

#include "unicode.hpp"

#include <algorithm>

namespace webpp {

    /**
     *  @brief  Mapping from character type to associated types.
     *
     *  @note This is an implementation class for the generic version
     *  of char_traits.  It defines int_type, off_type, pos_type, and
     *  state_type.  By default these are unsigned long, streamoff,
     *  streampos, and mbstate_t.  Users who need a different set of
     *  types, but who don't need to change the definitions of any function
     *  defined in char_traits, can specialize __gnu_cxx::_Char_types
     *  while leaving __gnu_cxx::char_traits alone. */
    template <typename CharT>
    struct char_types {
        using int_type   = unsigned long;
        using pos_type   = stl::streampos;
        using off_type   = stl::streamoff;
        using state_type = stl::mbstate_t;
    };

    template <typename CharT>
    struct unicode_char_traits {
        using char_type  = CharT;
        using int_type   = typename char_types<char_type>::int_type;
        using pos_type   = typename char_types<char_type>::pos_type;
        using off_type   = typename char_types<char_type>::off_type;
        using state_type = typename char_types<char_type>::state_type;

#ifdef has_compare
        // for 3-way comparison
        using comparison_category = stl::strong_ordering;
#endif

        static constexpr void assign(char_type& ch1, char_type const& ch2) {
            ch1 = ch2;
        }

        static constexpr bool eq(char_type const& ch1, char_type const& ch2) {
            return ch1 == ch2;
        }

        static constexpr bool lt(char_type const& ch1, char_type const& ch2) {
            return ch1 < ch2;
        }

        static constexpr int compare(char_type const* str1, char_type const* str2, stl::size_t const count) {
            for (stl::size_t i = 0; i < count; ++i) {
                if (lt(str1[i], str2[i])) {
                    return -1;
                }
                if (lt(str2[i], str1[i])) {
                    return 1;
                }
            }
            return 0;
        }

        static constexpr stl::size_t length(char_type const* ch_ptr) noexcept {
            return unicode::unchecked::count(stl::addressof(ch_ptr->value));
            // stl::size_t      i = 0;
            // const char_type* p = pp;
            // while (!eq(*p, char_type())) {
            //     if constexpr (sizeof(char_type) == sizeof(char8_t)) {
            //         if (*p < static_cast<char_type>(0x80) || *p > static_cast<char_type>(0xBF)) {
            //             // ascii or first byte of a multi byte sequence
            //             ++i;
            //         }
            //     } else if constexpr (sizeof(char_type) == sizeof(char16_t)) {
            //         if (*p < 0xDC00 || *p > 0xDFFF) {
            //             ++i;
            //         }
            //     } else {
            //         ++i;
            //     }
            //     ++p;
            // }
            // return i;
        }

        static constexpr char_type const*
        find(char_type const* str, stl::size_t const count, char_type const& inp_ch) noexcept {
            for (stl::size_t i = 0; i < count; ++i) {
                if (eq(str[i], inp_ch)) {
                    return str + i;
                }
            }
            return nullptr;
        }

        static constexpr char_type* move(char_type* str1, char_type const* str2, stl::size_t const count) {
            if (count == 0) {
                return str1;
            }
#ifdef __cpp_lib_is_constant_evaluated
            if (stl::is_constant_evaluated()) {
                if (str1 > str2 && str1 < str2 + count) {
                    stl::copy_backward(str2, str2 + count, str1);
                } else {
                    stl::copy(str2, str2 + count, str1);
                }
                return str1;
            }
#endif
            // todo
            return static_cast<CharT*>(builtin_memmove(str1, str2, count * sizeof(char_type)));
        }

        static constexpr char_type* copy(char_type* str1, char_type const* str2, stl::size_t const count) {
            // NB: Inline stl::copy so no recursive dependencies.
            stl::copy(str2, str2 + count, str1);
            return str1;
        }

        static constexpr char_type*
        assign(char_type* str, stl::size_t const count, char_type inp_ch) noexcept {
            // NB: Inline stl::fill_n so no recursive dependencies.
            stl::fill_n(str, count, inp_ch);
            return str;
        }

        static constexpr char_type to_char_type(int_type const& inp_ch) {
            return static_cast<char_type>(inp_ch);
        }

        static constexpr int_type to_int_type(char_type const& inp_ch) {
            return static_cast<int_type>(inp_ch);
        }

        static constexpr bool eq_int_type(int_type const& ch1, int_type const& ch2) {
            return ch1 == ch2;
        }

        static constexpr int_type eof() {
            return static_cast<int_type>(-1);
        }

        static constexpr int_type not_eof(int_type const& c) {
            return !eq_int_type(c, eof()) ? c : to_int_type(char_type());
        }
    };


} // namespace webpp

#endif // WEBPP_UNICODE_CHAR_TRAITS_HPP
