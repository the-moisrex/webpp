// Created by moisrex on 3/26/24.

#ifndef WEBPP_URI_IDNA_ASCII_HPP
#define WEBPP_URI_IDNA_ASCII_HPP

#include "../../std/string.hpp"
#include "../../unicode/normalization.hpp"
#include "../uri_status.hpp"

namespace webpp::uri::idna {

    enum struct domain_to_ascii_status {
        valid                     = stl::to_underlying(uri_status::valid),
        invalid_domain_code_point = stl::to_underlying(uri_status::invalid_domain_code_point),
        domain_to_ascii_error     = stl::to_underlying(uri_status::domain_to_ascii_error),
    };

    /**
     * WHATWG Spec: https://url.spec.whatwg.org/#concept-domain-to-ascii
     *         RFC: https://www.rfc-editor.org/rfc/rfc3490.html#section-4.1
     *     UTS #46: https://www.unicode.org/reports/tr46/#ToASCII
     */
    template <istl::String StrT = stl::string>
    static constexpr domain_to_ascii_status domain_to_ascii(
      StrT&                                                                         out,
      stl::basic_string_view<typename StrT::value_type, typename StrT::char_traits> src) {
        using enum domain_to_ascii_status;
    }


} // namespace webpp::uri::idna

#endif // WEBPP_URI_IDNA_ASCII_HPP
