// Created by moisrex on 5/12/23.

#ifndef WEBPP_URI_DOMAIN_HPP
#define WEBPP_URI_DOMAIN_HPP

#include "../std/string_like.hpp"
#include "./parser/constants.hpp"
#include "./parser/iiequals.hpp"
#include "./parser/parse_domain.hpp"

#include <compare>

namespace webpp::uri {
    /**
     * Structured Domain Name
     */
    template <istl::StringLike StorageT = stl::string_view>
    struct basic_domain {
        using string_type  = StorageT;
        using char_type    = typename string_type::value_type;
        using storage_type = StorageT;

        static constexpr bool is_modifiable   = istl::ModifiableString<string_type>;
        static constexpr bool is_nothrow      = !is_modifiable;
        static constexpr bool needs_allocator = requires { typename string_type::allocator_type; };

      private:
        storage_type       storage;
        domain_name_status status = domain_name_status::unparsed;

      public:
        template <typename... Args>
        explicit constexpr basic_domain(Args&&... args)
          noexcept(stl::is_nothrow_constructible_v<storage_type, Args...>)
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

        template <istl::StringViewifiable NStrT = stl::basic_string_view<char_type>>
        [[nodiscard]] constexpr bool operator==(NStrT&& inp_str) const noexcept {
            if constexpr (is_modifiable) {
                return iiequals_fl<details::TABS_OR_NEWLINES<char_type>>(storage,
                                                                         stl::forward<NStrT>(inp_str));
            } else {
                return iiequals<details::TABS_OR_NEWLINES<char_type>>(storage, stl::forward<NStrT>(inp_str));
            }
        }

        [[nodiscard]] constexpr bool operator==(basic_domain const& other) const noexcept {
            return storage == other.storage && status == other.status;
        }

        [[nodiscard]] constexpr stl::strong_ordering operator<=>(
          basic_domain const& other) const noexcept = default;

        /// Top-Level-Domain
        template <istl::StringLike StrT = storage_type>
        [[nodiscard]] constexpr StrT tld() const noexcept {
            // todo: this does not handle label separators (only in string_view probably)
            // https://www.unicode.org/reports/tr46/#Notation
            if (auto const pos = storage.rfind('.'); pos != StrT::npos) {
                return storage.substr(pos + 1);
            }
            return storage; // the whole thing is a TLD
        }
    };

} // namespace webpp::uri

#endif // WEBPP_URI_DOMAIN_HPP
