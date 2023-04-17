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

        static constexpr void assign(char_type& c1, const char_type& c2) {
            c1 = c2;
        }

        static constexpr bool eq(const char_type& c1, const char_type& c2) {
            return c1 == c2;
        }

        static constexpr bool lt(const char_type& c1, const char_type& c2) {
            return c1 < c2;
        }


        static constexpr int compare(const char_type* s1, const char_type* s2, stl::size_t n) {
            for (stl::size_t i = 0; i < n; ++i)
                if (lt(s1[i], s2[i]))
                    return -1;
                else if (lt(s2[i], s1[i]))
                    return 1;
            return 0;
        }

        static constexpr stl::size_t length(const char_type* pp) noexcept {
            return unicode::unchecked::count(stl::addressof(pp->value));
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


        static constexpr const char_type*
        find(const char_type* s, stl::size_t n, const char_type& a) noexcept {
            for (stl::size_t i = 0; i < n; ++i)
                if (eq(s[i], a))
                    return s + i;
            return 0;
        }


        static constexpr char_type* move(char_type* s1, const char_type* s2, stl::size_t n) {
            if (n == 0)
                return s1;
#ifdef __cpp_lib_is_constant_evaluated
            if (stl::is_constant_evaluated()) {
                if (s1 > s2 && s1 < s2 + n)
                    stl::copy_backward(s2, s2 + n, s1);
                else
                    stl::copy(s2, s2 + n, s1);
                return s1;
            }
#endif
            // todo
            return static_cast<CharT*>(builtin_memmove(s1, s2, n * sizeof(char_type)));
        }


        static constexpr char_type* copy(char_type* s1, const char_type* s2, stl::size_t n) {
            // NB: Inline stl::copy so no recursive dependencies.
            stl::copy(s2, s2 + n, s1);
            return s1;
        }


        static constexpr char_type* assign(char_type* s, stl::size_t n, char_type a) noexcept {
            // NB: Inline stl::fill_n so no recursive dependencies.
            stl::fill_n(s, n, a);
            return s;
        }


        static constexpr char_type to_char_type(const int_type& c) {
            return static_cast<char_type>(c);
        }

        static constexpr int_type to_int_type(const char_type& c) {
            return static_cast<int_type>(c);
        }

        static constexpr bool eq_int_type(const int_type& c1, const int_type& c2) {
            return c1 == c2;
        }

        static constexpr int_type eof() {
            return static_cast<int_type>(-1);
        }

        static constexpr int_type not_eof(const int_type& c) {
            return !eq_int_type(c, eof()) ? c : to_int_type(char_type());
        }
    };


} // namespace webpp

#endif // WEBPP_UNICODE_CHAR_TRAITS_HPP
