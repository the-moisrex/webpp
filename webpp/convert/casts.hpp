#ifndef WEBPP_CASTS_HPP
#define WEBPP_CASTS_HPP

#include "../std/string_view.hpp"
#include "../std/utility.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>

// NOLINTBEGIN(*-magic-numbers)
namespace webpp {

    enum struct integer_casting_errors : stl::uint8_t {
        no_error = 0,

        invalid_character = 1, // includes an invalid character
        invalid_base      = 2, // not a valid character in the specified base

        error_count = 3,       // not an error, represents the number of errors in this enum
    };

    constexpr stl::string_view to_string(integer_casting_errors const err) noexcept {
        using enum integer_casting_errors;
        switch (err) {
            case no_error: return {"No Error."};
            case invalid_character: return {"Invalid character found"};
            case invalid_base: return {"The specified string contains characters that are not in the valid base"};
            default: return {"Unknown error"};
        }
        return {}; // to get rid of warnings; it's 2023 for God’s sake!
    }

    /**
     * Casting result for safety reasons mostly.
     * This is pretty much `std::expected<T, integer_casting_errors>`
     */
    template <std::integral T>
    struct [[nodiscard]] integer_cast_result {
        // todo: for std::uint8_t and friends, return the error as well
        static constexpr T error_start =
          (std::numeric_limits<T>::max() - stl::to_underlying(integer_casting_errors::error_count));

      private:
        T result;

      public:
        consteval explicit(false) integer_cast_result(integer_casting_errors const err) noexcept
          : result{static_cast<T>(error_start + stl::to_underlying(err))} {}

        constexpr explicit(false) integer_cast_result(T const inp_value) noexcept : result{inp_value} {
            assert(has_value());
        }

        constexpr integer_cast_result(integer_cast_result const&)                = default;
        constexpr integer_cast_result(integer_cast_result&&) noexcept            = default;
        constexpr integer_cast_result& operator=(integer_cast_result const&)     = default;
        constexpr integer_cast_result& operator=(integer_cast_result&&) noexcept = default;
        constexpr ~integer_cast_result() noexcept                                = default;

        [[nodiscard]] explicit(false) constexpr operator T() const noexcept { // NOLINT(*-explicit*)
            return result;
        }

        [[nodiscard]] explicit constexpr operator bool() const noexcept {
            return has_value();
        }

        [[nodiscard]] constexpr T value() const noexcept {
            return result;
        }

        [[nodiscard]] constexpr T value_safe() const noexcept(false) {
            if (!has_value()) [[unlikely]] {
                auto const err = to_string(error());
                throw std::invalid_argument{
                  std::string{err.data(), err.size()}
                };
            }
            return result;
        }

        [[nodiscard]] constexpr integer_casting_errors error() const noexcept {
            using enum integer_casting_errors;
            return has_value() ? no_error : static_cast<integer_casting_errors>(result - error_start);
        }

        [[nodiscard]] constexpr T value_or(T const default_value) const noexcept {
            return has_value() ? result : default_value;
        }

        [[nodiscard]] constexpr bool has_value() const noexcept {
            return result < error_start;
        }
    };

    /**
     * In this algorithm we're using begin, end, ... because in some string types (like utf-8), the chars
     * are not exactly stored the way we want them to be for that.
     *
     * todo: check overflows as well
     */
    template <typename T, T base = 10, istl::StringViewifiable StrT = stl::string_view>
    constexpr integer_cast_result<T> to(StrT&& _str) noexcept {
        using enum integer_casting_errors;
        /**
         * glibc's implementation if you need help: https://fossies.org/linux/glib/glib/gstrfuncs.c
         */

        auto const str = istl::view(stl::forward<StrT>(_str));
        T          ret = 0;
        if (!str.size()) {
            return ret;
        }

        auto pos = str.begin();
        if (*pos == '-' || *pos == '+') {
            ++pos; // first character can be - or +
        }
        for (; pos != str.end(); ++pos) {
            auto cur_ch = *pos;
            if constexpr (base <= 10) {
                if (cur_ch < '0' || cur_ch > '9') [[unlikely]] {
                    return invalid_character;
                }
                cur_ch -= '0';
            } else if (base > 10) {
                if (cur_ch >= 'a') {
                    cur_ch -= 'a' - 10;
                } else if (cur_ch >= 'A') {
                    cur_ch -= 'A' - 10;
                } else {
                    cur_ch -= '0';
                }
                if (cur_ch > base) [[unlikely]] {
                    return invalid_base;
                }
            }
            ret *= base;
            ret += static_cast<T>(cur_ch);
        }
        ret *= static_cast<T>(str.front() == '-' ? -1 : 1);
        return ret;
    }

    // NOLINTNEXTLINE(*-macro-usage)
#define WEBPP_TO_FUNCTION(name, type)                                          \
    template <type base = 10, istl::StringViewifiable StrT = stl::string_view> \
    constexpr auto to_##name(StrT&& str) noexcept {                            \
        return to<type, base, StrT>(stl::forward<StrT>(str));                  \
    }                                                                          \
                                                                               \
    template <type base = 10, istl::StringViewifiable StrT = stl::string_view> \
    constexpr auto try_to_##name(StrT&& str) noexcept {                        \
        return to<type, base, StrT>(stl::forward<StrT>(str));                  \
    }

    WEBPP_TO_FUNCTION(int, int)
    WEBPP_TO_FUNCTION(int8, int8_t)
    WEBPP_TO_FUNCTION(int16, int16_t)
    WEBPP_TO_FUNCTION(int32, int32_t)
    WEBPP_TO_FUNCTION(int64, int64_t)
    WEBPP_TO_FUNCTION(uint, unsigned int)
    WEBPP_TO_FUNCTION(uint8, uint8_t)
    WEBPP_TO_FUNCTION(uint16, uint16_t)
    WEBPP_TO_FUNCTION(uint32, uint32_t)
    WEBPP_TO_FUNCTION(uint64, uint64_t)
    WEBPP_TO_FUNCTION(size_t, stl::size_t)

#undef WEBPP_TO_FUNCTION


} // namespace webpp

// NOLINTEND(*-magic-numbers)

#endif // WEBPP_CASTS_HPP
