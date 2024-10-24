// Created by moisrex on 10/5/23.

#ifndef WEBPP_URI_STATUS_HPP
#define WEBPP_URI_STATUS_HPP

#include "../ip/ip.hpp"
#include "../std/string_view.hpp"
#include "../std/utility.hpp"

#include <bit>
#include <cstdint>

namespace webpp::uri {

    // NOLINTBEGIN(*-magic-numbers)


    /// URI Parsing Options,
    /// These options are designed to
    /// Default values are WHATWG-Compliant values (if relevant)
    static constexpr struct alignas(32) uri_parsing_options {
        /// Consider `\0` (EOF) as a valid end of string character; you may want to disable it if you already
        /// know the end of your string and you may enable if you're working with a stream
        bool eof_is_valid = true;

        /// Parse username and password part of the authority (you may want to disable it if you're trying to
        /// parse Host Authority which doesn't have credentials)
        bool parse_credentails = true;

        /// Empty Host is an error (file:// URIs can have empty hosts)
        bool empty_host_is_error = true;

        /// Parse punny codes
        bool parse_punycodes = true;

        /// Parse port (file:// scheme doesn't have a port, this is used there)
        bool parse_port = true;

        /// Parse queries
        bool parse_queries = true;

        /// Parse fragment
        bool parse_fragment = true;

        /// Trailing empty octets are a warning in WHATWG, not an error
        /// This option only allows for one empty dot at the end, not multiple of them
        bool allow_trailing_empty_ipv4_octet = true;

        /// Only one single IPv4 empty octet are allowed in WHATWG; it is a warning to have ANY dots at the
        /// end, but if it's an ERROR to have multiple dots at the end of IPv4s of hosts in WHATWG specs.
        ///
        ///                   https://127.0.0.1/      ==> OK
        /// Example if false: https://127.0.0.1./     ==> Warning
        /// Example if false: https://127.0.0.1..../  ==> Error
        /// Example if true:  https://127.0.0.1./     ==> Warning
        /// Example if true:  https://127.0.0.1..../  ==> Warning
        bool allow_multiple_trailing_empty_ipv4_octets = false;

        /// Hexadecimals and Octals octets are allowed to be used in IPv4 addresses (not in IPv6 though)
        bool allow_ipv4_hex_octets   = true;
        bool allow_ipv4_octal_octets = true;

        /// Invalid characters (except in domains and schems and what not) are considered a warning, not an
        /// error in WHATWG
        bool allow_invalid_characters = true;

        /// file:// scheme can have hosts in WHATWG, even though not everyone using URL parser would need such
        /// a feature, so you can disable it now
        bool allow_file_hosts = true;


        /// Windows drive letters "C:\" or "file://C|" are allowed in WHATWG; not everybody needs them, so you
        /// can disable handling it here (this doesn't mean you'd get an error if used)
        bool handle_windows_drive_letters = true;


        /// For some reason, WHATWG just simply removes tabs and newlines (TAB, LF, and CR) characters before
        /// it starts the parsing process; I don't think everybody needs such a feature and it may even cause
        /// problems for some people, so here's an option to not ignore the newliens and tabs.
        bool ignore_tabs_or_newlines = true;


        /// Dots in paths are: `.`, `..`, `%2e`, '.%2e`, '%2e.', and '%2e%2e'
        /// By disabling this, these will be segments of their own and single dot won't mean
        /// current directory and double dot won't mean anything either.
        bool handle_dots_in_paths = true;

        /// UseSTD3ASCIIRules from:
        ///   - RFC 3490: https://www.rfc-editor.org/info/rfc3490
        ///   - UTS #46:  https://www.unicode.org/reports/tr46/#STD3_Rules
        ///
        /// If enabled (UseSTD3ASCIIRules=True):
        ///   - Domain name labels are required to follow the STD3 ASCII rules, which means they must consist
        ///     only of ASCII letters, digits, and hyphens ("-").
        ///   - Labels cannot start or end with a hyphen ("-").
        ///   - Labels cannot be entirely numeric.
        ///   - Labels cannot contain any characters other than ASCII letters, digits, and hyphens ("-").
        ///   - Domain names that violate these rules will fail validation and may be rejected.
        bool use_std3_ascii_rules = false;

        /// The length of the domain name, excluding the root label and its dot, is from 1 to 253, but
        /// we don't verify that in WHATWG because it's a DNS limitation, not a URI limitation.
        bool verify_dns_length = false;
    } standard_uri_parsing_options{};

