// Created by moisrex on 5/12/23.

#ifndef WEBPP_URI_DOMAIN_HPP
#define WEBPP_URI_DOMAIN_HPP

#include "../std/string_view.hpp"
#include "../strings/charset.hpp"

#include <cstdint>

namespace webpp {

    enum struct domain_name_status {
        valid,             // valid ascii domain name
        valid_punycode,    // valid domain name which is a punycode
        invalid_character, // found an invalid character
        too_long,          // the domain is too long
        dot_at_end,        // the domain ended unexpectedly
        begin_with_hyphen, // the domain cannot start with hyphens
        end_with_hyphen,   // the domain cannot end with hyphens
        double_hyphen,     // the domain cannot have double hyphens unless it's a punycode
        empty_subdomain,   // a domain/sub-domain cannot be empty (no double dotting)
        long_subdomain,    // there's a subdomain which is longer than 63 characters
    };

    namespace details {
        static constexpr auto domain_name_threshold = 255;
    }


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

        while (pos != end) {
            const char ch = *pos++;

            switch (ch) {
                case '.':
                    if (pos == end) {
                        return dot_at_end;
                    } else if (*pos == '.') {
                        return empty_subdomain;
                    } else if (*pos == '-') {
                        return begin_with_hyphen;
                    }
                    break;
                case '-':
                    if (pos == end || *pos == '.') {
                        return end_with_hyphen;
                    } else if (*pos == '-') { // todo
                        return double_hyphen;
                    }
                    break;
            }

            pos = ALPHA_DIGIT<char>.contains_until(pos, end);
        }

        return valid;
    }




    struct domain_name : stl::string_view {
        constexpr domain_name(const char* start, const char* end) noexcept {
            parse_domain_name(start, end);
        }

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
