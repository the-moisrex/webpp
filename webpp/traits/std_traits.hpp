// Created by moisrex on 4/28/20.

#ifndef WEBPP_STD_TRAITS_H
#define WEBPP_STD_TRAITS_H

#include "../logs/std_logger.hpp"

#include <string>
#include <string_view>

namespace webpp {

    template <typename CharT>
    struct basic_std_traits {
        using char_type   = CharT;
        using logger_type = stderr_logger;
        using string_view = stl::basic_string_view<char_type, stl::char_traits<char_type>>;

        struct general_allocator_descriptor {
            template <typename T>
            using allocator_type = stl::allocator<T>;

            template <typename T>
            [[nodiscard]] static constexpr stl::allocator<T> construct_allocator() noexcept {
                return {};
            }
        };

        /// standard library doesn't provide a monotonic allocator for std::allocator (only for pmr)
        /// "void" is used to indicate that we don't provide one
        using monotonic_allocator_descriptor = void;

        template <typename AllocT>
        using string = stl::basic_string<char_type, stl::char_traits<char_type>, AllocT>;
    };

    struct std_traits : basic_std_traits<char> {};

    struct std_wtraits : basic_std_traits<wchar_t> {};

    struct std_u8traits : basic_std_traits<char8_t> {};

    struct std_u16traits : basic_std_traits<char16_t> {};

    struct std_u32traits : basic_std_traits<char32_t> {};

} // namespace webpp

#endif // WEBPP_STD_TRAITS_H
