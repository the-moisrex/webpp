// Created by moisrex on 10/9/23.

#ifndef WEBPP_URL_COMPONENTS_HPP
#define WEBPP_URL_COMPONENTS_HPP

#include "../std/concepts.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"

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
        seg_type user_info_end   = 0;
        seg_type port_start      = 0;
        seg_type authority_end   = 0;
        seg_type query_start     = 0;
        seg_type fragment_start  = 0;
    };


    template <typename StrT>
        requires(istl::String<StrT> || istl::StringView<StrT>)
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
    create_uri_component(typename StrT::allocator_type const& alloc) noexcept(
      stl::is_nothrow_constructible_v<StrT, typename StrT::allocator_type>) {
        return {.scheme{alloc},
                .username{alloc},
                .password{alloc},
                .host{alloc},
                .port{alloc},
                .path{alloc},
                .queries{alloc},
                .fragment{alloc}};
    }

    using uri_components_view = uri_components<stl::string_view>;
    using uri_components_u32  = uri_components<stl::uint32_t>;

} // namespace webpp::uri

#endif // WEBPP_URL_COMPONENTS_HPP
