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
    ///
    /// indexes: [ 0 1 2 3  4 5 6 7    8 9 A B  C D E F ] == 16bits
    /// integer: [ B W W W  W W W W    N N N N  N N N N ]
    ///            ^ --------------    ----------------
    ///            |       ^                 ^
    ///            |       |                 |
    ///            |       |         valid/error number
    ///            |       |
    ///            |       ----> Each warning bit
    ///            |
    ///            |
    ///         error bit == 1
    ///         valid bit == 0
    using uri_status_type                        = stl::uint_fast16_t;
    static constexpr uri_status_type valid_bit   = 0u;
    static constexpr uri_status_type error_bit   = (1u << 15);
    static constexpr uri_status_type warning_bit = error_bit >> 1u;

    /// maximum number between errors and valids must go here,
    /// or you can find out (error_bit | warning_bit) + all the warning bits, and then negate that
    /// considering the IPv4 and IPv6 values that need to match special prefix values, we're going with
    /// all 8 bits even though it's possible to do it with even 4 bits.
    static constexpr uri_status_type values_mask = 0b0000'0000'1111'1111 | error_bit | valid_bit;

    /// warnings mask
    static constexpr uri_status_type warnings_mask = ~values_mask; // Warnings' bits

    /// successes are exclusive
    /// errors are exclusive,
    /// warnings are not exclusive,
    enum struct uri_status : uri_status_type {
        unparsed = 0, // not parsed at all

        // success:
        valid          = valid_bit | 1u, // valid URI
        valid_punycode = valid_bit | 2u, // valid URI which contains a punycode

        // common errors:
        invalid_character = warning_bit >> 0u, // found an invalid character
        too_long          = error_bit | 1u,    // the URI is too long
        empty_string      = error_bit | 2u,    // the URI/URL/.. is empty

        // scheme-specific errors/warnings:
        scheme_ended_unexpectedly       = error_bit | 3u,
        incompatible_schemes            = error_bit | 4u,
        missing_following_solidus       = warning_bit >> 1u, // Missing '//' after 'file:'
        missing_scheme_non_relative_url = error_bit | 5u,

        // host-specific errors:
        valid_path_or_authority = valid_bit | 3u,
        valid_authority         = valid_bit | 4u,
        valid_host              = valid_bit | 5u,
        valid_port              = valid_bit | 6u,
        valid_authority_end     = valid_bit | 7u,
        subdomain_too_long      = error_bit | 6u, // the subdomain is too long
        dot_at_end              = error_bit | 7u, // the domain ended unexpectedly
        begin_with_hyphen       = error_bit | 8u, // the domain cannot start with hyphens
        end_with_hyphen         = error_bit | 9u, // the domain cannot end with hyphens
        double_hyphen   = error_bit | 10u, // the domain cannot have double hyphens unless it's a punycode
        empty_subdomain = error_bit | 11u, // a domain/subdomain cannot be empty (no double dotting)
        host_missing    = error_bit | 12u,
        invalid_host_code_point   = error_bit | 13u, // non-special (opaque) host contains invalid character
        invalid_domain_code_point = error_bit | 14u, // domain name contains invalid chars

        // ipv4-specific errors and warnings:
        // ipv4_empty_octet = warning_bit | stl::to_underlying(inet_pton4_status::empty_octet),

        // port-specific errors:
        port_out_of_range = error_bit | 15u,
        port_invalid      = error_bit | 16u, // invalid characters and what not

        // path-specific errors/warnings:
        valid_path                   = valid_bit | 8u,
        valid_opaque_path            = valid_bit | 9u,
        reverse_solidus_used         = warning_bit >> 2u,
        windows_drive_letter_used    = warning_bit >> 3u,
        windows_drive_letter_as_host = warning_bit >> 4u,

        // queries-specific errors/warnings:
        valid_queries = valid_bit | 10u,

        // fragment-specific errors/warnings:
        valid_fragment = valid_bit | 11u,
    };

    /**
     * Get the error message as a string view
     */
    static constexpr stl::string_view to_string(uri_status status) noexcept {
        switch (status) {
            using enum uri_status;

            case unparsed: return {"The URI is not parsed."};

            // success:
            case valid: return {"Valid URI"};
            case valid_punycode:
                return {"Valid URI with unicode domain name which contains punycode"};

                // common errors:
            case invalid_character:
                return {"Found an invalid character in the URI; "
                        "more info: https://url.spec.whatwg.org/#invalid-url-unit"};
            case too_long: return {"The URI is too long, max allowed character is 255"};
            case empty_string:
                return {"The URI is empty."};

                // scheme-specific errors:
            case valid_path_or_authority:
                return {"Valid scheme that should be followed by a path or an authority."};
            case valid_authority: return {"Valid scheme that should be followed by an authority."};
            case valid_host: return {"Valid URI until host; parsing is not done yet."};
            case valid_port: return {"Valid URI until port, there's a port but parsing is not done yet."};
            case valid_authority_end:
                return {"Valid URI until the authority ends "
                        "(meaning the next part of the URI should be a path, query, "
                        "fragment, or a combination of them); "
                        "parsing is not done yet."};
            case scheme_ended_unexpectedly:
                return {"This URI doesn't seem to have enough information, not even a qualified scheme."};
            case incompatible_schemes:
                return {"The new URI Scheme is not compatible with the old one; can't merge them."};
            case missing_following_solidus:
                return {
                  "The URI's scheme is not followed by \"//\"; "
                  "more information: https://url.spec.whatwg.org/#special-scheme-missing-following-solidus"};
            case missing_scheme_non_relative_url:
                return {"The URI is missing scheme, and nothing was provided by the base URI; "
                        "more info: https://url.spec.whatwg.org/#missing-scheme-non-relative-url"};

                // domain-specific errors:
            case subdomain_too_long:
                return {"The subdomain is too long, max allowed character in a sub-domain is 63"};
            case dot_at_end:
                return {"The domain ended unexpectedly; "
                        "domains cannot have a dot at the end (this is not a dns record)"};
            case begin_with_hyphen: return {"The domain cannot start with hyphens"};
            case end_with_hyphen: return {"The domain cannot end with hyphens"};
            case double_hyphen: return {"The domain cannot have double hyphens unless it's a punycode"};
            case empty_subdomain: return {"A domain/sub-domain cannot be empty (no double dotting)"};
            case host_missing:
                return {"Based on the URI's scheme, this URL must have a host; "
                        "more info: https://url.spec.whatwg.org/#host-missing"};
            case invalid_host_code_point:
                return {"Invalid character found in the "
                        "opaque host (opaque means that URI is not ftp, http, https, ws, or wss); "
                        "more info: https://url.spec.whatwg.org/#host-invalid-code-point"};
            case invalid_domain_code_point:
                return {"Invalid character found in the domain name "
                        "(domain name is not host; "
                        "'domain' is only applied to ftp, http, https, ws, or wss protocols); "
                        "more info: https://url.spec.whatwg.org/#domain-invalid-code-point"};

                // port-specific errors:
            case port_out_of_range:
                return {"Port number is too big; "
                        "more info: https://url.spec.whatwg.org/#port-out-of-range"};
            case port_invalid:
                return {"Invalid characters used in port; "
                        "more info: https://url.spec.whatwg.org/#port-invalid"};

                // path-specific errors/warnings:
            case valid_path: return {"Valid URI until path; parsing is not done yet."};
            case valid_opaque_path: return {"Valid URI until opaque path; parsing is not done yet."};
            case reverse_solidus_used:
                return {"The URI is using backslash instead of a forward slash; "
                        "more info: https://url.spec.whatwg.org/#invalid-reverse-solidus"};
            case windows_drive_letter_used:
                return {"The URI is relative, starts with Windows Drive Letter, "
                        "and the base URI's scheme is 'file:'; "
                        "more info: https://url.spec.whatwg.org/#file-invalid-windows-drive-letter"};
            case windows_drive_letter_as_host:
                return {"A 'file:' URI cannot have a Windows Drive Letter as a host; "
                        "more info: https://url.spec.whatwg.org/#file-invalid-windows-drive-letter-host"};

            // queries-specific errors/warnings:
            case valid_queries:
                return {"Valid URI until queries, parsing is not done yet."};

                // fragment-specific errors/warnings:
            case valid_fragment: return {"Valid URI until fragment, parsing is not done yet."};
        }
        stl::unreachable();
    }



    [[nodiscard]] static constexpr bool is_valid(stl::underlying_type_t<uri_status> status) noexcept {
        return (error_bit & status) == 0;
    }

    [[nodiscard]] static constexpr bool is_valid(uri_status status) noexcept {
        return is_valid(stl::to_underlying(status));
    }

    [[nodiscard]] static constexpr bool has_warnings(stl::underlying_type_t<uri_status> status) noexcept {
        return (status & warnings_mask) != 0;
    }

    [[nodiscard]] static constexpr bool has_warnings(uri_status status) noexcept {
        return has_warnings(stl::to_underlying(status));
    }

    [[nodiscard]] static constexpr bool has_error(stl::underlying_type_t<uri_status> status) noexcept {
        return (error_bit & status) == error_bit;
    }

    [[nodiscard]] static constexpr bool has_error(uri_status status) noexcept {
        return has_error(stl::to_underlying(status));
    }

    /// get the error/valid value without the warnings if available
    [[nodiscard]] static constexpr stl::underlying_type_t<uri_status>
    get_value(stl::underlying_type_t<uri_status> status) noexcept {
        return status & values_mask;
    }

    [[nodiscard]] static constexpr uri_status get_value(uri_status status) noexcept {
        return static_cast<uri_status>(get_value(stl::to_underlying(status)));
    }

    [[nodiscard]] static constexpr uri_status get_warning(uri_status_type status,
                                                          uri_status_type index = 0) noexcept {
        return static_cast<uri_status>((warning_bit >> index) & status);
    }

    [[nodiscard]] static constexpr uri_status get_warning(uri_status      status,
                                                          uri_status_type index = 0) noexcept {
        return get_warning(stl::to_underlying(status), index);
    }


    /**
     * This is a forward iterator that gives you all the warnings inside a uri status.
     * You can use this to print the warnings and errors of a URI.
     */
    struct uri_status_iterator {
        using value_type = stl::underlying_type_t<uri_status>;

        using difference_type   = stl::ptrdiff_t;
        using reference         = value_type&;
        using const_reference   = value_type const&;
        using pointer           = value_type*;
        using const_pointer     = value_type const*;
        using iterator_category = stl::forward_iterator_tag;
        using iterator_concept  = stl::forward_iterator_tag;


        constexpr uri_status_iterator() noexcept = default;
        constexpr uri_status_iterator(uri_status inp_status) noexcept
          : status{stl::to_underlying(inp_status)} {}
        constexpr uri_status_iterator(value_type inp_status) noexcept : status{inp_status} {}
        constexpr uri_status_iterator(uri_status_iterator const&) noexcept            = default;
        constexpr uri_status_iterator(uri_status_iterator&&) noexcept                 = default;
        constexpr uri_status_iterator& operator=(uri_status_iterator const&) noexcept = default;
        constexpr uri_status_iterator& operator=(uri_status_iterator&&) noexcept      = default;
        constexpr ~uri_status_iterator() noexcept                                     = default;

        constexpr uri_status_iterator& operator++() noexcept {
            if (has_warnings(status)) {
                // remove the first warning
                auto const new_warnings = ((warnings_mask & status) << 1u) & warnings_mask;

                // remove the warnings
                status &= ~warnings_mask;

                // add the new warning bits again
                status |= new_warnings;
            } else {
                // the error or valid value will be the last element, and there only can be one of them, so
                // after that we zero it out
                status = 0u;
            }
            return *this;
        }

        [[nodiscard]] constexpr uri_status_iterator operator++(int) const noexcept {
            uri_status_iterator iter{*this};
            ++iter;
            return iter;
        }

        constexpr uri_status operator*() const noexcept {
            if (has_warnings(status)) {
                // return the first warning
                return get_warning(status);
            }
            // return valid, error, or 0; it doesn't matter because there's only one of them now.
            return static_cast<uri_status>(status);
        }

        constexpr const_pointer operator->() const noexcept {
            return &status;
        }

        constexpr pointer operator->() noexcept {
            return &status;
        }

        [[nodiscard]] constexpr bool operator==(uri_status_iterator rhs) const noexcept {
            return status == rhs.status;
        }

        [[nodiscard]] constexpr bool operator!=(uri_status_iterator rhs) const noexcept {
            return status != rhs.status;
        }

        [[nodiscard]] constexpr auto operator<=>(uri_status_iterator rhs) const noexcept {
            return status <=> rhs.status;
        }

      private:
        value_type status = 0u;
    };

    [[nodiscard]] static constexpr uri_status_iterator begin(uri_status_iterator status) noexcept {
        return {status};
    }

    [[nodiscard]] static constexpr uri_status_iterator end(uri_status_iterator) noexcept {
        return {};
    }

    [[nodiscard]] static constexpr uri_status_iterator begin(uri_status status) noexcept {
        return {status};
    }

    [[nodiscard]] static constexpr uri_status_iterator end(uri_status) noexcept {
        return {};
    }
} // namespace webpp::uri


#endif // WEBPP_URI_STATUS_HPP
