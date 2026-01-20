// Created by moisrex on 5/12/23.

#ifndef WEBPP_URI_DOMAIN_HPP
#define WEBPP_URI_DOMAIN_HPP

#include "../std/string_like.hpp"
#include "./parser/parse_domain.hpp"

namespace webpp::uri {

    /**
     * Structured Domain Name
     */
    template <typename CharT>
    struct basic_domain : stl::basic_string_view<CharT> {
        using char_type        = CharT;
        using string_view_type = stl::basic_string_view<CharT>;

        using stl::basic_string_view<CharT>::basic_string_view; // ctor

        /// Top-Level-Domain
        [[nodiscard]] constexpr string_view_type tld() const noexcept {
            // Domain Separators can be Unicode Code Points as well, but if the transition was not successful, then
            // we'll have an error anyway.
            // https://www.unicode.org/reports/tr46/#Notation
            if (auto const pos = this->rfind('.'); pos != string_view_type::npos) {
                return this->substr(pos + 1);
            }
            return *this; // the whole thing is a TLD
        }
    };

} // namespace webpp::uri

#endif // WEBPP_URI_DOMAIN_HPP
