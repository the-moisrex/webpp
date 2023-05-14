// Created by moisrex on 5/12/23.

#ifndef WEBPP_URI_DOMAIN_HPP
#define WEBPP_URI_DOMAIN_HPP

#include "../std/string_view.hpp"
#include "../strings/charset.hpp"

#include <cstdint>

namespace webpp {

    enum struct domain_name_status {
        valid,              // valid ascii domain name
        valid_punycode,     // valid domain name which is a punycode
        invalid_character,  // found an invalid character
        too_long,           // the domain is too long
        subdomain_too_long, // the subdomain is too long
        dot_at_end,         // the domain ended unexpectedly
        begin_with_hyphen,  // the domain cannot start with hyphens
        end_with_hyphen,    // the domain cannot end with hyphens
        double_hyphen,      // the domain cannot have double hyphens unless it's a punycode
        empty_subdomain,    // a domain/sub-domain cannot be empty (no double dotting)
        long_subdomain,     // there's a subdomain which is longer than 63 characters
    };

    /**
     * Get the error message as a string view
     */
    static constexpr stl::string_view to_string(domain_name_status status) noexcept {
        switch (status) {
            using enum domain_name_status;
            case valid: return "Valid ascii domain name";
            case valid_punycode: return "Valid unicode domain name which contains punycode";
            case invalid_character: return "Found an invalid character in the domain name";
            case too_long: return "The domain is too long, max allowed character is 255";
            case subdomain_too_long:
                return "The subdomain is too long, max allowed character in a sub-domain is 63";
            case dot_at_end:
                return "The domain ended unexpectedly; domains cannot have a dot at the end (this is not a dns record)";
            case begin_with_hyphen: return "The domain cannot start with hyphens";
            case end_with_hyphen: return "The domain cannot end with hyphens";
            case double_hyphen: return "The domain cannot have double hyphens unless it's a punycode";
            case empty_subdomain: return "A domain/sub-domain cannot be empty (no double dotting)";
            case long_subdomain: return "There's a subdomain which is longer than 63 characters";
        }
        return ""; // just to get rid of static analyzers' warning
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
    constexpr domain_name_status parse_domain_name(const char*& pos, const char* end) noexcept {
        using enum domain_name_status;
        if (pos == end) {
            return empty_subdomain;
        } else if (end - pos > details::domain_name_threshold) {
            return too_long;
        }

        if (*pos == '.') {
            return empty_subdomain;
        } else if (*pos == '-') {
            return begin_with_hyphen;
        }

        bool has_punycode    = false;
        auto subdomain_start = pos;
        while (pos != end) {

            if (*pos == 'x' && end - pos > 4 && *++pos == 'n' && *++pos == '-' && *++pos == '-') {
                has_punycode = true;
                pos          = charset{ALPHA_DIGIT<char>, charset{'-'}}.contains_until(pos, end);
                continue;
            }

            const char ch = *pos++;

            switch (ch) {
                case '.':
                    if (pos == end) {
                        return dot_at_end;
                    } else if (*pos == '.') {
                        return empty_subdomain;
                    } else if (*pos == '-') {
                        return begin_with_hyphen;
                    } else if (pos - subdomain_start > details::subdomain_threshold) {
                        return subdomain_too_long;
                    }
                    subdomain_start = pos;
                    continue;
                case '-':
                    if (pos == end || *pos == '.') {
                        return end_with_hyphen;
                    } else if (*pos == '-') {
                        return double_hyphen;
                    }
                    break;
                default: {
                    if (!ALPHA_DIGIT<char>.contains(ch)) {
                        --pos; // make sure the invalid character is selected
                        return invalid_character;
                    }
                }
            }
            pos = ALPHA_DIGIT<char>.contains_until(pos, end);
        }
        // checking if the TLD is of valid length
        if (end - subdomain_start > details::subdomain_threshold) {
            return subdomain_too_long;
        }
        return has_punycode ? valid_punycode : valid;
    }




    /**
     * Domain Name
     */
    struct domain_name : stl::string_view {
        using stl::string_view::basic_string_view;

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return !empty();
        }

        [[nodiscard]] constexpr bool has_punycode() const noexcept {
            return starts_with("xn--");
        }

        [[nodiscard]] constexpr operator bool() const noexcept {
            return is_valid();
        }
    };

} // namespace webpp

#endif // WEBPP_URI_DOMAIN_HPP