    static constexpr uri_parsing_options strict_uri_parsing_options{
      .eof_is_valid                              = false,
      .parse_credentails                         = true,
      .empty_host_is_error                       = true,
      .parse_punycodes                           = true,
      .parse_port                                = true,
      .parse_queries                             = true,
      .parse_fragment                            = true,
      .allow_trailing_empty_ipv4_octet           = false,
      .allow_multiple_trailing_empty_ipv4_octets = false,
      .allow_ipv4_hex_octets                     = false,
      .allow_ipv4_octal_octets                   = false,
      .allow_invalid_characters                  = false,
      .allow_file_hosts                          = false,
      .handle_windows_drive_letters              = false,
      .ignore_tabs_or_newlines                   = false,
      .handle_dots_in_paths                      = true,
      .use_std3_ascii_rules                      = false,
      .verify_dns_length                         = true,
    };

    static constexpr uri_parsing_options loose_uri_parsing_options{
      .eof_is_valid                              = true,
      .parse_credentails                         = true,
      .empty_host_is_error                       = true,
      .parse_punycodes                           = true,
      .parse_port                                = true,
      .parse_queries                             = true,
      .parse_fragment                            = true,
      .allow_trailing_empty_ipv4_octet           = true,
      .allow_multiple_trailing_empty_ipv4_octets = true,
      .allow_ipv4_hex_octets                     = true,
      .allow_ipv4_octal_octets                   = true,
      .allow_invalid_characters                  = true,
      .allow_file_hosts                          = true,
      .handle_windows_drive_letters              = true,
      .ignore_tabs_or_newlines                   = true,
      .handle_dots_in_paths                      = true, // for security reasons, it's enabled in loose too
      .use_std3_ascii_rules                      = false,
      .verify_dns_length                         = false,
    };

    /// Uri status can have multiple warnings (WHATWG calls it "validation error"), but
    /// only one error is possible.
    /// This warning bit helps with:
    ///   - see if there's a warning
    ///   - which warnings we have found
    ///   - distinguish between a warning flag and an error flag or a success flag
    ///
    /// indexes: [       B A 9 8    7 6 5 4  3 2 1 0    7 6 5 4  3 2 1 0 ] == 21bits
    /// integer: [ E     W W W W    W W W W  W W W W    N N N N  N N N N ]
    ///            ^     -------    ----------------    ----------------
    ///            |        ^               ^                 ^
    ///            |        |               |                 |
    ///            |        |               |         valid/error number
    ///            |        |               |
    ///            |        --------------------> Each warning bit
    ///            |
    ///            |
    ///         error bit == 1
    ///         valid bit == 0
    using uri_status_type                        = stl::uint32_t;
    static constexpr uri_status_type valid_bit   = 0U;
    static constexpr uri_status_type error_bit   = 1U << 20U;
    static constexpr uri_status_type warning_bit = error_bit >> 1U;

    /// maximum number between errors and valids must go here,
    /// or you can find out (error_bit | warning_bit) + all the warning bits, and then negate that
    /// considering the IPv4 and IPv6 values that need to match special prefix values, we're going with
    /// all 8 bits even though it's possible to do it with even 4 bits.
    static constexpr uri_status_type values_mask = 0b0000'0000'1111'1111U | error_bit | valid_bit;

    /// warnings mask
    static constexpr uri_status_type warnings_mask = ~values_mask; // Warnings' bits

    /// successes are exclusive
    /// errors are exclusive,
    /// warnings are not exclusive,
    enum struct uri_status : uri_status_type { // NOLINT(*-enum-size)
        unparsed = 0,                          // not parsed at all

        // success:
        valid          = valid_bit | 1U, // valid URI
        valid_punycode = valid_bit | 2U, // valid URI which contains a punycode

        // common errors:
        invalid_character = warning_bit >> 0U, // found an invalid character
        too_long          = error_bit | 1U,    // the URI is too long
        empty_string      = error_bit | 2U,    // the URI/URL/.. is empty

        // scheme-specific errors/warnings:
        invalid_scheme_character        = error_bit | 3U,
        scheme_ended_unexpectedly       = error_bit | 4U,
        incompatible_schemes            = error_bit | 5U,
        missing_following_solidus       = warning_bit >> 1U, // Missing '//' after 'file:'
        missing_scheme_non_relative_url = error_bit | 6U,

