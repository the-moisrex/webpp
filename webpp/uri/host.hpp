// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_HOST_HPP
#define WEBPP_URI_HOST_HPP

#include "../ip/ipv4.hpp"
#include "../ip/ipv6.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../strings/splits.hpp"

#include <variant>

namespace webpp::uri {

    /**
     * Iterator through labels of a valid domain name
     */
    template <typename CharT>
    [[nodiscard]] constexpr auto split_labels(stl::basic_string_view<CharT> const str = {}) noexcept {
        using string_view_type = stl::basic_string_view<CharT>;
        using splitter_type    = strings::splitter<typename string_view_type::iterator, CharT>;
        // IDNA processing should have already done the mapping of the other label separators
        return splitter_type{str, static_cast<CharT>('.')};
    }

    /**
     * @brief Basic Host
     * A host is
     *   - a domain,
     *   - an IP address,
     *   - an opaque host, or
     *   - an empty host.
     * Typically, a host serves as a network address, but it is sometimes used as opaque identifier in URLs
     * where a network address is not necessary.
     *
     * We're not going to segregate the host by into its labels initially since it doesn't seem like something
     * most users of this class would need.
     *
     * A typical URL whose host is an opaque host is git://github.com/whatwg/url.git.
     * https://url.spec.whatwg.org/#concept-host
     */
    template <typename CharT>
    struct [[nodiscard]]
    basic_host : stl::variant<stl::monostate, pure_ipv4, pure_ipv6, stl::basic_string_view<CharT>> {
        using string_view_type = stl::basic_string_view<CharT>;
        using char_type        = CharT;
        using storage_type     = stl::variant<stl::monostate, pure_ipv4, pure_ipv6, string_view_type>;

        using storage_type::variant; // ctor

        [[nodiscard]] constexpr string_view_type const* as_domain() const noexcept webpp_lifetimebound {
            return get_if<string_view_type>(this);
        }

        [[nodiscard]] constexpr pure_ipv4 const* as_ipv4() const noexcept {
            return get_if<pure_ipv4>(this);
        }

        [[nodiscard]] constexpr pure_ipv6 const* as_ipv6() const noexcept {
            return get_if<pure_ipv6>(this);
        }

        [[nodiscard]] constexpr bool is_localhost() const noexcept {
            return stl::visit([]<typename T>(T const& host) noexcept {
                if constexpr (stl::same_as<T, stl::monostate>) {
                    return false;
                } else if constexpr (stl::same_as<T, pure_ipv4>) {
                    return host == pure_ipv4::loopback();
                } else if constexpr (stl::same_as<T, pure_ipv6>) {
                    return host == pure_ipv6::loopback();
                } else {
                    return host == "localhost";
                }
            });
        }

        template <istl::String NStrT = stl::basic_string<CharT>>
        constexpr void to_string(NStrT& out) const {
            if (auto* domain = as_domain()) {
                out += *domain;
            } else if (auto* ip4 = as_ipv4()) {
                ip4->to_string(out);
            } else if (auto* ip6 = as_ipv6()) {
                ip6->to_string(out);
            }
        }

        template <istl::String NStrT = stl::basic_string<CharT>, typename... Args>
        [[nodiscard]] constexpr NStrT as_string(Args&&... args) const {
            NStrT out{stl::forward<Args>(args)...};
            to_string(out);
            return out;
        }

        /**
         * @brief check if we have value
         * @return false if we don't have anything
         */
        [[nodiscard]] constexpr bool has_value() const noexcept {
            return stl::holds_alternative<stl::monostate>(*this);
        }

        /**
         * Top Level Domain; sometimes called the extension
         */
        [[nodiscard]] constexpr string_view_type tld() const noexcept {
            if (auto* domain = as_domain()) {
                return split_labels(*domain).begin().template value<string_view_type>();
            }
            return {};
        }

        /// Split the domain labels
        [[nodiscard]] constexpr auto labels() const noexcept {
            if (auto* domain = as_domain()) {
                return split_labels(*domain);
            }
            return split_labels<char_type>();
        }

        constexpr void clear() noexcept {
            *this = stl::monostate{};
        }

        /// Calculate the length as a string
        [[nodiscard]] constexpr stl::size_t size() const noexcept {
            return stl::visit(
              []<typename T>(T const& host) noexcept {
                  if constexpr (!stl::same_as<T, stl::monostate>) {
                      return host.size();
                  } else {
                      return 0;
                  }
              },
              *this);
        }

        /// Equality check
        /// https://url.spec.whatwg.org/#host-equivalence
        /// Attention: this function doesn't parse your input
        [[nodiscard]] constexpr bool operator==(string_view_type const inp_str) const noexcept {
            return stl::visit([=]<typename T>(T const& host) noexcept {
                if constexpr (!stl::same_as<T, stl::monostate>) {
                    return host == inp_str;
                } else {
                    return false;
                }
            });
        }
    };

} // namespace webpp::uri

#endif // WEBPP_URI_HOST_HPP
