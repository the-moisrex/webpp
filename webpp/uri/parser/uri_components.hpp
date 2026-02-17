// Created by moisrex on 10/9/23.

#ifndef WEBPP_URL_COMPONENTS_HPP
#define WEBPP_URL_COMPONENTS_HPP

#include "../../std/collection.hpp"
#include "../../std/string_like.hpp"
#include "../../std/vector.hpp"
#include "../uri_status.hpp"

#include <cstdint>
#include <limits>

namespace webpp::uri {

    /**
     * URI Components contain all the pieces of a URL including:
     *  - Scheme
     *  - Credentials (Username and Password)
     *  - Hostname
     *  - Port
     *  - Path
     *  - Queries
     *  - Fragments
     * Though this doesn't mean we would force them to a specific name for each the of the components.
     */
    template <typename T, typename U = stl::remove_cvref_t<T>>
    concept URIComponents = requires {
        typename U::seg_type; // segment type (string/string-view/uint32_t/...)
        typename U::char_type;
        // requires can_get_allocator<U>;

        U::is_nothrow;
        U::is_modifiable;
        U::is_segregated;
        U::max_supported_length;
    };

    /// Relative Components are components that only point to the components of a URL using numbers or iterators or a
    /// combination of them.
    template <typename T>
    concept URIRelativeComponents = URIComponents<T> && requires(stl::remove_cvref_t<T> comps) {
        comps.uri_beg;
        comps.scheme_end;
        comps.authority_start;
        comps.password_start;
        comps.host_start;
        comps.port_start;
        comps.authority_end;
        comps.queries_start;
        comps.fragment_start;
        comps.uri_end;
    };

    /// Href Components are components that store the `.href()` strings directly, and may or may not include the
    /// components as string views that point to that main href string.
    template <typename T>
    concept URIHrefComponents = URIComponents<T> && requires(stl::remove_cvref_t<T> comps) { comps.href; };

    /// Structured Components are components that store each URI's components separately.
    template <typename T, typename U = stl::remove_cvref_t<T>>
    concept URIStructuredComponents = URIComponents<T> && requires(U& comps) {
        typename U::string_type;
        typename U::vec_type;
        typename U::map_type;
        { comps.scheme } -> stl::same_as<typename U::string_type&>;
        { comps.username } -> stl::same_as<typename U::string_type&>;
        { comps.password } -> stl::same_as<typename U::string_type&>;
        { comps.hostname } -> stl::same_as<typename U::string_type&>;
        requires stl::same_as<decltype((comps.port)), typename U::string_type&> ||
                   stl::same_as<decltype((comps.port)), stl::uint16_t&>;
        { comps.path } -> stl::same_as<typename U::vec_type&>;
        { comps.queries } -> stl::same_as<typename U::map_type&>;
        { comps.fragment } -> stl::same_as<typename U::string_type&>;
    };

    /// Owning Components are components that are using strings and not string views.
    template <typename T, typename U = stl::remove_cvref_t<T>>
    concept URIOwningComponents = URIComponents<T> && requires(U& comps) {
        requires istl::String<typename U::string_type>;
        { comps.scheme } -> stl::same_as<typename U::string_type&>;
        { comps.username } -> stl::same_as<typename U::string_type&>;
        { comps.password } -> stl::same_as<typename U::string_type&>;
        { comps.hostname } -> stl::same_as<typename U::string_type&>;
        requires stl::same_as<decltype((comps.port)), typename U::string_type&> ||
                   stl::same_as<decltype((comps.port)), stl::uint16_t&>;
        { comps.path } -> stl::same_as<typename U::string_type&>;
        { comps.queries } -> stl::same_as<typename U::string_type&>;
        { comps.fragment } -> stl::same_as<typename U::string_type&>;
    };

    template <typename CompT, auto MemberPtr, typename... Args>
    [[nodiscard]] consteval bool is_component_assignable() noexcept {
        return URIOwningComponents<CompT> && (requires(CompT& comps) {
                   requires stl::is_nothrow_assignable_v<stl::remove_cvref_t<decltype(comps.*MemberPtr)>, Args...>;
               });
    }

    template <typename CompT>
    concept PortNumberAssignable = is_component_assignable<CompT, &CompT::port, stl::uint16_t>();


    template <typename T>
    concept URIModifiableComponents =
      URIComponents<T> && requires { requires istl::String<typename stl::remove_cvref_t<T>::string_type>; };

    /**
     * An output type that is like a vector or a map
     */
    template <typename T>
    concept SegregatedOutput =
      (istl::LinearContainer<T> && !istl::String<T>) || requires { requires T::is_segregated; };