        // host-specific errors:
        valid_path_or_authority = valid_bit | 3U,
        valid_authority         = valid_bit | 4U,
        valid_file_host         = valid_bit | 5U,
        valid_port              = valid_bit | 6U,
        valid_authority_end     = valid_bit | 7U,
        subdomain_too_long      = error_bit | 7U,  // the subdomain is too long
        dot_at_end              = error_bit | 8U,  // the domain ended unexpectedly
        begin_with_hyphen       = error_bit | 9U,  // the domain cannot start with hyphens
        end_with_hyphen         = error_bit | 10U, // the domain cannot end with hyphens
        double_hyphen   = error_bit | 11U, // the domain cannot have double hyphens unless it's a punycode
        empty_subdomain = error_bit | 12U, // a domain/subdomain cannot be empty (no double dotting)
        host_missing    = error_bit | 13U,
        invalid_host_code_point   = error_bit | 14U, // non-special (opaque) host contains invalid character
        invalid_domain_code_point = error_bit | 15U, // domain name contains invalid chars
        has_credentials           = warning_bit >> 2U,

        // ipv4-specific errors and warnings:
        ipv4_trailing_empty_octet = warning_bit >> 3U,
        ipv4_non_decimal_octet    = warning_bit >> 4U,

        // ipv4 and ipv6 errors:
        ip_too_little_octets    = error_bit | stl::to_underlying(ip_address_status::too_little_octets),
        ip_too_many_octets      = error_bit | stl::to_underlying(ip_address_status::too_many_octets),
        ip_invalid_octet_range  = error_bit | stl::to_underlying(ip_address_status::invalid_octet_range),
        ip_invalid_leading_zero = error_bit | stl::to_underlying(ip_address_status::invalid_leading_zero),
        ip_bad_ending           = error_bit | stl::to_underlying(ip_address_status::bad_ending),
        ip_invalid_character    = error_bit | stl::to_underlying(ip_address_status::invalid_character),
        ip_invalid_colon_usage  = error_bit | stl::to_underlying(ip_address_status::invalid_colon_usage),

        // ipv6-specific errors and warnings:
        ipv6_unclosed           = error_bit | 16U,
        ipv6_char_after_closing = error_bit | 17U,

        // port-specific errors:
        port_out_of_range = error_bit | 18U,
        port_invalid      = error_bit | 19U, // invalid characters and what not

        // path-specific errors/warnings:
        valid_path                           = valid_bit | 8U,
        valid_opaque_path                    = valid_bit | 9U,
        reverse_solidus_used                 = warning_bit >> 5U,
        windows_drive_letter_used            = warning_bit >> 6U,
        windows_drive_letter_in_ralative_url = warning_bit >> 7U,
        windows_drive_letter_as_host         = warning_bit >> 8U,

        // queries-specific errors/warnings:
        valid_queries             = valid_bit | 10U,
        invalid_queries_character = error_bit | 20U,

        // fragment-specific errors/warnings:
        valid_fragment = valid_bit | 11U,
    };

