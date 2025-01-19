// Created by moisrex on 3/26/24.

#ifndef WEBPP_URI_IDNA_ASCII_HPP
#define WEBPP_URI_IDNA_ASCII_HPP

#include "../../std/string.hpp"
#include "../../unicode/normalization.hpp"
#include "../uri_status.hpp"
#include "./idna_mappings.hpp"

namespace webpp::uri::idna {

    enum struct domain_to_ascii_status {
        valid                     = stl::to_underlying(uri_status::valid),
        invalid_domain_code_point = stl::to_underlying(uri_status::invalid_domain_code_point),
        domain_to_ascii_error     = stl::to_underlying(uri_status::domain_to_ascii_error),
    };

    /**
     * The ToASCII operation takes a sequence of Unicode code points that
     * make up one label and transforms it into a sequence of code points in
     * the ASCII range (0..7F).  If ToASCII succeeds, the original sequence
     * and the resulting sequence are equivalent labels.
     *  - from RFC 3490
     *
     * WHATWG Spec: https://url.spec.whatwg.org/#concept-domain-to-ascii
     *         RFC: https://www.rfc-editor.org/rfc/rfc3490.html#section-4.1
     *     UTS #46: https://www.unicode.org/reports/tr46/#ToASCII
     *  Steps From: https://www.unicode.org/reports/tr46/#Processing
     */
    template <uri_parsing_options Options, istl::String StrT = stl::string>
    static constexpr domain_to_ascii_status domain_to_ascii(istl::string_view_type_of<StrT> src, StrT& out) {
        using enum domain_to_ascii_status;
        using unicode::normalization_form;

        auto const beg_index = out.size();

        // 1. Map
        if (!idna::map(src.begin(), src.end(), out)) {
            // todo: is this error code the correct error?
            return invalid_domain_code_point;
        }

        // 2. Normalize
        unicode::normalize<normalization_form::NFC>(out);

        // 3. Break: Break the string into labels at U+002E (.) FULL STOP

        // 4. Convert/Validate
        if constexpr (Options.parse_punycodes) {
            // todo
            if constexpr (Options.ignore_invalid_punycode) {
            }
        }

        return valid;
    }


} // namespace webpp::uri::idna

#endif // WEBPP_URI_IDNA_ASCII_HPP