    /// Path can be a vector
    template <typename T>
    concept VectorOutput = istl::LinearContainer<T> && !istl::MapContainer<T> && !istl::String<T>;

    //////////////////////////////////////// ////////////// ////////////////////////////////////////
    //////////////////////////////////////// URI Components ////////////////////////////////////////
    //////////////////////////////////////// ////////////// ////////////////////////////////////////

    template <typename Iter>
    struct [[nodiscard]] segment {
        Iter beg;
        Iter end;
    };

    template <typename Iter>
    [[nodiscard]] static constexpr Iter begin(segment<Iter> const& seg) noexcept {
        return seg.beg;
    }

    template <typename Iter>
    [[nodiscard]] static constexpr Iter end(segment<Iter> const& seg) noexcept {
        return seg.end;
    }

    /**
     * URL Components
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
    template <typename CharT, typename AllocT = stl::allocator<CharT>>
    struct [[nodiscard]] uri_components_u32 {
        using seg_type              = stl::uint32_t; // maximum size of uint32_t is 4GiB of URL
        using string_allocator_type = typename stl::allocator_traits<AllocT>::template rebind_alloc<CharT>;
        using string_type           = stl::basic_string<CharT, stl::char_traits<CharT>, string_allocator_type>;

        static constexpr seg_type omitted              = stl::numeric_limits<seg_type>::max();
        static constexpr auto     max_supported_length = stl::numeric_limits<seg_type>::max() - 1;
        static constexpr bool     is_nothrow           = true;
        static constexpr bool     is_modifiable        = false;
        static constexpr bool     is_segregated        = false;

        // Source:
        string_type href;

        // Components:
        seg_type scheme_end      = omitted; // excluding :// stuff
        seg_type authority_start = omitted; // username/host start
        seg_type password_start  = omitted;
        seg_type host_start      = omitted;
        seg_type port_start      = omitted;
        seg_type authority_end   = omitted; // path start
        seg_type queries_start   = omitted; // path end
        seg_type fragment_start  = omitted; // query end
        seg_type uri_end         = omitted; // string end
    };

    /**
     * Non-Owning completely
     */
    template <istl::CharType CharT = char32_t>
    struct [[nodiscard]] uri_components_u32_view {
        using seg_type         = stl::uint32_t; // maximum size of uint32_t is 4GiB of URL
        using string_view_type = stl::basic_string_view<CharT>;
        using iterator         = string_view_type::iterator;

        /// maximum number that this url component class supports
        static constexpr auto max_supported_length = stl::numeric_limits<seg_type>::max() - 1;

        /// resetting the values of integer types are always noexcept
        static constexpr bool     is_nothrow    = true;
        static constexpr bool     is_modifiable = false;
        static constexpr bool     is_segregated = false;
        static constexpr seg_type omitted       = stl::numeric_limits<seg_type>::max();

        iterator uri_beg;

        seg_type scheme_end      = omitted; // excluding :// stuff
        seg_type authority_start = omitted; // username/host start
        seg_type password_start  = omitted;
        seg_type host_start      = omitted;
        seg_type port_start      = omitted;
        seg_type authority_end   = omitted; // path start
        seg_type queries_start   = omitted; // path end
        seg_type fragment_start  = omitted; // query end
        seg_type uri_end         = omitted; // string end
    };

    /**
     * String View based, but still structured enough
     */
    template <istl::CharType CharT = char32_t>
    struct [[nodiscard]] uri_components_view {
        using string_type  = stl::basic_string_view<CharT>;
        using seg_type     = string_type;
        using char_type    = typename string_type::value_type;
        using size_type    = typename string_type::size_type;
        using vec_iterator = seg_type*;

        /// maximum number that this url component class supports
        static constexpr auto max_supported_length = stl::numeric_limits<size_type>::max() - 1;

        /// is resetting the values are noexcept or not
        static constexpr bool is_nothrow    = stl::is_nothrow_assignable_v<string_type, char_type const*>;
        static constexpr bool is_modifiable = istl::ModifiableString<string_type>;
        static constexpr bool is_segregated = false;

        string_type scheme;
        string_type username;
        string_type password;
        string_type hostname;
        string_type port;
        string_type path;
        string_type queries;
        string_type fragment;
    };

    /**
     * String-Based, owning URI Components
     */
    template <istl::CharType CharT = char32_t, typename AllocT = stl::allocator<CharT>>
    struct [[nodiscard]] uri_components_owning {
        using string_allocator_type = typename stl::allocator_traits<AllocT>::template rebind_alloc<CharT>;
        using string_type           = stl::basic_string<CharT, stl::char_traits<CharT>, string_allocator_type>;
        using allocator_type        = typename string_type::allocator_type;
        using seg_type              = string_type;
        using char_type             = typename string_type::value_type;
        using size_type             = typename string_type::size_type;