    /**
     * Get the error message as a string view
     */
    static constexpr stl::string_view to_string(uri_status const status) noexcept {
        switch (status) {
            using enum uri_status;

            case unparsed: return {"The URI is not parsed."};

            // success:
            case valid: return {"Valid URI"};
            case valid_punycode:
                return {"Valid URI with unicode domain name which contains punycode"};


                // common errors:
            case invalid_character:
                return {
                  "Found an invalid character in the URI; "
                  "more info: https://url.spec.whatwg.org/#invalid-url-unit"};
            case too_long: return {"The URI is too long, max allowed character is 255"};
            case empty_string:
                return {"The URI is empty."};


                // authority-specific errors:
            case valid_path_or_authority:
                return {"Valid scheme that should be followed by a path or an authority."};
            case valid_authority:
                return {
                  "Valid scheme that should be followed by an authority "
                  "(a host optionally with username and password or port)."};
            case valid_file_host:
                return {"Valid URI until host, scheme is 'file:'; parsing is not done yet."};
            case valid_port: return {"Valid URI until port, there's a port but parsing is not done yet."};
            case valid_authority_end:
                return {
                  "Valid URI until the authority ends "
                  "(meaning the next part of the URI should be a path, query, "
                  "fragment, or a combination of them); "
                  "parsing is not done yet."};

                // scheme-specific errors:
            case invalid_scheme_character: return {"Invlaid character found in the scheme of the URI."};
            case scheme_ended_unexpectedly:
                return {"This URI doesn't seem to have enough information, not even a qualified scheme."};
            case incompatible_schemes:
                return {"The new URI Scheme is not compatible with the old one; can't merge them."};
            case missing_following_solidus:
                return {
                  "The URI's scheme is not followed by \"//\"; "
                  "more information: https://url.spec.whatwg.org/#special-scheme-missing-following-solidus"};
            case missing_scheme_non_relative_url:
                return {
                  "The URI is missing scheme, and nothing was provided by the base URI; "
                  "more info: https://url.spec.whatwg.org/#missing-scheme-non-relative-url"};


                // domain-specific errors:
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
            case host_missing:
                return {
                  "Based on the URI's scheme, this URL must have a host; "
                  "more info: https://url.spec.whatwg.org/#host-missing"};
            case invalid_host_code_point:
                return {
                  "Invalid character found in the "
                  "opaque host (opaque means that URI is not ftp, http, https, ws, or wss); "
                  "more info: https://url.spec.whatwg.org/#host-invalid-code-point"};
            case invalid_domain_code_point:
                return {
                  "Invalid character found in the domain name "
                  "(domain name is not host; "
                  "'domain' is only applied to ftp, http, https, ws, or wss protocols); "
                  "more info: https://url.spec.whatwg.org/#domain-invalid-code-point"};
            case has_credentials:
                return {
                  "The input has credentials (username or password), it is a deprecated feature of URIs; "
                  "more info: https://url.spec.whatwg.org/#invalid-credentials "
                  "and https://httpwg.org/specs/rfc9110.html#http.userinfo"};

                // ipv4 specific warnings/errors:

            case ipv4_trailing_empty_octet:
                return {
                  "IPv4 in the host has empty octet(s); "
                  "more info: https://url.spec.whatwg.org/#ipv4-empty-part"};
            case ipv4_non_decimal_octet:
                return {
                  "The input has non-decimal IPv4 in it's host;"
                  " more info: https://url.spec.whatwg.org/#ipv4-non-decimal-part"};

                // ipv4 and ipv6 errors:
            case ip_too_little_octets:
                return {
                  "Host's IP address doesn't have enough octets "
                  "(4 octets for IPv4, and 8 parts for uncompressed IPv6); "
                  "more info at https://url.spec.whatwg.org/#ipv6-too-few-pieces"};
            case ip_too_many_octets:
                return {
                  "Host's IP address has too many octets "
                  "(4 octets for IPv4, and 8 parts for uncompressed IPv6); "
                  "more info at https://url.spec.whatwg.org/#ipv6-too-many-pieces and "
                  "https://url.spec.whatwg.org/#ipv6-too-many-pieces and "
                  "https://url.spec.whatwg.org/#ipv4-in-ipv6-too-many-pieces and"
                  "https://url.spec.whatwg.org/#ipv4-too-many-parts"};
            case ip_invalid_octet_range:
                return {
                  "Out of range IP address octet found; "
                  "more info at https://url.spec.whatwg.org/#ipv4-out-of-range-part and "
                  "https://url.spec.whatwg.org/#ipv4-in-ipv6-out-of-range-part"};
            case ip_invalid_leading_zero: return {"Host's IPv4's octet starts with a leading zero"};
            case ip_bad_ending: return {"Host's IP address ended unexpectedly."};
            case ip_invalid_character: return {"Host's IP address has an invalid character"};
            case ip_invalid_colon_usage:
                return {"The host's IPv6 address is using a colon where it shouldn't"};


                // ipv6-specific errors:
            case ipv6_unclosed:
                return {
                  "IPv6 is missing the closing character ']'; "
                  "more info: https://url.spec.whatwg.org/#ipv6-unclosed"};
            case ipv6_char_after_closing:
                return {"Invalid character found after closing (the ']' character) an IPv6 host."};


                // port-specific errors:
            case port_out_of_range:
                return {
                  "Port number is too big; "
                  "more info: https://url.spec.whatwg.org/#port-out-of-range"};
            case port_invalid:
                return {
                  "Invalid characters used in port; "
                  "more info: https://url.spec.whatwg.org/#port-invalid"};


                // path-specific errors/warnings:
            case valid_path: return {"Valid URI until path; parsing is not done yet."};
            case valid_opaque_path: return {"Valid URI until opaque path; parsing is not done yet."};
            case reverse_solidus_used:
                return {
                  "The URI is using backslash instead of a forward slash; "
                  "more info: https://url.spec.whatwg.org/#invalid-reverse-solidus"};
            case windows_drive_letter_used:
                return {"Windows Drive Letters is being used inside the 'file:' scheme."};
            case windows_drive_letter_in_ralative_url:
                return {
                  "The URI is relative, starts with Windows Drive Letter, "
                  "and the base URI's scheme is 'file:'; "
                  "more info: https://url.spec.whatwg.org/#file-invalid-windows-drive-letter"};
            case windows_drive_letter_as_host:
                return {
                  "A 'file:' URI cannot have a Windows Drive Letter as a host; "
                  "more info: https://url.spec.whatwg.org/#file-invalid-windows-drive-letter-host"};


            // queries-specific errors/warnings:
            case valid_queries: return {"Valid URI until queries, parsing is not done yet."};
            case invalid_queries_character:
                return {"Invalid character found in queries."};


                // fragment-specific errors/warnings:
            case valid_fragment: return {"Valid URI until fragment, parsing is not done yet."};
            default: stl::unreachable();
        }
    }

