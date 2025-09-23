// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_DOMAIN_HPP
#define WEBPP_URI_PARSE_DOMAIN_HPP

#include "../../strings/charset.hpp"
#include "../uri_status.hpp"

namespace webpp::uri {

    enum struct domain_name_status : uri_status_type {        // NOLINT(*-enum-size)
        unparsed           = +uri_status::unparsed,           // Not yet parsed
        valid              = +uri_status::valid,              // valid ascii domain name
        valid_punycode     = +uri_status::valid_punycode,     // valid domain name which is a punycode
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
        static constexpr auto domain_name_threshold = 255;
        static constexpr auto subdomain_threshold   = 63;
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

        if (pos == end) {
            return empty_subdomain;
        }
        if (end - pos > details::domain_name_threshold) {
            return too_long;
        }

        switch (*pos) {
            case static_cast<char_type>('.'): return empty_subdomain;
            case static_cast<char_type>('-'): return begin_with_hyphen;
            default: break;
        }

        bool has_punycode    = false;
        auto subdomain_start = pos;
        while (pos != end) {
            if (*pos == static_cast<char_type>('x') && end - pos > 4 && *++pos == static_cast<char_type>('n') &&
                *++pos == static_cast<char_type>('-') && *++pos == static_cast<char_type>('-'))
            {
                has_punycode = true;
                pos =
                  charset{ALPHA_DIGIT<char_type>, charset<char_type, 1>{static_cast<char_type>('-')}}.find_first_not_in(
                    pos,
                    end);
                continue;
            }

            switch (char_type const cur_char = *pos++) {
                // todo: are we handling label-separators? https://www.unicode.org/reports/tr46/#Notation
                case static_cast<char_type>('.'):
                    if (pos == end) {
                        return dot_at_end;
                    }
                    if (*pos == static_cast<char_type>('.')) {
                        return empty_subdomain;
                    }
                    if (*pos == static_cast<char_type>('-')) {
                        return begin_with_hyphen;
                    }
                    if (pos - subdomain_start > details::subdomain_threshold) {
                        return subdomain_too_long;
                    }
                    subdomain_start = pos;
                    continue;
                case static_cast<char_type>('-'):
                    if (pos == end || *pos == static_cast<char_type>('.')) {
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
        if (end - subdomain_start > details::subdomain_threshold) {
            return subdomain_too_long;
        }
        return has_punycode ? valid_punycode : valid;
        // NOLINTEND(*-pro-bounds-pointer-arithmetic, *-inc-dec-in-conditions)
    }


} // namespace webpp::uri

#endif // WEBPP_URI_PARSE_DOMAIN_HPP
