// Created by moisrex on 10/5/23.

#ifndef WEBPP_URI_STATUS_HPP
#define WEBPP_URI_STATUS_HPP

#include "../../std/string_view.hpp"
#include "../../std/utility.hpp"

namespace webpp::uri {

    enum struct uri_status {
        // success:
        valid,          // valid URI
        valid_punycode, // valid URI which contains a punycode

        // common errors:
        invalid_character, // found an invalid character
        too_long,          // the URI is too long
        empty_string,      // the URI/URL/.. is empty

        // scheme-specific errors:
        scheme_ended_unexpectedly,

        // domain-specific errors:
        subdomain_too_long, // the subdomain is too long
        dot_at_end,         // the domain ended unexpectedly
        begin_with_hyphen,  // the domain cannot start with hyphens
        end_with_hyphen,    // the domain cannot end with hyphens
        double_hyphen,      // the domain cannot have double hyphens unless it's a punycode
        empty_subdomain,    // a domain/subdomain cannot be empty (no double dotting)

        last // to identify the last value of the uri_status, from this number forward, the values are used
             // during parsing, and parsing is not finished yet.
    };

    /**
     * Get the error message as a string view
     */
    static constexpr stl::string_view to_string(uri_status status) noexcept {
        switch (status) {
            using enum uri_status;

            // success:
            case valid: return "Valid URI";
            case valid_punycode:
                return "Valid URI with unicode domain name which contains punycode";

                // common errors:
            case invalid_character: return "Found an invalid character in the URI";
            case too_long: return "The URI is too long, max allowed character is 255";
            case empty_string:
                return "The URI is empty.";

                // scheme-specific errors:
            case scheme_ended_unexpectedly:
                return "This URL doesn't seem to have enough information, not even a qualified scheme.";

                // domain-specific errors:
            case subdomain_too_long:
                return "The subdomain is too long, max allowed character in a sub-domain is 63";
            case dot_at_end:
                return "The domain ended unexpectedly; domains cannot have a dot at the end (this is not a dns record)";
            case begin_with_hyphen: return "The domain cannot start with hyphens";
            case end_with_hyphen: return "The domain cannot end with hyphens";
            case double_hyphen: return "The domain cannot have double hyphens unless it's a punycode";
            case empty_subdomain: return "A domain/sub-domain cannot be empty (no double dotting)";
            case last: return "Invalid state of URL parsing";
        }
        stl::unreachable();
    }

    static constexpr bool is_valid(uri_status status) noexcept {
        using enum uri_status;
        return status == valid || status == valid_punycode;
    }

} // namespace webpp::uri

#endif // WEBPP_URI_STATUS_HPP
