// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_HOST_HPP
#define WEBPP_URI_HOST_HPP

#include "../ip/ipv4.hpp"
#include "../ip/ipv6.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../strings/splits.hpp"
#include "./parser/uri_components.hpp"

#include <variant>
#include <version>

namespace webpp::uri {

    template <typename StrT>
    struct domain_labels_splitter {
        using char_type  = typename StrT::value_type;
        using delim_type = stl::basic_string_view<char_type>;
        using type =
          strings::splitter<typename StrT::iterator, char_type, delim_type, delim_type, delim_type>;
    };

    /// IDNA Mapping have already changed the Modifiable strings
    template <istl::String StrT>
    struct domain_labels_splitter<StrT> {
        using char_type = typename StrT::value_type;
        using type      = strings::splitter<typename StrT::iterator, char_type>;
    };

    template <typename StrT>
    using domain_labels_splitter_t = typename domain_labels_splitter<StrT>::type;

    template <typename T, typename U, stl::size_t N>
    [[nodiscard]] consteval stl::array<T, N> to_array(stl::array<U, N> const& src) noexcept {
        stl::array<T, N> out{};
        stl::size_t      index = 0;
        for (auto const val : src) {
            out[index++] = val;
        }
        return out;
    }

    template <typename T, typename... U>
    [[nodiscard]] consteval stl::array<T, sizeof...(U)> to_array(U... src) noexcept {
        return stl::array<T, sizeof...(U)>{static_cast<T>(src)...};
    }

    template <typename T, typename U, stl::size_t LENGTH>
    struct string_view_array {
      private:
        stl::array<T, LENGTH> data{};

      public:
        template <typename... RU>
            requires(stl::same_as<RU, U> && ...)
        explicit(false) consteval string_view_array(RU... rest) noexcept : data{static_cast<T>(rest)...} {}

        [[nodiscard]] consteval stl::basic_string_view<T> view() const noexcept {
            return stl::basic_string_view<T>(data.data(), LENGTH);
        }
    };

