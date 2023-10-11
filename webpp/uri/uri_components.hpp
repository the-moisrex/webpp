// Created by moisrex on 10/9/23.

#ifndef WEBPP_URL_COMPONENTS_HPP
#define WEBPP_URL_COMPONENTS_HPP

#include "../std/concepts.hpp"
#include "../std/string_like.hpp"

#include <cstdint>
#include <limits>

namespace webpp::uri {

    template <typename SegType = stl::uint32_t>
    struct uri_components;

    template <typename T>
    struct is_uri_component : stl::false_type {};

    template <typename SegType>
    struct is_uri_component<uri_components<SegType>> : stl::true_type {};

    template <typename T>
    concept OptionalURIComponent =
      stl::same_as<T, istl::nothing_type> ||
      (is_uri_component<stl::remove_reference_t<T>>::value && stl::is_reference_v<T>);

    /**
     * URL Components
     * @tparam SegType defines the size of each component, for example if you specify uint32_t, then this
     * class will only support up to 4GiB of URL.
     *
     * RFC: https://tools.ietf.org/html/rfc3986
     *
     *    foo://example.com:8042/over/there?name=ferret#nose
     *    \_/   \______________/\_________/ \_________/ \__/
     *     |           |            |            |        |
     *  scheme     authority       path        query   fragment
     *     |   _____________________|__
     *    / \ /                        \
     *    urn:example:animal:ferret:nose
     *
     *  [protocol"://"[username[":"password]"@"]hostname[":"port]"/"?][path]["?"querystring]["#"fragment]
     */
    template <stl::integral SegType>
    struct uri_components<SegType> {
        using seg_type = SegType;

        /// maximum number that this url component class supports
        static constexpr auto max_supported_length = stl::numeric_limits<seg_type>::max() - 1;

        static constexpr seg_type omitted = stl::numeric_limits<seg_type>::max();

        /// resetting the values of integer types are always noexcept
        static constexpr bool is_nothrow = true;

        static constexpr bool is_overridable = false;

        seg_type scheme_end      = omitted; // excluding :// stuff
        seg_type authority_start = omitted; // username/host start
        seg_type password_start  = omitted;
        seg_type host_start      = omitted;
        seg_type port_start      = omitted;
        seg_type authority_end   = omitted; // path start
        seg_type query_start     = omitted; // path end
        seg_type fragment_start  = omitted; // query end

        template <typename CharT = char>
        constexpr void set_scheme(CharT const* beg, CharT const* end) noexcept(is_nothrow) {
            scheme_end = static_cast<seg_type>(end - beg);
        }

        constexpr void clear_port() noexcept {
            port_start = omitted;
        }

        [[nodiscard]] constexpr bool has_scheme() const noexcept {
            return scheme_end != omitted;
        }

        [[nodiscard]] constexpr bool has_username() const noexcept {
            return authority_start != omitted;
        }

        [[nodiscard]] constexpr bool has_password() const noexcept {
            return password_start != omitted;
        }

        [[nodiscard]] constexpr bool has_credentials() const noexcept {
            return authority_start != omitted;
        }

        [[nodiscard]] constexpr bool has_port() const noexcept {
            return port_start != omitted;
        }

        [[nodiscard]] constexpr bool has_path() const noexcept {
            return authority_end != omitted;
        }

        [[nodiscard]] constexpr bool has_query() const noexcept {
            return query_start != omitted;
        }

        [[nodiscard]] constexpr bool has_fragment() const noexcept {
            return fragment_start != omitted;
        }

        // template <typename CharT = char>
        // constexpr void set_username(CharT const* beg, CharT const* end) noexcept(is_nothrow) {
        //     authority_start = static_cast<seg_type>(beg);
        //     password_start = static_cast<seg_type>(end);
        // }

        // template <typename CharT = char>
        // constexpr void set_password(CharT const* beg, CharT const* end) noexcept(is_nothrow) {
        //     istl::assign(password, beg, end);
        // }

        // template <typename CharT = char>
        // constexpr void set_host(CharT const* beg, CharT const* end) noexcept(is_nothrow) {
        //     istl::assign(host, beg, end);
        // }

        // template <typename CharT = char>
        // constexpr void set_port(CharT const* beg, CharT const* end) noexcept(is_nothrow) {
        //     istl::assign(port, beg, end);
        // }

        // template <typename CharT = char>
        // constexpr void set_path(CharT const* beg, CharT const* end) noexcept(is_nothrow) {
        //     istl::assign(path, beg, end);
        // }

        // template <typename CharT = char>
        // constexpr void set_queries(CharT const* beg, CharT const* end) noexcept(is_nothrow) {
        //     istl::assign(queries, beg, end);
        // }

        // template <typename CharT = char>
        // constexpr void set_fragment(CharT const* beg, CharT const* end) noexcept(is_nothrow) {
        //     istl::assign(fragment, beg, end);
        // }
    };


    template <istl::StringLike StrT>
    struct uri_components<StrT> {
        using string_type   = StrT;
        using seg_type      = string_type;
        using char_type     = typename string_type::value_type;
        using size_type     = typename string_type::size_type;
        using pointer       = typename string_type::pointer;
        using const_pointer = typename string_type::const_pointer;

        /// maximum number that this url component class supports
        static constexpr auto max_supported_length = stl::numeric_limits<size_type>::max() - 1;

        /// is resetting the values are noexcept or not
        static constexpr bool is_nothrow = stl::is_nothrow_assignable_v<string_type, char_type const*>;

        static constexpr bool is_overridable = false;


