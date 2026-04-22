#ifndef WEBPP_CASTS_HPP
#define WEBPP_CASTS_HPP

#include "../std/string_view.hpp"
#include "../std/utility.hpp"

#include <cassert>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>

// NOLINTBEGIN(*-magic-numbers)
namespace webpp {

    enum struct integer_casting_errors : stl::uint8_t {
        no_error = 0,

        invalid_character = 1, // includes an invalid character
        invalid_base      = 2, // not a valid character in the specified base
        overflow          = 3, // integer overflow detected
        negative_unsigned = 4, // negative value found

        error_count = 5,       // not an error, represents the number of errors in this enum
    };

    [[nodiscard]] static constexpr stl::string_view to_string(integer_casting_errors const err) noexcept {
        using enum integer_casting_errors;
        switch (err) {
            case no_error: return {"No Error."};
            case invalid_character: return {"Invalid character found"};
            case invalid_base: return {"The specified string contains characters that are not in the valid base"};
            case overflow: return {"Integer overflow: value exceeds type limits"};
            case negative_unsigned: return {"Trying to parse a negative value to an unsigned integer type."};
            default: return {"Unknown error"};
        }
        return {}; // to get rid of warnings; it's 2023 for God’s sake!
    }

    /**
     * Casting result for safety reasons mostly.
     * This is pretty much `stl::expected<T, integer_casting_errors>`
     *
     * todo: for signed integers, make negative values the errors
     * todo: for (u)int8 and probably (u)int16, don't use the values technique, we have storage to spare
     */
    template <stl::integral T>
    struct [[nodiscard]] integer_cast_result {
        // todo: for stl::uint8_t and friends, return the error as well
        static constexpr T error_start =
          (stl::numeric_limits<T>::max() - stl::to_underlying(integer_casting_errors::error_count));

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
                throw stl::invalid_argument{
                  stl::string{err.data(), err.size()}
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
            return result <= error_start;
        }

        [[nodiscard]] constexpr bool operator==(integer_casting_errors const err) const noexcept {
            return error() == err;
        }
    };

    /**
     * In this algorithm we're using begin, end, ... because in some string types (like utf-8), the chars
     * are not exactly stored the way we want them to be for that.
     */
    template <typename T, T base = 10, typename CharT>
    static constexpr integer_cast_result<T> to(stl::basic_string_view<CharT> const str) noexcept {
        using enum integer_casting_errors;
        /**
         * glibc's implementation if you need help: https://fossies.org/linux/glib/glib/gstrfuncs.c
         */

        T ret = 0;
        if (str.empty()) {
            return ret;
        }

        auto        pos         = str.begin();
        bool const  is_negative = (*pos == '-');
        constexpr T max_number =
          stl::numeric_limits<T>::max() - stl::to_underlying(integer_casting_errors::error_count);

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

            // Check for integer overflow
            if (ret > (max_number - static_cast<T>(cur_ch)) / base) [[unlikely]] {
                // Handle the edge case where the minimum negative value has an absolute value 1 higher than the max
                if constexpr (stl::is_signed_v<T>) {
                    if (is_negative && ret == max_number / base && static_cast<T>(cur_ch) == (max_number % base) + 1) {
                        return stl::numeric_limits<T>::min();
                    }
                }
                return overflow;
            }

            ret *= base;
            ret += static_cast<T>(cur_ch);
        }

        // We check for this here at the end to make sure if there is a bad character, the bad character error is being
        // returned and not negative unsigned error.
        if constexpr (stl::is_unsigned_v<T>) {
            if (is_negative) [[unlikely]] {
                return negative_unsigned;
            }
        }
        ret *= static_cast<T>(is_negative ? -1 : 1);
        return ret;
    }

    template <typename T, T base = 10>
    static constexpr integer_cast_result<T> to(stl::string_view const str) noexcept {
        return to<T, base, char>(str);
    }

    // NOLINTNEXTLINE(*-macro-usage)
#define WEBPP_TO_FUNCTION(name, type)                                                   \
    template <type base = 10, typename CharT>                                           \
    static constexpr auto to_##name(stl::basic_string_view<CharT> const str) noexcept { \
        return to<type, base>(str);                                                     \
    }                                                                                   \
                                                                                        \
    template <type base = 10>                                                           \
    static constexpr auto to_##name(stl::string_view const str) noexcept {              \
        return to<type, base, char>(str);                                               \
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
