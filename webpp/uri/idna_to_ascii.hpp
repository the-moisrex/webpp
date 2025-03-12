// Created by moisrex on 3/26/24.

#ifndef WEBPP_URI_IDNA_ASCII_HPP
#define WEBPP_URI_IDNA_ASCII_HPP

#include "../std/string.hpp"
#include "../unicode/idna.hpp"
#include "../unicode/normalization.hpp"
#include "uri_status.hpp"

namespace webpp::uri::idna {

    enum struct domain2ascii_status {
        valid                     = stl::to_underlying(uri_status::valid),
        invalid_domain_code_point = stl::to_underlying(uri_status::invalid_domain_code_point),
        domain_to_ascii_error     = stl::to_underlying(uri_status::domain_to_ascii_error),
    };

    [[nodiscard]] static constexpr bool is_valid(domain2ascii_status const status) noexcept {
        using enum domain2ascii_status;
        switch (status) {
            case valid: return true;
            default: return false;
        }
    }

    static constexpr void set_error(uri_status_type& status, domain2ascii_status const value) noexcept {
        set_error(status, static_cast<uri_status>(stl::to_underlying(value)));
    }

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
    template <uri_parsing_options Options, istl::String StrT = stl::string, typename Iter>
    static constexpr domain2ascii_status domain_to_ascii(Iter spos, Iter send, StrT& out) {
        using enum domain2ascii_status;
        using unicode::normalization_form;

        auto const status = unicode::idna::to_ascii(spos, send, out);

        if constexpr (Options.verify_dns_length) {
        }

        return status;
    }


} // namespace webpp::uri::idna

#endif // WEBPP_URI_IDNA_ASCII_HPP
