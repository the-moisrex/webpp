// Created by moisrex on 3/29/23.

#ifndef WEBPP_STD_TYPES_HPP
#define WEBPP_STD_TYPES_HPP

#include <string_view>
#include <typeinfo>

namespace webpp::istl {

    template <typename T>
    constexpr std::string_view type_name() noexcept;

    template <>
    constexpr std::string_view type_name<void>() noexcept {
        return {"void"};
    }

    namespace details {

        using type_name_prober = void;

        template <typename T>
        constexpr std::string_view wrapped_type_name() noexcept {
#ifdef __clang__
            return __PRETTY_FUNCTION__; // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
#elif defined(__GNUC__)
            return __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
            return __FUNCSIG__;
#else
            return typeid(T).name(); // Unsupported compiler
#endif
        }

        constexpr std::size_t wrapped_type_name_prefix_length() noexcept {
            return wrapped_type_name<type_name_prober>().find(type_name<type_name_prober>());
        }

        constexpr std::size_t wrapped_type_name_suffix_length() noexcept {
            return wrapped_type_name<type_name_prober>().length() - wrapped_type_name_prefix_length() -
                   type_name<type_name_prober>().length();
        }

    } // namespace details

    template <typename T>
    constexpr std::string_view type_name() noexcept {
        constexpr auto wrapped_name     = details::wrapped_type_name<T>();
        constexpr auto prefix_length    = details::wrapped_type_name_prefix_length();
        constexpr auto suffix_length    = details::wrapped_type_name_suffix_length();
        constexpr auto type_name_length = wrapped_name.length() - prefix_length - suffix_length;
        return wrapped_name.substr(prefix_length, type_name_length);
    }

} // namespace webpp::istl

#endif // WEBPP_STD_TYPES_HPP
