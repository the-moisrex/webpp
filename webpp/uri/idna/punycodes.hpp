#ifndef WEBPP_URL_PUNY_CODES_HPP
#define WEBPP_URL_PUNY_CODES_HPP

#include "../../std/string.hpp"

#include <cstdint>
#include <cstring>

namespace webpp::uri {

    /**
     * Default values for punycode parameters
     * From: https://www.rfc-editor.org/rfc/rfc3492.html#section-5
     */
    struct bootstring_parameters {
        // NOLINTBEGIN(*-magic-numbers)
        stl::int32_t  base         = 36;
        stl::int32_t  tmin         = 1;
        stl::int32_t  tmax         = 26;
        stl::int32_t  skew         = 38;
        stl::int32_t  damp         = 700;
        stl::int32_t  initial_bias = 72;
        stl::uint32_t initial_n    = 0x80U;
        // NOLINTEND(*-magic-numbers)
    };

    /**
     * Bias adaptation function
     * https://www.rfc-editor.org/rfc/rfc3492.html#section-6.1
     */
    template <bootstring_parameters Options = {}>
    static constexpr stl::int32_t
    adapt(stl::int32_t delta, stl::int32_t const num_points, bool const first_time) noexcept {
        delta               = first_time ? delta / Options.damp : delta / 2;
        delta              += delta / num_points;
        stl::int32_t k_val  = 0;
        while (delta > ((Options.base - Options.tmin) * Options.tmax) / 2) {
            delta /= Options.base - Options.tmin;
            k_val += Options.base;
        }
        return k_val + (((Options.base - Options.tmin + 1) * delta) / (delta + Options.skew));
    }

    /**
     * Converts a UTF-8 input into punycode.
     *
     * https://www.rfc-editor.org/info/rfc3492
     * https://www.rfc-editor.org/info/rfc5891
     */
    template <bootstring_parameters Options = {},
              istl::CharType        CharT   = char32_t,
              istl::String          OutStrT = stl::u32string>
    [[nodiscard]] constexpr bool bootstring(stl::basic_string_view<CharT> src, OutStrT &out) {}

    /// Convert to punycode
    template <istl::CharType CharT = char32_t, istl::String OutStrT = stl::u32string>
    [[nodiscard]] constexpr bool to_punycode(stl::basic_string_view<CharT> src, OutStrT &out) {
        return bootstring(src, out);
    }

} // namespace webpp::uri


#endif // WEBPP_URL_PUNY_CODES_HPP