    /**
     * Iterator through labels of a valid domain name
     */
    template <istl::StringLike StrT = stl::string_view>
    [[nodiscard]] constexpr auto domain_labels(StrT&& str) noexcept {
        using splitter_type = domain_labels_splitter_t<stl::remove_cvref_t<StrT>>;
        using char_type     = typename stl::remove_cvref_t<StrT>::value_type;
        if constexpr (istl::String<StrT>) {
            // IDNA processing should have already done the mapping of the other label separators
            return splitter_type{stl::forward<StrT>(str), static_cast<char_type>('.')};
        } else {
            // label separators:
            // https://www.unicode.org/reports/tr46/#Notation
            // NOLINTBEGIN(*-magic-numbers)
            return splitter_type{
              stl::forward<StrT>(str),
              static_cast<char_type>('.'),
              string_view_array<char_type, int, 4>{0xef, 0xbc, 0x8e, 0}
                .view(), // \uFF0E
              string_view_array<char_type, int, 4>{0xe3, 0x80, 0x82, 0}
                .view(), // \u3002
              string_view_array<char_type, int, 4>{0xef, 0xbd, 0xa1, 0}
                .view()  // \uFF61
            };
            // NOLINTEND(*-magic-numbers)
        }
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
     *
     * @tparam StringType
     */
    template <istl::StringLike StringType = stl::string_view>
    struct basic_host {
        using string_type       = StringType;
        using char_type         = typename string_type::value_type;
        using modifiable_string = istl::defaulted_string<string_type>;
        using string_view_type  = istl::string_view_type_of<string_type>;
        using storage_type      = stl::variant<stl::monostate, pure_ipv4, pure_ipv6, string_type>;

        static constexpr bool is_modifiable = istl::ModifiableString<string_type>;
        static constexpr bool is_nothrow    = !is_modifiable;
        static constexpr bool is_segregated = false;

      private:
        storage_type storage = stl::monostate{};

      public:
        constexpr basic_host(basic_host const& rhs)                = default;
        constexpr basic_host(basic_host&& rhs) noexcept            = default;
        constexpr basic_host& operator=(basic_host const& rhs)     = default;
        constexpr basic_host& operator=(basic_host&& rhs) noexcept = default;
        constexpr ~basic_host()                                    = default;

        template <uri_parsing_options Options = uri_parsing_options{}, typename Iter>
        constexpr uri_status_type parse(Iter beg, Iter end) noexcept(is_nothrow) {
            parsing_uri_component_context<components::host, basic_host*, stl::remove_cvref_t<Iter>> ctx{};
            ctx.beg = beg;
            ctx.pos = beg;
            ctx.end = end;
            ctx.out = this;
            // todo: parse hose not authority (even though they will be ignored)
            parse_authority<Options>(ctx);
            return ctx.status;
        }

        template <Allocator AllocT = stl::allocator<char_type>>
        explicit constexpr basic_host(AllocT const& alloc = {}) noexcept : storage{string_type{alloc}} {}

        template <istl::String InpStr = modifiable_string>
            requires is_modifiable
        explicit constexpr basic_host(InpStr const& str) noexcept(is_nothrow)
          : storage{string_type{str.get_allocator()}} {
            parse(str.begin(), str.end());
        }

        template <istl::StringViewifiable InpStr = string_view_type,
                  Allocator               AllocT = stl::allocator<char_type>>
            requires(is_modifiable && !istl::String<InpStr>)
        explicit constexpr basic_host(InpStr&& inp_str, AllocT alloc = {}) noexcept(is_nothrow)
          : storage{string_type{alloc}} {
            auto const str = istl::string_viewify(stl::forward<InpStr>(inp_str));
            parse(str.begin(), str.end());
        }

        template <istl::StringViewifiable InpStr = string_view_type>
            requires(!is_modifiable)
        explicit constexpr basic_host(InpStr&& inp_str) noexcept(is_nothrow) {
            auto const str = istl::string_viewify(stl::forward<InpStr>(inp_str));
            parse(str.begin(), str.end());
        }

        template <istl::StringLike InpStr>
        constexpr basic_host& operator=(InpStr const& inp_str) noexcept(is_nothrow) {
            parse(inp_str.begin(), inp_str.end());
            return *this;
        }

        constexpr basic_host& operator=(pure_ipv6 ip6) noexcept(is_nothrow) {
            storage = ip6;
            return *this;
        }

        constexpr basic_host& operator=(pure_ipv6::octets_t ip6) noexcept(is_nothrow) {
            storage = ip6;
            return *this;
        }

        constexpr basic_host& operator=(pure_ipv4::octets_t ip4) noexcept(is_nothrow) {
            storage = ip4;
            return *this;
        }

        constexpr void assign(pure_ipv6 ip6) noexcept(is_nothrow) {
            storage = ip6;
        }

        constexpr void assign(pure_ipv6::octets_t ip6) noexcept(is_nothrow) {
            storage = ip6;
        }

        constexpr void assign(pure_ipv4::octets_t ip4) noexcept(is_nothrow) {
            storage = ip4;
        }

        // // Append a label to the end of the domain
        // template <istl::StringViewifiable StrT>
        //     requires is_modifiable
        // constexpr uri_status append_label(StrT&& inp_str) {
        //     if (!stl::holds_alternative<string_type>(storage)) {
        //         return uri_status::hostname_type_mismatch;
        //     }
        //     auto const str = istl::string_viewify(stl::forward<StrT>(inp_str));
        //     return parse(str.begin(), str.end());
        // }
        //
        // // Prepend a label to the beginning of the domain
        // template <istl::StringViewifiable StrT>
        //     requires is_modifiable
        // constexpr uri_status prepend_label(StrT&& inp_str) {
        //     auto const status = append_label(stl::forward<StrT>(inp_str));
        //     if (is_valid(status)) [[likely]] {
        //         auto& domain = *as_domain();
        //         // move the last item to the beginning:
        //         stl::rotate(domain.rbegin(), stl::next(domain.rbegin()), domain.rend());
        //     }
        //     return status;
        // }

        [[nodiscard]] constexpr string_type const* as_domain() const noexcept {
            return get_if<string_type>(&storage);
        }

        [[nodiscard]] constexpr string_type* as_domain() noexcept {
            return get_if<string_type>(&storage);
        }

        [[nodiscard]] constexpr pure_ipv4 const* as_ipv4() const noexcept {
            return get_if<pure_ipv4>(&storage);
        }

        [[nodiscard]] constexpr pure_ipv4* as_ipv4() noexcept {
            return get_if<pure_ipv4>(&storage);
        }

        [[nodiscard]] constexpr pure_ipv6 const* as_ipv6() const noexcept {
            return get_if<pure_ipv6>(&storage);
        }

        [[nodiscard]] constexpr pure_ipv6* as_ipv6() noexcept {
            return get_if<pure_ipv6>(&storage);
        }

        /**
         * @brief Replace the values with the specified raw data, without parsing
         * @param beg start of the value
         * @param end the end of the value
         */
        template <typename Iter>
        constexpr void assign(Iter beg, Iter end) {
            storage = string_type{beg, end};
        }

        template <istl::String NStrT = modifiable_string>
        constexpr void to_string(NStrT& out) const {
            if (auto* domain = as_domain()) {
                out += *domain;
            } else if (auto* ip4 = as_ipv4()) {
                ip4->to_string(out);
            } else if (auto* ip6 = as_ipv6()) {
                ip6->to_string(out);
            }
        }

        template <istl::String NStrT = modifiable_string, typename... Args>
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
            if (stl::holds_alternative<stl::monostate>(storage)) {
                return true;
            }
            if (auto* domain = as_domain()) {
                return !domain->empty();
            }
            return false;
        }

        /**
         * Top Level Domain; sometimes called the extension
         */
        template <istl::StringView StrV = string_view_type>
        [[nodiscard]] constexpr StrV tld() const noexcept {
            if (auto* domain = as_domain()) {
                return domain_labels(*domain).begin().template value<StrV>();
            }
            return {};
        }

        /// Split the domain labels
        [[nodiscard]] constexpr domain_labels_splitter_t<string_type> labels() const noexcept {
            if (auto* domain = as_domain()) {
                return domain_labels(*domain);
            }
            return domain_labels(string_type{});
        }

        [[nodiscard]] constexpr auto& storage_ref() noexcept {
            return storage;
        }

        [[nodiscard]] constexpr auto const& storage_ref() const noexcept {
            return storage;
        }

        constexpr void clear() noexcept {
            storage = stl::monostate{};
        }

        /// Calculate the length as a string
        [[nodiscard]] constexpr stl::size_t size() const noexcept {
            if (auto* domain = as_domain()) {
                return domain->size();
            }
            if (auto* ip4 = as_ipv4()) {
                return ip4->size(); // re-calculates the size again
            }
            if (auto* ip6 = as_ipv6()) {
                return ip6->size(); // re-calculates the size again
            }
            return 0;
        }

        /// Equality check
        /// https://url.spec.whatwg.org/#host-equivalence
        /// Attention: this function doesn't parse your input
        // template <istl::StringViewifiable NStrT = stl::basic_string_view<char_type>>
        // [[nodiscard]] constexpr bool operator==(NStrT&& inp_str) const noexcept {
        //     auto str      = istl::string_viewify(stl::forward<NStrT>(inp_str));
        //     auto piece_it = storage.begin();
        //     for (; piece_it != storage.end(); ++piece_it) {
        //         bool should_continue = false;
        //         if constexpr (is_modifiable) {
        //             should_continue = iiequals_fl<details::TABS_OR_NEWLINES<char_type>>(
        //               *piece_it,
        //               str.substr(0, piece_it->size()));
        //         } else {
        //             should_continue = iiequals<details::TABS_OR_NEWLINES<char_type>>(
        //               *piece_it,
        //               str.substr(0, piece_it->size()));
        //         }
        //         if (!should_continue) {
        //             return false;
        //         }
        //         str.remove_prefix(piece_it->size());
        //         // todo: does this support label-separators?
        //         if (!str.starts_with('.')) {
        //             ++piece_it;
        //             break;
        //         }
        //         str.remove_prefix(1);
        //     }
        //     return str.empty() && piece_it == storage.end();
        // }

        // [[nodiscard]] constexpr bool operator==(basic_host const& other) const noexcept {
        //     return iiequals<details::TABS_OR_NEWLINES<char_type>>(storage, other.storage_ref());
        // }
    };

    template <istl::StringView T>
    basic_host(T&&) -> basic_host<stl::remove_cvref_t<T>>;

    template <istl::String T>
    basic_host(T&&) -> basic_host<stl::remove_cvref_t<T>>;

    /// Check if it's string "localhost"
    template <typename... T>
    [[nodiscard]] static constexpr bool is_localhost_string(basic_host<T...> const& host) noexcept {
        using char_type   = typename basic_host<T...>::char_type;
        using string_type = typename basic_host<T...>::string_type;
        if (auto* domain = host.as_domain()) {
            return iiequals_fl<details::TABS_OR_NEWLINES<char_type>>("localhost", *domain);
        }
        if (auto* str = get_if<string_type>(&host.storage_ref())) {
            return iiequals_fl<details::TABS_OR_NEWLINES<char_type>>("localhost", *str);
        }
        return false;
    }


} // namespace webpp::uri

#endif // WEBPP_URI_HOST_HPP
