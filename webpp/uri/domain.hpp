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

      private:
        domain_name_status status = domain_name_status::unparsed;

      public:
        basic_domain(basic_domain const&)                = default;
        basic_domain(basic_domain&&) noexcept            = default;
        basic_domain& operator=(basic_domain const&)     = default;
        basic_domain& operator=(basic_domain&&) noexcept = default;
        ~basic_domain()                                  = default;

        explicit constexpr basic_domain(stl::basic_string_view<CharT> input,
                                        domain_name_status const      inp_status) noexcept
          : stl::basic_string_view<CharT>{input},
            status{inp_status} {}

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            using enum domain_name_status;
            return status == valid || status == valid_punycode;
        }

        [[nodiscard]] constexpr bool has_punycode() const noexcept {
            return status == domain_name_status::valid_punycode;
        }

        [[nodiscard]] explicit constexpr operator bool() const noexcept {
            return is_valid();
        }

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
