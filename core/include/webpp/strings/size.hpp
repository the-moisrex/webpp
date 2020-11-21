//
// Created by moisrex on 11/20/20.
//

#ifndef WEBPP_SIZE_HPP
#define WEBPP_SIZE_HPP

#include "../std/type_traits.hpp"
#include <cstring>
#include <limits>

namespace webpp::ascii {


    template <typename INT>
    constexpr auto digit_count() noexcept {
        uint_fast8_t t = 0;
        INT          a = stl::numeric_limits<int>::max();
        while (a /= 10)
            ++t;
        return t;
    }



    template <typename T>
    struct constexpr_array_type {
        static constexpr bool is_array = false;
    };

    template <typename T, stl::size_t N>
    struct constexpr_array_type<const T (&)[N]> {
        using array_type                          = T;
        static constexpr bool        is_array     = true;
        static constexpr stl::size_t array_length = N - (stl::is_same_v<T, char> || stl::is_same_v<T, wchar_t>); // for strings, we don't want to count the last null character
    };

    // it's best to use this for counting strings and not other types of arrays or containers,
    // there's one edge case you should be aware; it may not count the last char if it's the
    // size is known at compile time.
    // This method is also supports integer sizes
    template <typename T>
    [[nodiscard]] constexpr stl::size_t size(T&& str) noexcept {
        if constexpr (requires{ str.size(); }) {
            return str.size();
        } else if constexpr (requires { str.length(); }) {
            return str.length();
        } else if constexpr (constexpr_array_type<T>::is_array) {
            return constexpr_array_type<T>::array_length; // todo: I'm not sure if this works or not
        } else if constexpr (stl::is_same_v<stl::remove_cvref_t<T>, char*>) {
            return stl::strlen(str);
        } else {
            // todo: is it possible to optimize this with SIMD?
            const T* end = str;
            while (*end++ != 0)
                ;
            return end - str - 1;
        }
    }



    template <typename T>
    [[nodiscard]] constexpr stl::size_t max_size(T&& val) noexcept {
        if constexpr (stl::is_integral_v<stl::remove_cvref_t<T>>) {
            constexpr auto res = digit_count<stl::remove_cvref_t<T>>();
            return res;
            // todo: add floats as well
        } else {
            return size(stl::forward<T>(val));
        }
    }



}

#endif // WEBPP_SIZE_HPP