        /// maximum number that this url component class supports
        static constexpr auto max_supported_length = stl::numeric_limits<size_type>::max() - 1;
        static constexpr bool is_nothrow           = false;
        static constexpr bool is_modifiable        = true;
        static constexpr bool is_segregated        = false;

        string_type scheme;
        string_type username;
        string_type password;
        string_type hostname;
        string_type port;
        string_type path;
        string_type queries;
        string_type fragment;
    };

    /**
     * Single-Source based URI Components.
     * Let's have one single href, and have components as string views pointing to that source
     */
    template <istl::CharType CharT = char32_t, typename AllocT = stl::allocator<CharT>>
    struct [[nodiscard]] uri_components_href {
        using string_allocator_type = typename stl::allocator_traits<AllocT>::template rebind_alloc<CharT>;
        using string_type           = stl::basic_string<CharT, stl::char_traits<CharT>, string_allocator_type>;
        using string_view_type      = stl::basic_string_view<CharT>;
        using char_type             = typename string_type::value_type;
        using size_type             = typename string_type::size_type;

        static constexpr auto max_supported_length = stl::numeric_limits<size_type>::max() - 1;
        static constexpr bool is_nothrow           = false;
        static constexpr bool is_modifiable        = true;
        static constexpr bool is_segregated        = false;

        // Source:
        string_type href;

        // Components:
        string_view_type scheme;
        string_view_type username;
        string_view_type password;
        string_view_type hostname;
        string_view_type port;
        string_view_type path;
        string_view_type queries;
        string_view_type fragment;
    };

    /**
     * URI Components fully separated.
     *   - Domains are split into its subdomains.
     *   - Path are split into its segments.
     *   - Queries are mapped (or rather vector of pairs).
     *   - Strings own their data.
     */
    template <istl::CharType CharT = char32_t, typename AllocT = stl::allocator<CharT>>
    struct [[nodiscard]] uri_components_structured {
        using char_type             = CharT;
        using string_allocator_type = typename stl::allocator_traits<AllocT>::template rebind_alloc<CharT>;
        using string_type           = stl::basic_string<char_type, stl::char_traits<CharT>, string_allocator_type>;
        using size_type             = typename string_type::size_type;
        using vec_type              = stl::vector<string_type, AllocT>;
        using pair_type             = stl::pair<string_type const, string_type>;
        using map_type =
          stl::vector<pair_type, typename stl::allocator_traits<AllocT>::template rebind_alloc<pair_type>>;

        static constexpr auto max_supported_length = stl::numeric_limits<size_type>::max() - 1;
        static constexpr bool is_nothrow           = false;
        static constexpr bool is_modifiable        = true;
        static constexpr bool is_segregated        = true;

        string_type scheme;
        string_type username;
        string_type password;
        string_type hostname;
        string_type port;
        vec_type    path;
        map_type    queries;
        string_type fragment;
    };

    //////////////////////////////////////// ///////////////// ////////////////////////////////////////
    //////////////////////////////////////// Create Components ////////////////////////////////////////
    //////////////////////////////////////// ///////////////// ////////////////////////////////////////

    template <typename CompT>
    using comp_iter = typename stl::basic_string_view<typename CompT::char_type>::iterator;

    template <URIComponents CompT>
        requires requires { typename CompT::allocator_type; }
    static constexpr CompT create(
      stl::type_identity<CompT>,
      [[maybe_unused]] comp_iter<CompT> beg,
      [[maybe_unused]] comp_iter<CompT> end,
      allocator_type_of<CompT>          alloc = {}) noexcept(CompT::is_nothrow) {
        using seg_type = typename CompT::seg_type;
        return CompT{
          .scheme   = seg_type{alloc},
          .username = seg_type{alloc},
          .password = seg_type{alloc},
          .hostname = seg_type{alloc},
          .port     = seg_type{alloc},
          .path     = seg_type{alloc},
          .queries  = seg_type{alloc},
          .fragment = seg_type{alloc},
        };
    }

    template <URIComponents CompT>
    static constexpr CompT create(
      stl::type_identity<CompT>,
      [[maybe_unused]] comp_iter<CompT>         beg,
      [[maybe_unused]] comp_iter<CompT>         end,
      [[maybe_unused]] allocator_type_of<CompT> alloc = {}) noexcept(CompT::is_nothrow) {
        using seg_type = typename CompT::seg_type;
        return CompT{
          .scheme   = seg_type{},
          .username = seg_type{},
          .password = seg_type{},
          .hostname = seg_type{},
          .port     = seg_type{},
          .path     = seg_type{},
          .queries  = seg_type{},
          .fragment = seg_type{},
        };
    }

