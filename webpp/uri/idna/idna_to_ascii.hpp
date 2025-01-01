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
     * WHATWG Spec: https://url.spec.whatwg.org/#concept-domain-to-ascii
     *         RFC: https://www.rfc-editor.org/rfc/rfc3490.html#section-4.1
     *     UTS #46: https://www.unicode.org/reports/tr46/#ToASCII
     */
    template <istl::String StrT = stl::string>
    static constexpr domain_to_ascii_status domain_to_ascii(StrT& out, istl::string_view_type_of<StrT> src) {
        using enum domain_to_ascii_status;

        auto const beg_index = out.size();

        // todo: mapping simply ignores invalid code points, is that okay?
        if (!idna::map(src.begin(), src.end(), out)) {
            // todo: is this error code the correct error?
            return invalid_domain_code_point;
        }

        return valid;
    }


} // namespace webpp::uri::idna

#endif // WEBPP_URI_IDNA_ASCII_HPP
