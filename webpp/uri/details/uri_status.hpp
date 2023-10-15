// Created by moisrex on 10/5/23.

#ifndef WEBPP_URI_STATUS_HPP
#define WEBPP_URI_STATUS_HPP

#include "../../std/string_view.hpp"
#include "../../std/utility.hpp"

#include <cstdint>

namespace webpp::uri {

    /// Uri status can have multiple warnings (WHATWG calls it "validation error"), but
    /// only one error is possible.
    /// This warning bit helps with:
    ///   - see if there's a warning
    ///   - which warnings we have found
    ///   - distinguish between a warning flag and an error flag or a success flag
    static constexpr stl::uint_fast32_t warning_bit = (1u << 31);
    static constexpr stl::uint_fast32_t valid_bit   = (1u << 30);
    static constexpr stl::uint_fast32_t error_bit   = 0u;

    /// sucesses are exclusive
    /// errors are exclusive,
    /// warnings are not exclusive,
    enum struct uri_status : stl::uint_fast32_t {
        // success:
        valid          = valid_bit | 1u, // valid URI
        valid_punycode = valid_bit | 2u, // valid URI which contains a punycode

        // common errors:
        invalid_character = warning_bit | (1u << 0u), // found an invalid character
        too_long          = error_bit | 1u,           // the URI is too long
        empty_string      = error_bit | 2u,           // the URI/URL/.. is empty

        // scheme-specific errors/warnings:
        valid_no_scheme                 = valid_bit | 3u, // no scheme is specified
        valid_file                      = valid_bit | 4u, // Valid scheme "file://"
        valid_path_or_authority         = valid_bit | 5u,
        valid_authority                 = valid_bit | 6u,
        valid_opaque_path               = valid_bit | 7u,
        scheme_ended_unexpectedly       = error_bit | 3u,
        incompatible_schemes            = error_bit | 4u,
        missing_following_solidus       = warning_bit | (1u << 1u), // Missing '//' after 'file:'
        missing_scheme_non_relative_url = error_bit | 5u,

        // domain-specific errors:
        subdomain_too_long = error_bit | 6u,  // the subdomain is too long
        dot_at_end         = error_bit | 7u,  // the domain ended unexpectedly
        begin_with_hyphen  = error_bit | 8u,  // the domain cannot start with hyphens
        end_with_hyphen    = error_bit | 9u,  // the domain cannot end with hyphens
        double_hyphen      = error_bit | 10u, // the domain cannot have double hyphens unless it's a punycode
        empty_subdomain    = error_bit | 11u, // a domain/subdomain cannot be empty (no double dotting)
        host_missing       = error_bit | 12u,

        // port-specific errors:
        port_out_of_range = error_bit | 13u,
        port_invalid      = error_bit | 14u, // invalid characters and what not

        // path-specific errors/warnings:
        reverse_solidus_used         = warning_bit | (1u << 2u),
        windows_drive_letter_used    = warning_bit | (1u << 3u),
        windows_drive_letter_as_host = warning_bit | (1u << 4u),
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
            case invalid_character:
                return "Found an invalid character in the URI; "
                       "more info: https://url.spec.whatwg.org/#invalid-url-unit";
            case too_long: return "The URI is too long, max allowed character is 255";
            case empty_string:
                return "The URI is empty.";

                // scheme-specific errors:
            case valid_no_scheme: return "This URL doesn't have a scheme.";
            case valid_file: return "Valid file scheme (starts with file://)";
            case valid_path_or_authority:
                return "Valid scheme that should be followed by a path or an authority.";
            case valid_authority: return "Valid scheme that should be followed by an authority.";
            case valid_opaque_path: return "Valid scheme that should be followed by an opaque path.";
            case scheme_ended_unexpectedly:
                return "This URI doesn't seem to have enough information, not even a qualified scheme.";
            case incompatible_schemes:
                return "The new URI Scheme is not compatible with the old one; can't merge them.";
            case missing_following_solidus:
                return "The URI's scheme is not followed by \"//\"; "
                       "more information: https://url.spec.whatwg.org/#special-scheme-missing-following-solidus";
            case missing_scheme_non_relative_url:
                return "The URI is missing scheme, and nothing was provided by the base URI; "
                       "more info: https://url.spec.whatwg.org/#missing-scheme-non-relative-url";

                // domain-specific errors:
            case subdomain_too_long:
                return "The subdomain is too long, max allowed character in a sub-domain is 63";
            case dot_at_end:
                return "The domain ended unexpectedly; domains cannot have a dot at the end (this is not a dns record)";
            case begin_with_hyphen: return "The domain cannot start with hyphens";
            case end_with_hyphen: return "The domain cannot end with hyphens";
            case double_hyphen: return "The domain cannot have double hyphens unless it's a punycode";
            case empty_subdomain: return "A domain/sub-domain cannot be empty (no double dotting)";
            case host_missing:
                return "Based on the URI's scheme, this URL must have a host; "
                       "more info: https://url.spec.whatwg.org/#host-missing";

                // port-specific errors:
            case port_out_of_range:
                return "Port number is too big; "
                       "more info: https://url.spec.whatwg.org/#port-out-of-range";
            case port_invalid:
                return "Invalid characters used in port; "
                       "more info: https://url.spec.whatwg.org/#port-invalid";

                // path-specific errors/warnings:
            case reverse_solidus_used:
                return "The URI is using backslash instead of a forward slash; "
                       "more info: https://url.spec.whatwg.org/#invalid-reverse-solidus";
            case windows_drive_letter_used:
                return "The URI is relative, starts with Windows Drive Letter, and the base URI's scheme is 'file:'; "
                       "more info: https://url.spec.whatwg.org/#file-invalid-windows-drive-letter";
            case windows_drive_letter_as_host:
                return "A 'file:' URI cannot have a Windows Drive Letter as a host; "
                       "more info: https://url.spec.whatwg.org/#file-invalid-windows-drive-letter-host";
        }
        stl::unreachable();
    }

    [[nodiscard]] static constexpr bool is_valid(stl::underlying_type_t<uri_status> status) noexcept {
        return valid_bit & status;
    }

    [[nodiscard]] static constexpr bool is_valid(uri_status status) noexcept {
        return is_valid(stl::to_underlying(status));
    }

    [[nodiscard]] static constexpr bool has_warnings(stl::underlying_type_t<uri_status> status) noexcept {
        return (warning_bit & status) == warning_bit;
    }

    [[nodiscard]] static constexpr bool has_warnings(uri_status status) noexcept {
        return has_warnings(stl::to_underlying(status));
    }


} // namespace webpp::uri

#endif // WEBPP_URI_STATUS_HPP