    template <istl::CharType CharT>
    static constexpr uri_components_u32_view<CharT> create(
      stl::type_identity<uri_components_u32_view<CharT>>,
      typename uri_components_u32_view<CharT>::iterator                  beg,
      [[maybe_unused]] typename uri_components_u32_view<CharT>::iterator end,
      [[maybe_unused]] allocator_type_of<uri_components_u32_view<CharT>> alloc = {}) noexcept {
        return {
          .uri_beg = beg,
        };
    }

    template <URIHrefComponents CompT>
    static constexpr CompT create(
      stl::type_identity<CompT>,
      [[maybe_unused]] comp_iter<CompT> beg,
      [[maybe_unused]] comp_iter<CompT> end,
      allocator_type_of<CompT>          alloc = {}) noexcept {
        using component_type = CompT;
        using string_type    = typename component_type::string_type;
        return {.href = string_type{alloc}};
    }

    template <URIComponents CompT>
    [[nodiscard]] static constexpr decltype(auto) get_allocator(CompT const& comps) noexcept {
        if constexpr (requires { comps.href.get_allocator(); }) {
            return comps.href.get_allocator();
        } else if constexpr (requires { comps.scheme.get_allocator(); }) {
            return comps.scheme.get_allocator();
        } else {
            static_assert_false(CompT, "This component don't have allocator");
        }
    }

    [[nodiscard]] static constexpr stl::size_t length(URIRelativeComponents auto const& components) noexcept {
        assert(components.uri_beg <= components.uri_end);
        return static_cast<stl::size_t>(stl::distance(components.uri_beg, components.uri_end));
    }

    [[nodiscard]] static constexpr stl::size_t length(URIHrefComponents auto const& components) noexcept {
        return components.href.size();
    }

    // [[nodiscard]] constexpr stl::size_t length(URIComponents auto const& components) noexcept {
    //     // todo: optimize this:
    //     return scheme(components).size() + username(components).size() + password(components).size() +
    //            hostname(components).size() + port(components).size() + path(components).size() +
    //            queries(components).size() + fragment(components).size();
    // }

    //////////////////////////////////////// /////////////////// ////////////////////////////////////////
    //////////////////////////////////////// Relative Components ////////////////////////////////////////
    //////////////////////////////////////// /////////////////// ////////////////////////////////////////

    template <URIRelativeComponents CompT>
    static constexpr void set_min_authority_end(CompT& comps, typename CompT::seg_type const end) noexcept {
        comps.authority_end = comps.authority_end != CompT::omitted ? stl::max(comps.authority_end, end) : end;
    }

    template <URIRelativeComponents CompT>
    static constexpr void set_min_uri_end(CompT& comps, typename CompT::seg_type const end) noexcept {
        comps.uri_end = comps.uri_end != CompT::omitted ? stl::max(comps.uri_end, end) : end;
    }

    template <URIRelativeComponents CompT>
    static constexpr void set_scheme(CompT& comps, segment<comp_iter<CompT>> seg) noexcept {
        comps.uri_beg    = seg.beg;
        comps.scheme_end = static_cast<typename CompT::seg_type>(seg.end - seg.beg);
        set_min_uri_end(comps.scheme_end);
    }

    template <URIRelativeComponents CompT>
    static constexpr void set_path(CompT& comps, segment<typename CompT::seg_type> const seg) noexcept {
        comps.authority_end = seg.beg;
        set_min_uri_end(seg.end);
    }

    template <URIRelativeComponents CompT>
    static constexpr void set_path(CompT& comps, segment<comp_iter<CompT>> const seg) noexcept {
        set_path(comps,
                 static_cast<typename CompT::seg_type>(seg.beg - comps.uri_beg),
                 static_cast<typename CompT::seg_type>(seg.end - comps.uri_beg));
    }

    template <URIRelativeComponents CompT>
    static constexpr void set_username(CompT& comps, segment<typename CompT::seg_type> const seg) noexcept {
        comps.authority_start = seg.beg;
        set_min_authority_end(comps, seg.end);
        set_min_uri_end(comps, seg.end);
    }

    template <URIRelativeComponents CompT>
    static constexpr void set_username(CompT& comps, segment<comp_iter<CompT>> const seg) noexcept {
        set_username(comps,
                     static_cast<typename CompT::seg_type>(seg.beg - comps.uri_beg),
                     static_cast<typename CompT::seg_type>(seg.end - comps.uri_beg));
    }

