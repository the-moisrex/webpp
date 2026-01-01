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
        using string_type      = StorageT;
        using char_type        = typename string_type::value_type;
        using storage_type     = StorageT;
        using string_view_type = istl::string_view_type_of<string_type>;

        static constexpr bool is_modifiable = istl::ModifiableString<string_type>;
        static constexpr bool is_nothrow    = !is_modifiable;

      private:
        storage_type       storage;
        domain_name_status status = domain_name_status::unparsed;

      public:
        basic_domain(basic_domain const&)                = default;
        basic_domain(basic_domain&&) noexcept            = default;
        basic_domain& operator=(basic_domain const&)     = default;
        basic_domain& operator=(basic_domain&&) noexcept = default;
        ~basic_domain()                                  = default;

        template <typename... Args>
        explicit constexpr basic_domain(Args&&... args) noexcept(is_nothrow)
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

        template <istl::StringViewifiable NStrT = string_view_type>
        [[nodiscard]] constexpr bool operator==(NStrT&& inp_str) const noexcept {
            return iiequals_afl(storage, stl::forward<NStrT>(inp_str));
        }

        [[nodiscard]] constexpr bool operator==(basic_domain const& other) const noexcept {
            return storage == other.storage && status == other.status;
        }

        [[nodiscard]] constexpr stl::strong_ordering operator<=>(basic_domain const& other) const noexcept = default;

        /// Top-Level-Domain
        [[nodiscard]] constexpr string_view_type tld() const noexcept {
            // Domain Separators can be Unicode Code Points as well, but if the transition was not successful, then
            // we'll have an error anyway.
            // https://www.unicode.org/reports/tr46/#Notation
            if (auto const pos = storage.rfind('.'); pos != string_view_type::npos) {
                return storage.substr(pos + 1);
            }
            return storage; // the whole thing is a TLD
        }
    };

} // namespace webpp::uri

#endif // WEBPP_URI_DOMAIN_HPP