    [[nodiscard]] static constexpr bool is_valid(stl::underlying_type_t<uri_status> const status) noexcept {
        return (error_bit & status) == 0;
    }

    [[nodiscard]] static constexpr bool is_valid(uri_status const status) noexcept {
        return is_valid(stl::to_underlying(status));
    }

    [[nodiscard]] static constexpr bool has_warnings(
      stl::underlying_type_t<uri_status> const status) noexcept {
        return (status & warnings_mask) != 0;
    }

    [[nodiscard]] static constexpr bool has_warnings(uri_status const status) noexcept {
        return has_warnings(stl::to_underlying(status));
    }

    [[nodiscard]] static constexpr bool has_warning(stl::underlying_type_t<uri_status> const status,
                                                    uri_status const warning) noexcept {
        return (status & stl::to_underlying(warning)) == stl::to_underlying(warning);
    }

    [[nodiscard]] static constexpr bool has_warning(
      stl::underlying_type_t<uri_status> const status,
      stl::underlying_type_t<uri_status> const warning) noexcept {
        return (status & warning) == warning;
    }

    [[nodiscard]] static constexpr bool has_error(stl::underlying_type_t<uri_status> const status) noexcept {
        return (error_bit & status) == error_bit;
    }

    [[nodiscard]] static constexpr bool has_error(uri_status const status) noexcept {
        return has_error(stl::to_underlying(status));
    }

    /// get the error/valid value without the warnings if available
    [[nodiscard]] static constexpr uri_status get_value(
      stl::underlying_type_t<uri_status> const status) noexcept {
        return static_cast<uri_status>(status & values_mask);
    }

    [[nodiscard]] static constexpr uri_status get_value(uri_status const status) noexcept {
        return get_value(stl::to_underlying(status));
    }

    [[nodiscard]] static constexpr bool has_error(stl::underlying_type_t<uri_status> const status,
                                                  uri_status const expected_err) noexcept {
        return get_value(status) == expected_err;
    }

    [[nodiscard]] static constexpr bool has_error(uri_status const status,
                                                  uri_status const expected_err) noexcept {
        return has_error(stl::to_underlying(status), expected_err);
    }

    static constexpr void set_valid(stl::underlying_type_t<uri_status>& status,
                                    uri_status const                    value) noexcept {
        status &= ~values_mask;
        status |= stl::to_underlying(value);
    }

    static constexpr void set_error(stl::underlying_type_t<uri_status>& status,
                                    uri_status const                    value) noexcept {
        status = stl::to_underlying(value);
    }

    /// multiple calls with the same value must not affect the result, meaning, if you set an specific warning
    /// 5 times, the status should not be corrupted.
    static constexpr void set_warning(stl::underlying_type_t<uri_status>& status,
                                      uri_status const                    value) noexcept {
        status |= stl::to_underlying(value);
    }

    [[nodiscard]] static constexpr uri_status get_warning(uri_status_type const status) noexcept {
        return static_cast<uri_status>(stl::bit_floor(warnings_mask & status));
    }