    template <URIRelativeComponents CompT>
    static constexpr void set_password(CompT& comps, segment<typename CompT::seg_type> const seg) noexcept {
        comps.password_start = seg.beg;
        set_min_authority_end(comps, seg.end);
        set_min_uri_end(comps, seg.end);
    }

    template <URIRelativeComponents CompT>
    static constexpr void set_password(CompT& comps, segment<comp_iter<CompT>> const seg) noexcept {
        set_password(comps,
                     static_cast<typename CompT::seg_type>(seg.beg - comps.uri_beg),
                     static_cast<typename CompT::seg_type>(seg.end - comps.uri_beg));
    }

    template <URIRelativeComponents CompT>
    static constexpr void set_hostname(CompT& comps, segment<typename CompT::seg_type> const seg) noexcept {
        comps.host_start = seg.beg;
        set_min_authority_end(comps, seg.end);
        set_min_uri_end(comps, seg.end);
    }

    template <URIRelativeComponents CompT>
    static constexpr void set_hostname(CompT& comps, segment<comp_iter<CompT>> const seg) noexcept {
        set_hostname(comps,
                     static_cast<typename CompT::seg_type>(seg.beg - comps.uri_beg),
                     static_cast<typename CompT::seg_type>(seg.end - comps.uri_beg));
    }

    template <URIRelativeComponents CompT>
    static constexpr void set_port(CompT& comps, segment<typename CompT::seg_type> const seg) noexcept {
        comps.port_start = seg.beg;
        set_min_authority_end(comps, seg.end);
        set_min_uri_end(comps, seg.end);
    }

    template <URIRelativeComponents CompT>
    static constexpr void set_port(CompT& comps, segment<comp_iter<CompT>> const seg) noexcept {
        set_port(comps,
                 static_cast<typename CompT::seg_type>(seg.beg - comps.uri_beg),
                 static_cast<typename CompT::seg_type>(seg.end - comps.uri_beg));
    }

    template <URIRelativeComponents CompT>
    static constexpr void set_queries(CompT& comps, segment<typename CompT::seg_type> const seg) noexcept {
        comps.queries_start = seg.beg;
        set_min_uri_end(comps, seg.end);
    }

    template <URIRelativeComponents CompT>
    static constexpr void set_queries(CompT& comps, segment<comp_iter<CompT>> const seg) noexcept {
        set_queries(comps,
                    static_cast<typename CompT::seg_type>(seg.beg - comps.uri_beg),
                    static_cast<typename CompT::seg_type>(seg.end - comps.uri_beg));
    }

    template <URIRelativeComponents CompT>
    static constexpr void set_fragment(CompT& comps, segment<typename CompT::seg_type> const seg) noexcept {
        comps.fragment_start = seg.beg;
        set_min_uri_end(comps, seg.end);
    }

    template <URIRelativeComponents CompT>
    static constexpr void set_fragment(CompT& comps, segment<comp_iter<CompT>> const seg) noexcept {
        set_fragment(comps,
                     static_cast<typename CompT::seg_type>(seg.beg - comps.uri_beg),
                     static_cast<typename CompT::seg_type>(seg.end - comps.uri_beg));
    }

    template <URIRelativeComponents CompT>
    static constexpr void clean_authority_end(CompT& comps) noexcept {
        bool test  = comps.authority_start == CompT::omitted;
        test      &= comps.password_start == CompT::omitted;
        test      &= comps.port_start == CompT::omitted;
        test      &= comps.host_start == CompT::omitted;
        if (test) {
            comps.authority_end = CompT::omitted;
        }
    }

    template <URIRelativeComponents CompT>
    static constexpr void clear_scheme(CompT& comps) noexcept {
        comps.scheme_end = CompT::omitted;
    }

    template <URIRelativeComponents CompT>
    static constexpr void clear_hostname(CompT& comps) noexcept {
        comps.host_start = CompT::omitted;
        clean_authority_end(comps);
    }

    template <URIRelativeComponents CompT>
    static constexpr void clear_port(CompT& comps) noexcept {
        comps.port_start = CompT::omitted;
        clean_authority_end(comps);
    }

    template <URIRelativeComponents CompT>
    static constexpr void clear_username(CompT& comps) noexcept {
        comps.authority_start = CompT::omitted;
        clean_authority_end(comps);
    }

    template <URIRelativeComponents CompT>
    static constexpr void clear_password(CompT& comps) noexcept {
        comps.password_start = CompT::omitted;
        clean_authority_end(comps);
    }

