// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_DOMAIN_HPP
#define WEBPP_URI_PARSE_DOMAIN_HPP

#include "../../strings/charset.hpp"
#include "../uri_status.hpp"

namespace webpp::uri {

    enum struct domain_name_status : uri_status_type {        // NOLINT(*-enum-size)
        unparsed           = +uri_status::unparsed,           // Not yet parsed
        valid              = +uri_status::valid,              // valid ascii domain name
        valid_punycode     = +uri_status::last_valid + 1U,    // valid domain name which is a punycode
        invalid_character  = +uri_status::invalid_character,  // found an invalid character
        too_long           = +uri_status::too_long,           // the domain is too long
        subdomain_too_long = +uri_status::subdomain_too_long, // the subdomain is too long
        dot_at_end         = +uri_status::dot_at_end,         // the domain ended unexpectedly
        begin_with_hyphen  = +uri_status::begin_with_hyphen,  // the domain cannot start with hyphens
        end_with_hyphen    = +uri_status::end_with_hyphen,    // the domain cannot end with hyphens
        double_hyphen      = +uri_status::double_hyphen, // the domain cannot have double hyphens unless it's a punycode
        empty_subdomain    = +uri_status::empty_string,  // a domain/subdomain cannot be empty (no double dotting)
    };

    /**
     * Get the error message as a string view
     */
    [[nodiscard]] static constexpr stl::string_view to_string(domain_name_status const status) noexcept {
        switch (status) {
            using enum domain_name_status;
            case unparsed: return {"The domain name is not parsed yet"};
            case valid: return {"Valid ascii domain name"};
            case valid_punycode: return {"Valid unicode domain name which contains punycode"};
            case invalid_character: return {"Found an invalid character in the domain name"};
            case too_long: return {"The domain is too long, max allowed character is 255"};
            case subdomain_too_long: return {"The subdomain is too long, max allowed character in a sub-domain is 63"};
            case dot_at_end:
                return {
                  "The domain ended unexpectedly; "
                  "domains cannot have a dot at the end (this is not a dns record)"};
            case begin_with_hyphen: return {"The domain cannot start with hyphens"};
            case end_with_hyphen: return {"The domain cannot end with hyphens"};
            case double_hyphen: return {"The domain cannot have double hyphens unless it's a punycode"};
            case empty_subdomain: return {"A domain/sub-domain cannot be empty (no double dotting)"};
        }
        stl::unreachable();
    }

    namespace details {
        static constexpr auto domain_length_limit    = 255;
        static constexpr auto subdomain_length_limit = 63;
    } // namespace details

    /**
     * Parse a domain name
     *
     * @param pos begin
     * @param end end
     * @return status of the parsing
     */
    template <typename Iter, typename EIter = Iter>
    constexpr domain_name_status parse_domain_name(Iter pos, EIter end) noexcept {
        // NOLINTBEGIN(*-pro-bounds-pointer-arithmetic, *-inc-dec-in-conditions)
        using enum domain_name_status;

        using char_type = stl::iter_value_t<Iter>;

        auto const length = stl::distance(pos, end);

        if (pos == end) [[unlikely]] {
            return empty_subdomain;
        }
        if (length > details::domain_length_limit) [[unlikely]] {
            // todo: is `too_long` the right error?
            return too_long;
        }

        switch (*pos) {
            [[unlikely]] case static_cast<char_type>('.'):
                return empty_subdomain;
            [[unlikely]] case static_cast<char_type>('-'):
                return begin_with_hyphen;
            default: break;
        }

        webpp_static_constexpr auto nonascii =
          charmap{charmap_range<'a', 'z'>(),
                  charmap_range<'A', 'Z'>(),
                  charmap_range<'0', '9'>(),
                  charmap_range<'-', '-'>()};
        bool has_punycode    = false;
        auto subdomain_start = pos;
        while (pos != end) {
            if (*pos == static_cast<char_type>('x') && end - pos > 4 && *++pos == static_cast<char_type>('n') &&
                *++pos == static_cast<char_type>('-') && *++pos == static_cast<char_type>('-'))
            {
                has_punycode = true;
                pos          = nonascii.find_first_not_in(pos, end);
                continue;
            }

            switch (char_type const cur_char = *pos++) {
                // todo: are we handling label-separators? https://www.unicode.org/reports/tr46/#Notation
                case '.':
                    if (pos == end) {
                        return dot_at_end;
                    }
                    if (*pos == '.') {
                        return empty_subdomain;
                    }
                    if (*pos == '-') {
                        return begin_with_hyphen;
                    }
                    if (pos - subdomain_start > details::subdomain_length_limit) {
                        return subdomain_too_long;
                    }
                    subdomain_start = pos;
                    continue;
                case '-':
                    if (pos == end || *pos == '.') {
                        return end_with_hyphen;
                    }
                    if (*pos == static_cast<char_type>('-')) {
                        return double_hyphen;
                    }
                    break;
                default: {
                    if (!ALPHA_DIGIT<char_type>.contains(cur_char)) {
                        --pos; // make sure the invalid character is selected
                        return invalid_character;
                    }
                }
            }
            pos = ALPHA_DIGIT<char_type>.find_first_not_in(pos, end);
        }
        // checking if the TLD is of valid length
        if (end - subdomain_start > details::subdomain_length_limit) {
            return subdomain_too_long;
        }
        return has_punycode ? valid_punycode : valid;
        // NOLINTEND(*-pro-bounds-pointer-arithmetic, *-inc-dec-in-conditions)
    }


} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_DOMAIN_HPP
