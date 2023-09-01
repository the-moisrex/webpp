#ifndef WEBPP_CASTS_HPP
#define WEBPP_CASTS_HPP

#include "../std/string_view.hpp"
#include "../utils/error_handling.hpp"

#include <cstdint>
#include <stdexcept>

// NOLINTBEGIN(*-avoid-magic-numbers)
namespace webpp {

    enum struct integer_casting_errors {
        invalid_character, // includes an invalid character
        invalid_base       // not a valid character in the specified base
    };

    constexpr stl::string_view to_string(integer_casting_errors err) noexcept {
        using enum integer_casting_errors;
        switch (err) {
            case invalid_character: return "Invalid character found";
            case invalid_base:
                return "The specified string contains characters that are not in the valid base";
        }
        return ""; // to get rid of warnings; it's 2023 for God sake!
    }

    /**
     * In this algorithm we're using begin, end, ... because in some string types (like utf-8), the chars
     * are not exactly stored the way we want them to be for that.
     *
     * todo: check overflows as well
     */
    template <typename T,
              T                       base     = 10,
              error_handling_strategy strategy = error_handling_strategy::assume_safe,
              istl::StringViewifiable StrT     = stl::string_view>
    constexpr expected_strategy_t<strategy, T, integer_casting_errors>
    to(StrT&& _str) noexcept(is_noexcept(strategy)) {
        /**
         * glibc's implementation if you need help: https://fossies.org/linux/glib/glib/gstrfuncs.c
         */

        const auto str = istl::string_viewify(stl::forward<StrT>(_str));
        T          ret = 0;
        if (!str.size())
            return ret;

        auto c = str.begin();
        if (*c == '-' || *c == '+')
            c++; // first character can be - or +
        for (; c != str.end(); c++) {
            auto ch = *c;
            if constexpr (base <= 10) {
                if constexpr (strategy == error_handling_strategy::throw_errors) {
                    if (ch < '0' || ch > '9') {
                        throw stl::invalid_argument(
                          to_string(integer_casting_errors::invalid_character).data());
                    }
                } else if constexpr (strategy == error_handling_strategy::use_expected) {
                    if (ch < '0' || ch > '9') {
                        return stl::unexpected(integer_casting_errors::invalid_character);
                    }
                }
                ch -= '0';
            } else if (base > 10) {
                if (ch >= 'a')
                    ch -= 'a' - 10;
                else if (ch >= 'A')
                    ch -= 'A' - 10;
                else
                    ch -= '0';
                if constexpr (strategy == error_handling_strategy::throw_errors) {
                    if (ch > base) {
                        throw stl::invalid_argument(to_string(integer_casting_errors::invalid_base).data());
                    }
                } else if constexpr (strategy == error_handling_strategy::use_expected) {
                    if (ch > base) {
                        return stl::unexpected(integer_casting_errors::invalid_base);
                    }
                }
            }
            ret *= base;
            ret += static_cast<T>(ch);
        }
        ret *= static_cast<T>(str.front() == '-' ? -1 : 1);
        return ret;
    }

#define WEBPP_TO_FUNCTION(name, type)                                                  \
    template <type                    base     = 10,                                   \
              error_handling_strategy strategy = error_handling_strategy::assume_safe, \
              istl::StringViewifiable StrT     = stl::string_view>                     \
    constexpr auto to_##name(StrT&& str) noexcept {                                    \
        return to<type, base, strategy, StrT>(stl::forward<StrT>(str));                \
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
// NOLINTEND(*-avoid-magic-numbers)

#endif // WEBPP_CASTS_HPP