    template <URIRelativeComponents CompT>
    static constexpr void clear_path(CompT& comps) noexcept {
        if (comps.queries_start == CompT::omitted && comps.fragment_start == CompT::omitted) {
            comps.uri_end = stl::min(comps.authority_end, comps.uri_end);
        }
        comps.authority_end = stl::min(comps.authority_end, comps.uri_end);
    }

    template <URIRelativeComponents CompT>
    static constexpr void clear_queries(CompT& comps) noexcept {
        if (comps.fragment_start == CompT::omitted) {
            comps.uri_end = stl::min(comps.fragment_start, comps.uri_end);
        }
        comps.queries_start = CompT::omitted;
    }

    template <URIRelativeComponents CompT>
    static constexpr void clear_fragment(CompT& comps) noexcept {
        comps.uri_end        = stl::min(comps.fragment_start, comps.uri_end);
        comps.fragment_start = CompT::omitted;
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] static constexpr bool has_scheme(CompT const& comps) noexcept {
        return comps.scheme_end != CompT::omitted;
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] static constexpr bool has_username(CompT const& comps) noexcept {
        return comps.authority_start != CompT::omitted;
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] static constexpr bool has_password(CompT const& comps) noexcept {
        return comps.password_start != CompT::omitted;
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] static constexpr bool has_hostname(CompT const& comps) noexcept {
        return comps.host_start != CompT::omitted;
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] static constexpr bool has_credentials(CompT const& comps) noexcept {
        return comps.authority_start != CompT::omitted;
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] static constexpr bool has_port(CompT const& comps) noexcept {
        return comps.port_start != CompT::omitted;
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] static constexpr bool has_path(CompT const& comps) noexcept {
        return comps.authority_end != CompT::omitted;
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] static constexpr bool has_queries(CompT const& comps) noexcept {
        return comps.queries_start != CompT::omitted;
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] static constexpr bool has_fragment(CompT const& comps) noexcept {
        return comps.fragment_start != CompT::omitted;
    }

    template <typename CharT>
    [[nodiscard]] static constexpr stl::basic_string_view<CharT> view(
      [[maybe_unused]] uri_components_u32_view<CharT> const& comps) noexcept {
        return {};
    }

    template <typename CharT>
    [[nodiscard]] static constexpr stl::basic_string_view<CharT> view(
      uri_components_u32_view<CharT> const&                   comps,
      typename uri_components_u32_view<CharT>::seg_type const pos,
      typename uri_components_u32_view<CharT>::seg_type const length) noexcept {
        return stl::basic_string_view<CharT>{stl::next(comps.beg, pos), length};
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] static constexpr typename CompT::seg_type size(CompT const& comps) noexcept {
        return comps.uri_end == CompT::omitted ? 0 : comps.uri_end;
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] static constexpr auto scheme(CompT const& comps) noexcept(CompT::is_nothrow) {
        if (comps.scheme_end == CompT::omitted) {
            return view(comps);
        }
        return view(comps, 0, stl::min(comps.scheme_end, size(comps)));
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] static constexpr auto username(CompT const& comps) noexcept(CompT::is_nothrow) {
        if (comps.authority_start == CompT::omitted) {
            return view(comps);
        }
        return view(comps,
                    comps.authority_start,
                    stl::min(comps.password_start - 1, comps.host_start - 1) - comps.authority_start);
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] static constexpr auto password(CompT const& comps) noexcept(CompT::is_nothrow) {
        if (comps.password_start == CompT::omitted) {
            return view(comps);
        }
        return view(
          comps,
          comps.password_start,
          stl::min(stl::min(comps.host_start, comps.port_start), comps.authority_end) - 1 - comps.password_start);
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] static constexpr auto hostname(CompT const& comps) noexcept(CompT::is_nothrow) {
        if (comps.host_start == CompT::omitted) {
            return view(comps);
        }
        return view(comps,
                    comps.host_start,
                    stl::min(stl::min(comps.port_start - 1, comps.authority_end), size(comps)) - comps.host_start);
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] static constexpr auto port(CompT const& comps) noexcept(CompT::is_nothrow) {
        if (comps.port_start == CompT::omitted) {
            return view(comps);
        }
        return view(comps, comps.port_start, stl::min(comps.authority_end, size(comps)) - comps.port_start);
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] static constexpr auto authority(CompT const& comps) noexcept(CompT::is_nothrow) {
        if (comps.authority_start == CompT::omitted || comps.authority_end == CompT::omitted) {
            return view(comps);
        }
        return view(comps, comps.authority_start, comps.authority_end - comps.authority_start);
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] static constexpr auto path(CompT const& comps) noexcept(CompT::is_nothrow) {
        if (comps.authority_end == CompT::omitted) {
            return view(comps);
        }
        return view(
          comps,
          comps.authority_end,
          stl::min(stl::min(comps.queries_start - 1, comps.fragment_start - 1), size(comps)) - comps.authority_end);
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] static constexpr auto queries(CompT const& comps) noexcept(CompT::is_nothrow) {
        if (comps.queries_start == CompT::omitted) {
            return view(comps);
        }
        return view(comps, comps.queries_start, stl::min(comps.fragment_start - 1, size(comps)) - comps.queries_start);
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] static constexpr auto fragment(CompT const& comps) noexcept(CompT::is_nothrow) {
        if (comps.fragment_start == CompT::omitted) {
            return view(comps);
        }
        return view(comps, comps.fragment_start, comps.uri_end - comps.fragment_start);
    }

