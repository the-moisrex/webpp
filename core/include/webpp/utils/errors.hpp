// Created by moisrex on 11/26/20.

#ifndef WEBPP_ERRORS_HPP
#define WEBPP_ERRORS_HPP

#include "./flags.hpp"
#include "../std/string_view.hpp"
#include <variant>

/**
 * Todo: update the error system if you ever see Matching got into C++ (hopefully in c++23)
 *
 * # Error Category:
 * These are a series of error types that are related to each other.
 *
 * Error Types:
 *   - Flag Errors
 *     - Success flag
 *     - Failure flag
 *   - Class Errors
 *     - Success type
 *     - Failure type
 *
 * Ways of passing:
 *   - throw as exception
 *   - return as a std::variant
 *   - as a member variable
 *
 * Flag Errors:
 * These are the error types that use `enum`s as their error types.
 * @code
 *   enum struct uri_errors {
 *      none            = success(),
 *      parsing_error   = failure("The specified input is not a valid uri"),
 *      path_not_found  = failure("Path not found"),
 *   }
 * @endcode
 *
 * Class Errors:
 * These are the error types that use `class` types as their error types.
 *
 * Fixme: the success and failure functions don't work. It's better to use 0, and 1 directly in enums
 *
 */
namespace webpp {


    namespace hashing {
        // initial source from: https://stackoverflow.com/questions/2111667/compile-time-string-hashing
        template <typename CharT>
        static constexpr unsigned const_hash(CharT const* input) noexcept {
            return *input ? static_cast<unsigned int>(*input) + 33 * const_hash(input + 1) : 5381;
        }

        // FNV-1a 32bit hashing algorithm.
        // initial source from: https://gist.github.com/Lee-R/3839813
        static constexpr stl::uint32_t fnv1a_32(istl::StringViewifiable auto&& _str) noexcept {
            auto const str = istl::string_viewify(stl::forward<decltype(_str)>(_str));
            auto const count = str.size();
            return ((count ? fnv1a_32(str.substr(0u, str.size() - 1u)) : 2166136261u) ^ str[count]) * 16777619u;
        }
    }

    template <typename T>
    static
#ifdef __cpp_consteval
      consteval
#else
      constexpr
#endif
      stl::underlying_type_t<T> success() noexcept {
        return 0;
    }

    template <typename T>
    static
#ifdef __cpp_consteval
    consteval
#else
    constexpr
#endif
    stl::underlying_type_t<T> success(istl::StringViewifiable auto&& _str) noexcept {
        return 0 - static_cast<stl::underlying_type_t<T>>(hashing::fnv1a_32(stl::forward<decltype(_str)>(_str)));
    }

    template <typename T>
    static
#ifdef __cpp_consteval
    consteval
#else
    constexpr
#endif
    stl::underlying_type_t<T> failure(istl::StringViewifiable auto&& _str) noexcept {
        return 1 + static_cast<stl::underlying_type_t<T>>(hashing::fnv1a_32(stl::forward<decltype(_str)>(_str)));
    }


    template <typename ErrEnum>
    struct error_handler : public flags::manager<ErrEnum> {
        using super = flags::manager<ErrEnum>;
        using type = typename super::type;

      private:
        [[nodiscard]] static constexpr type get_success_value() noexcept {
            const auto values = magic_enum::enum_values<type>();
            for (stl::size_t i = 0; i < values.size(); ++i)
                if (magic_enum::enum_integer(values[i]) < 0)
                    return values[i];
            return values[0];
        }

      public:
        static constexpr type success_value = get_success_value();

        constexpr error_handler() : super{success_value} {};
        constexpr error_handler(auto val) noexcept : super{val} {}

        operator bool() const noexcept {
            return is_success();
        }

        [[nodiscard]] constexpr bool is_success() const noexcept {
            return *this == success_value;
        }

        [[nodiscard]] constexpr bool is_failure() const noexcept {
            return *this != success_value;
        }


        /**
         * Set success
         */
        auto& success() noexcept {
            this->reset(success_value);
            return *this;
        }

        auto& failure(type val) noexcept {
            if (val != success_value) {
                this->off(success_value);
                this->on(val);
            }
            return *this;
        }
    };


    template <typename Type, typename EnumType>
    struct result : public stl::variant<Type, error_handler<EnumType>> {
        using error_type = error_handler<EnumType>;
        using type = Type;

        constexpr operator bool() const noexcept {
            return stl::holds_alternative<error_type>(*this);
        }
    };


    // todo: add a result type that can hold a value AND an error

}

#endif // WEBPP_ERRORS_HPP
