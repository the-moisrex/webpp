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

        seg_type scheme_end      = 0;
        seg_type authority_start = 0;
        seg_type password_start  = 0;
        seg_type host_start      = 0;
        seg_type port_start      = 0;
        seg_type authority_end   = 0;
        seg_type query_start     = 0;
        seg_type fragment_start  = 0;
    };


    template <istl::StringLike StrT>
    struct uri_components<StrT> {
        using string_type = StrT;
        using seg_type    = string_type;
        using char_type   = typename string_type::value_type;

        string_type scheme{};
        string_type username{};
        string_type password{};
        string_type host{};
        string_type port{};
        string_type path{};
        string_type queries{};
        string_type fragment{};
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