    //////////////////////////////////////// ////////////////// ////////////////////////////////////////
    //////////////////////////////////////// General Components ////////////////////////////////////////
    //////////////////////////////////////// ////////////////// ////////////////////////////////////////

    // Helper function to get a string_view from a string-like object
    template <typename StringType>
    [[nodiscard]] static constexpr decltype(auto) make_view(StringType&& str) noexcept {
        using T = stl::remove_cvref_t<StringType>;
        if constexpr (istl::StringView<T>) {
            return str;
        } else {
            return stl::basic_string_view<typename T::value_type>{stl::forward<StringType>(str)};
        }
    }

    template <URIComponents CompT>
    [[nodiscard]] static constexpr decltype(auto) scheme(CompT&& comps) noexcept {
        return make_view(stl::forward<CompT>(comps).scheme);
    }

    template <URIOwningComponents CompT>
    static constexpr void set_scheme(CompT& comps, typename CompT::string_type&& value) noexcept(CompT::is_nothrow) {
        comps.scheme = stl::move(value);
    }

    template <URIComponents CompT>
    [[nodiscard]] static constexpr decltype(auto) username(CompT&& comp) noexcept {
        return make_view(stl::forward<CompT>(comp).username);
    }

    template <URIOwningComponents CompT>
    static constexpr void set_username(CompT& comps, typename CompT::string_type&& value) noexcept(CompT::is_nothrow) {
        comps.username = stl::move(value);
    }

    template <URIComponents CompT>
    [[nodiscard]] static constexpr decltype(auto) password(CompT&& comp) noexcept {
        return make_view(stl::forward<CompT>(comp).password);
    }

    template <URIOwningComponents CompT>
    static constexpr void set_password(CompT& comps, typename CompT::string_type&& value) noexcept(CompT::is_nothrow) {
        comps.password = stl::move(value);
    }

    template <URIComponents CompT>
    [[nodiscard]] static constexpr decltype(auto) hostname(CompT&& comp) noexcept {
        return make_view(stl::forward<CompT>(comp).hostname);
    }

    // For href-based components, the authority is the substring from the start of the first authority
    // component to the end of the last authority component
    template <URIHrefComponents CompT>
    [[nodiscard]] static constexpr auto authority(CompT const& comp) noexcept {
        using string_view_type = typename CompT::string_view_type;

        // If there's no hostname, there's no authority
        if (comp.hostname.empty()) {
            return string_view_type{};
        }

        // Find the start of the authority (earliest of username, password, hostname)
        auto const& href_str  = comp.href;
        auto const  href_view = string_view_type{href_str};

        auto start_ptr = comp.hostname.data();
        auto end_ptr   = start_ptr + comp.hostname.length();

        // Check if username exists and comes earlier
        if (!comp.username.empty() && comp.username.data() < start_ptr) {
            start_ptr = comp.username.data();
        }

        // Check if password exists and comes earlier or later
        if (!comp.password.empty()) {
            if (comp.password.data() < start_ptr) {
                start_ptr = comp.password.data();
            }
            // Extend end if password goes further
            auto const password_end = comp.password.data() + comp.password.length();
            if (password_end > end_ptr) {
                end_ptr = password_end;
            }
        }

        // Extend end if hostname goes further
        auto const hostname_end = comp.hostname.data() + comp.hostname.length();
        if (hostname_end > end_ptr) {
            end_ptr = hostname_end;
        }

        // Extend end if port exists and goes further
        if (!comp.port.empty()) {
            auto const port_end = comp.port.data() + comp.port.length();
            if (port_end > end_ptr) {
                end_ptr = port_end;
            }
        }

        // Calculate the offset and length
        auto const start_offset = start_ptr - href_view.data();
        auto const length       = end_ptr - start_ptr;

        return href_view.substr(start_offset, length);
    }