        string_type scheme{};
        string_type username{};
        string_type password{};
        string_type host{};
        string_type port{};
        string_type path{};
        string_type queries{};
        string_type fragment{};

        constexpr void set_scheme(const_pointer beg, const_pointer end) noexcept(is_nothrow) {
            istl::assign(scheme, beg, end);
        }

        constexpr void set_username(const_pointer beg, const_pointer end) noexcept(is_nothrow) {
            istl::assign(username, beg, end);
        }

        constexpr void set_password(const_pointer beg, const_pointer end) noexcept(is_nothrow) {
            istl::assign(password, beg, end);
        }

        constexpr void set_host(const_pointer beg, const_pointer end) noexcept(is_nothrow) {
            istl::assign(host, beg, end);
        }

        constexpr void set_port(const_pointer beg, const_pointer end) noexcept(is_nothrow) {
            istl::assign(port, beg, end);
        }

        constexpr void set_path(const_pointer beg, const_pointer end) noexcept(is_nothrow) {
            istl::assign(path, beg, end);
        }

        constexpr void set_queries(const_pointer beg, const_pointer end) noexcept(is_nothrow) {
            istl::assign(queries, beg, end);
        }

        constexpr void set_fragment(const_pointer beg, const_pointer end) noexcept(is_nothrow) {
            istl::assign(fragment, beg, end);
        }

        constexpr void clear_port() noexcept {
            istl::clear(port);
        }

        [[nodiscard]] constexpr bool has_scheme() const noexcept {
            return !scheme.empty();
        }

        [[nodiscard]] constexpr bool has_username() const noexcept {
            return !username.empty();
        }

        [[nodiscard]] constexpr bool has_password() const noexcept {
            return !password.empty();
        }

        [[nodiscard]] constexpr bool has_credentials() const noexcept {
            return has_username(); // password cannot exist without the username
        }

        [[nodiscard]] constexpr bool has_host() const noexcept {
            return !host.empty();
        }

        [[nodiscard]] constexpr bool has_port() const noexcept {
            return !port.empty();
        }

        [[nodiscard]] constexpr bool has_path() const noexcept {
            return !path.empty();
        }

        [[nodiscard]] constexpr bool has_queries() const noexcept {
            return !queries.empty();
        }

        [[nodiscard]] constexpr bool has_fragment() const noexcept {
            return !fragment.empty();
        }
    };

    /// Create a URI Component using an allocator
    template <istl::String StrT>
    [[nodiscard]] constexpr uri_components<StrT>
    uri_components_from(typename StrT::allocator_type const& alloc) noexcept(
      stl::is_nothrow_constructible_v<StrT, typename StrT::allocator_type>) {
        using string_type = StrT;
        return {.scheme   = string_type{alloc},
                .username = string_type{alloc},
                .password = string_type{alloc},
                .host     = string_type{alloc},
                .port     = string_type{alloc},
                .path     = string_type{alloc},
                .queries  = string_type{alloc},
                .fragment = string_type{alloc}};
    }


    template <istl::String StrT, stl::integral SegType, istl::StringLike SourceStr>
        requires(stl::same_as<istl::char_type_of_t<StrT>, istl::char_type_of_t<SourceStr>>)
    [[nodiscard]] constexpr uri_components<StrT>
    uri_components_from(uri_components<SegType> const&       comps,
                        SourceStr const&                     source,
                        typename StrT::allocator_type const& alloc =
                          {}) noexcept(stl::is_nothrow_constructible_v<StrT, typename StrT::allocator_type>) {
        using string_type = StrT;
        auto const beg    = source.data();
        return {
          .scheme = string_type{beg, comps.scheme_end, alloc},
          .username =
            string_type{beg + comps.authority_start, comps.host_start - comps.authority_start, alloc},
          .password = string_type{beg + comps.password_start, comps.host_start - comps.password_start, alloc},
          .host     = string_type{beg + comps.host_start, comps.port_start - comps.host_start, alloc},
          .port     = string_type{beg + comps.port_start, comps.authority_end - comps.port_start, alloc},
          .path     = string_type{beg + comps.authority_end, comps.query_start - comps.authority_end, alloc},
          .queries  = string_type{beg + comps.query_start, comps.fragment_start - comps.query_start, alloc},
          .fragment = string_type{beg + comps.fragment_start, source.size() - comps.fragment_start, alloc}};
    }

    template <istl::StringView StrT, stl::integral SegType, istl::StringLike SourceStr>
        requires(stl::same_as<istl::char_type_of_t<StrT>, istl::char_type_of_t<SourceStr>>)
    [[nodiscard]] constexpr uri_components<StrT> uri_components_from(uri_components<SegType> const& comps,
                                                                     SourceStr const& source) noexcept {
        using string_type = StrT;
        auto const beg    = source.data();
        return {.scheme = string_type{beg, comps.scheme_end},
                .username =
                  string_type{beg + comps.authority_start, comps.host_start - comps.authority_start},
                .password = string_type{beg + comps.password_start, comps.host_start - comps.password_start},
                .host     = string_type{beg + comps.host_start, comps.port_start - comps.host_start},
                .port     = string_type{beg + comps.port_start, comps.authority_end - comps.port_start},
                .path     = string_type{beg + comps.authority_end, comps.query_start - comps.authority_end},
                .queries  = string_type{beg + comps.query_start, comps.fragment_start - comps.query_start},
                .fragment = string_type{beg + comps.fragment_start, source.size() - comps.fragment_start}};
    }


    using uri_components_view = uri_components<stl::string_view>;
    using uri_components_u32  = uri_components<stl::uint32_t>;


} // namespace webpp::uri

#endif // WEBPP_URL_COMPONENTS_HPP