    [[nodiscard]] static constexpr uri_status get_warning(uri_status const status) noexcept {
        return get_warning(stl::to_underlying(status));
    }

    /**
     * This is a forward iterator that gives you all the warnings inside a uri status.
     * You can use this to print the warnings and errors of a URI.
     *
     * uri_status_iterator is designed to work with ranges.
     *
     * Example Usage with ranges:
     * @code
     *     // Create a string containing all the error messages:
     *     uint32_t status = ...;
     *     std::string errors_string =
     *        uri_status_iterator{status} | transform([](uri_status cur_status) {
     *            return to_string(cur_status);
     *        }) | join_with('\n') | to<std::string>();
     *
     *     // If status is already in type of uri_status, you can just use status directly,
     *     // because std::begin(status) returns a uri_status_iterator:
     *     string error_string2 =
     *        status | transform(...) | join_with('\n') | to<string>();
     * @endcode
     */
    struct uri_status_iterator {
        using value_type   = uri_status;
        using storage_type = stl::underlying_type_t<value_type>;

        using difference_type   = stl::ptrdiff_t;
        using reference         = value_type&;
        using const_reference   = value_type const&;
        using pointer           = value_type*;
        using const_pointer     = value_type const*;
        using iterator_category = stl::forward_iterator_tag;
        using iterator_concept  = stl::forward_iterator_tag;


        constexpr uri_status_iterator() noexcept = default;

        constexpr explicit uri_status_iterator(uri_status const inp_status) noexcept
          : status{stl::to_underlying(inp_status)},
            current{get_warning(status)} {}

        constexpr explicit uri_status_iterator(storage_type const inp_status) noexcept
          : status{inp_status},
            current{get_warning(status)} {}

        constexpr uri_status_iterator(uri_status_iterator const&) noexcept            = default;
        constexpr uri_status_iterator(uri_status_iterator&&) noexcept                 = default;
        constexpr uri_status_iterator& operator=(uri_status_iterator const&) noexcept = default;
        constexpr uri_status_iterator& operator=(uri_status_iterator&&) noexcept      = default;
        constexpr ~uri_status_iterator() noexcept                                     = default;

        constexpr uri_status_iterator& operator++() noexcept {
            if (has_warnings(status)) {
                // remove the first warning
                auto const left_most_warning_bit  = stl::bit_floor(warnings_mask & status);
                status                           &= ~left_most_warning_bit;

                current = has_warnings(status) ? get_warning(status) : static_cast<uri_status>(status);
            } else {
                status  = 0U;
                current = static_cast<uri_status>(status);
            }
            return *this;
        }

        [[nodiscard]] constexpr uri_status_iterator operator++(int) noexcept {
            uri_status_iterator const iter{*this};
            ++*this;
            return iter;
        }

        constexpr const_reference operator*() const noexcept {
            // return valid, error, or 0; it doesn't matter because there's only one of them
            return current;
        }

        constexpr const_pointer operator->() const noexcept {
            return &current;
        }

        constexpr pointer operator->() noexcept {
            return &current;
        }

        [[nodiscard]] constexpr bool operator==(uri_status_iterator const rhs) const noexcept {
            return status == rhs.status;
        }

        [[nodiscard]] constexpr bool operator!=(uri_status_iterator const rhs) const noexcept {
            return status != rhs.status;
        }

        [[nodiscard]] constexpr auto operator<=>(uri_status_iterator const rhs) const noexcept {
            return status <=> rhs.status;
        }

      private:
        storage_type status  = 0U;
        value_type   current = uri_status::unparsed;
    };

    [[nodiscard]] static constexpr uri_status_iterator begin(uri_status_iterator status) noexcept {
        return {status};
    }

    [[nodiscard]] static constexpr uri_status_iterator end(
      [[maybe_unused]] uri_status_iterator iter) noexcept {
        return {};
    }

    [[nodiscard]] static constexpr uri_status_iterator begin(uri_status const status) noexcept {
        return uri_status_iterator{status};
    }

    [[nodiscard]] static constexpr uri_status_iterator end([[maybe_unused]] uri_status status) noexcept {
        return {};
    }

    // NOLINTEND(*-magic-numbers)

} // namespace webpp::uri


#endif // WEBPP_URI_STATUS_HPP