    template <URIOwningComponents CompT>
    static constexpr void set_hostname(CompT& comps, typename CompT::string_type&& value) noexcept(CompT::is_nothrow) {
        comps.hostname = stl::move(value);
    }

    template <URIComponents CompT>
    [[nodiscard]] static constexpr decltype(auto) port(CompT&& comp) noexcept {
        return make_view(stl::forward<CompT>(comp).port);
    }

    template <URIOwningComponents CompT>
    static constexpr void set_port(CompT& comps, typename CompT::string_type&& value) noexcept(CompT::is_nothrow) {
        comps.port = stl::move(value);
    }

    template <URIComponents CompT, typename Iter>
    static constexpr void set_port(CompT& comp, Iter beg, Iter end) noexcept(CompT::is_nothrow) {
        set_port(comp, typename CompT::string_type{beg, end});
    }

    template <PortNumberAssignable CompT>
    static constexpr void set_port(CompT& comps, stl::uint16_t const port_value) noexcept {
        comps.port = port_value;
    }

    template <URIComponents CompT>
    [[nodiscard]] static constexpr decltype(auto) queries(CompT&& comp) noexcept {
        return make_view(stl::forward<CompT>(comp).queries);
    }

    template <URIOwningComponents CompT>
    static constexpr void set_queries(CompT& comps, typename CompT::string_type&& value) noexcept(CompT::is_nothrow) {
        comps.queries = stl::move(value);
    }

    template <URIComponents CompT>
    [[nodiscard]] static constexpr decltype(auto) path(CompT&& comp) noexcept {
        return make_view(stl::forward<CompT>(comp).path);
    }

    template <URIOwningComponents CompT>
    static constexpr void set_path(CompT& comps, typename CompT::string_type&& value) noexcept(CompT::is_nothrow) {
        comps.path = stl::move(value);
    }

    template <URIComponents CompT>
    [[nodiscard]] static constexpr decltype(auto) fragment(CompT&& comp) noexcept {
        return make_view(stl::forward<CompT>(comp).fragment);
    }

    template <URIOwningComponents CompT>
    static constexpr void set_fragment(CompT& comps, typename CompT::string_type&& value) noexcept(CompT::is_nothrow) {
        comps.fragment = stl::move(value);
    }

    static constexpr void clear_scheme(URIComponents auto& comp) noexcept {
        istl::clear(comp.scheme);
    }

    static constexpr void clear_username(URIComponents auto& comp) noexcept {
        istl::clear(comp.username);
    }

    static constexpr void clear_password(URIComponents auto& comp) noexcept {
        istl::clear(comp.password);
    }

    static constexpr void clear_hostname(URIComponents auto& comp) noexcept {
        istl::clear(comp.hostname);
    }

    static constexpr void clear_port(URIComponents auto& comp) noexcept {
        istl::clear(comp.port);
    }

    static constexpr void clear_path(URIComponents auto& comp) noexcept {
        istl::clear(comp.path);
    }

    static constexpr void clear_queries(URIComponents auto& comp) noexcept {
        istl::clear(comp.queries);
    }

    static constexpr void clear_fragment(URIComponents auto& comp) noexcept {
        istl::clear(comp.fragment);
    }

    [[nodiscard]] static constexpr bool has_scheme(URIComponents auto const& comp) noexcept {
        return !scheme(comp).empty();
    }

    [[nodiscard]] static constexpr bool has_username(URIComponents auto const& comp) noexcept {
        return !username(comp).empty();
    }

    [[nodiscard]] static constexpr bool has_password(URIComponents auto const& components) noexcept {
        return !password(components).empty();
    }

    [[nodiscard]] static constexpr bool has_credentials(URIComponents auto const& components) noexcept {
        return has_username(components) || has_password(components);
    }

    [[nodiscard]] static constexpr bool has_hostname(URIComponents auto const& comp) noexcept {
        return !hostname(comp).empty();
    }

    [[nodiscard]] static constexpr bool has_port(URIComponents auto const& comp) noexcept {
        return !port(comp).empty();
    }

    [[nodiscard]] static constexpr bool has_path(URIComponents auto const& comp) noexcept {
        return !path(comp).empty();
    }

    [[nodiscard]] static constexpr bool has_queries(URIComponents auto const& comp) noexcept {
        return !queries(comp).empty();
    }

    [[nodiscard]] static constexpr bool has_fragment(URIComponents auto const& comp) noexcept {
        return !fragment(comp).empty();
    }


} // namespace webpp::uri

#endif // WEBPP_URL_COMPONENTS_HPP
