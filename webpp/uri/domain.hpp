// Created by moisrex on 5/12/23.

#ifndef WEBPP_URI_DOMAIN_HPP
#define WEBPP_URI_DOMAIN_HPP

#include "../std/string_like.hpp"
#include "../strings/charset.hpp"
#include "./details/uri_status.hpp"

#include <compare>
#include <cstdint>

namespace webpp {
    enum struct domain_name_status : stl::underlying_type_t<uri::uri_status> { // NOLINT(*-enum-size)
    // NOLINTBEGIN(*-macro-usage)
#define webpp_def(status) status = stl::to_underlying(uri::uri_status::status)
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
            case '.': return empty_subdomain;
            case '-': return begin_with_hyphen;
            default: break;
        }

        bool has_punycode    = false;
        auto subdomain_start = pos;
        while (pos != end) {
            if (*pos == 'x' && end - pos > 4 && *++pos == 'n' && *++pos == '-' && *++pos == '-') {
                has_punycode = true;
                pos = charset{ALPHA_DIGIT<char_type>, charset<char_type, 1>{'-'}}.find_first_not_in(pos, end);
                continue;
            }

            switch (char const cur_char = *pos++) {
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
                    if (pos - subdomain_start > details::subdomain_threshold) {
                        return subdomain_too_long;
                    }
                    subdomain_start = pos;
                    continue;
                case '-':
                    if (pos == end || *pos == '.') {
                        return end_with_hyphen;
                    }
                    if (*pos == '-') {
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

    /**
     * Structured Domain Name
     */
    template <istl::StringLike StorageT = stl::string_view>
    struct basic_domain {
        using storage_type = StorageT;

      private:
        storage_type       storage;
        domain_name_status status = domain_name_status::unparsed;

      public:
        template <typename... Args>
        explicit constexpr basic_domain(Args&&... args) noexcept(
          stl::is_nothrow_constructible_v<storage_type, Args...>)
          : storage{stl::forward<Args>(args)...},
            status{parse_domain_name(storage.begin(), storage.end())} {}

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

        [[nodiscard]] constexpr stl::strong_ordering operator<=>(
          basic_domain const& other) const noexcept = default;

        /// Top-Level-Domain
        template <istl::StringLike StrT = storage_type>
        [[nodiscard]] constexpr StrT tld() const noexcept {
            if (auto const pos = storage.rfind('.'); pos != StrT::npos) {
                return storage.substr(pos + 1);
            }
            return storage; // the whole thing is a TLD
        }
    };

} // namespace webpp

#endif // WEBPP_URI_DOMAIN_HPP
