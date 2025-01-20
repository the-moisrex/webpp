// Created by moisrex on 1/20/25.

#ifndef WEBPP_URI_PARSE_DOMAIN_HPP
#define WEBPP_URI_PARSE_DOMAIN_HPP

#include "../../strings/charset.hpp"
#include "../uri_status.hpp"

namespace webpp::uri {

    enum struct domain_name_status : stl::underlying_type_t<uri_status> { // NOLINT(*-enum-size)
    // NOLINTBEGIN(*-macro-usage)
#define webpp_def(status) status = stl::to_underlying(uri_status::status)
        webpp_def(unparsed),           // Not yet parsed
        webpp_def(valid),              // valid ascii domain name
        webpp_def(valid_punycode),     // valid domain name which is a punycode
        webpp_def(invalid_character),  // found an invalid character
        webpp_def(too_long),           // the domain is too long
        webpp_def(subdomain_too_long), // the subdomain is too long
        webpp_def(dot_at_end),         // the domain ended unexpectedly
        webpp_def(begin_with_hyphen),  // the domain cannot start with hyphens
        webpp_def(end_with_hyphen),    // the domain cannot end with hyphens
        webpp_def(double_hyphen),      // the domain cannot have double hyphens unless it's a punycode
        webpp_def(empty_subdomain),    // a domain/subdomain cannot be empty (no double dotting)
#undef webpp_def
        // NOLINTEND(*-macro-usage)
    };

    /**
     * Get the error message as a string view
     */
    static constexpr stl::string_view to_string(domain_name_status const status) noexcept {
        switch (status) {
            using enum domain_name_status;
            case unparsed: return {"The domain name is not parsed yet"};
            case valid: return {"Valid ascii domain name"};
            case valid_punycode: return {"Valid unicode domain name which contains punycode"};
            case invalid_character: return {"Found an invalid character in the domain name"};
            case too_long: return {"The domain is too long, max allowed character is 255"};
            case subdomain_too_long:
                return {"The subdomain is too long, max allowed character in a sub-domain is 63"};
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

        using char_type = typename stl::iterator_traits<Iter>::value_type;

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
            if (*pos == static_cast<char_type>('x') && end - pos > 4 &&
                *++pos == static_cast<char_type>('n') && *++pos == static_cast<char_type>('-') &&
                *++pos == static_cast<char_type>('-'))
            {
                has_punycode = true;
                pos = charset{ALPHA_DIGIT<char_type>, charset<char_type, 1>{static_cast<char_type>('-')}}
                        .find_first_not_in(pos, end);
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
