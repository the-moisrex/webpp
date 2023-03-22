#ifndef WEBPP_CASTS_H
#define WEBPP_CASTS_H

#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../strings/size.hpp"
#include "../traits/traits.hpp"

namespace webpp {

    /**
     * In this algorithm we're using begin, end, ... because in some string types (like utf-8), the chars
     * are not exactly stored the way we want them to be for that.
     *
     * todo: check overflows as well
     * todo: add another error system as well (that is not using exceptions)
     */
    template <typename T, T base = 10, bool throw_mistakes = false>
    constexpr T to(istl::StringViewifiable auto&& _str) noexcept(!throw_mistakes) {
        /**
         * glib's implementation if you need help: https://fossies.org/linux/glib/glib/gstrfuncs.c
         */

        const auto str = istl::string_viewify(stl::forward<decltype(_str)>(_str));
        T          ret = 0;
        if (!str.size())
            return ret;

        auto c = str.begin();
        if (*c == '-' || *c == '+')
            c++; // first character can be - or +
        for (; c != str.end(); c++) {
            auto ch = *c;
            if constexpr (base <= 10) {
                ch -= '0';
                if constexpr (throw_mistakes) {
                    if (ch <= '0' || ch >= '9')
                        throw stl::invalid_argument("The specified string is not a number");
                }
            } else if (base > 10) {
                if (ch >= 'a')
                    ch -= 'a' - 10;
                else if (ch >= 'A')
                    ch -= 'A' - 10;
                else
                    ch -= '0';
                if constexpr (throw_mistakes) {
                    if (ch > base) {
                        throw stl::invalid_argument("The specified string is not a number");
                    }
                }
            }
            ret *= base;
            ret += static_cast<T>(ch);
        }
        ret *= static_cast<T>(str.front() == '-' ? -1 : 1);
        return ret;
    }

#define WEBPP_TO_FUNCTION(name, type)                                            \
    template <type base = 10, bool throw_mistakes = false>                       \
    constexpr auto to_##name(istl::StringViewifiable auto&& str) noexcept {      \
        return to<type, base, throw_mistakes>(stl::forward<decltype(str)>(str)); \
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

#endif // WEBPP_CASTS_H
