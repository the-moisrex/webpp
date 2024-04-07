// Created by moisrex on 3/26/24.

#ifndef WEBPP_URI_IDNA_ASCII_HPP
#define WEBPP_URI_IDNA_ASCII_HPP

namespace webpp::uri::idna {

    enum struct domain_to_ascii_status {
        success = 0,
        invalid_code_point,
        dissallowed_code_point_found
    };

    /**
     *
     * RFC: https://www.rfc-editor.org/rfc/rfc3490.html#section-4.1
     */
    static constexpr auto domain_to_ascii() {}

} // namespace webpp::uri::idna

#endif // WEBPP_URI_IDNA_ASCII_HPP
