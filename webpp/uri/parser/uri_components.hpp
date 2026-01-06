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
    template <typename T>
    concept URIComponents = requires(T comps) {
        T::is_nothrow;
        T::is_modifiable;
        T::is_segregated;
        T::max_supported_length;
    };

    template <typename T>
    concept URIRelativeComponents = URIComponents<T> && requires(T comps) {
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

    template <typename T>
    concept URIStructuredComponents = URIComponents<T> && requires(T comps) {
        typename T::string_type;
        typename T::vec_type;
        typename T::map_type;
        { comps.scheme } -> stl::same_as<typename T::string_type>;
        { comps.username } -> stl::same_as<typename T::string_type>;
        { comps.password } -> stl::same_as<typename T::string_type>;
        { comps.hostname } -> stl::same_as<typename T::string_type>;
        { comps.port } -> stl::same_as<typename T::string_type>;
        { comps.path } -> stl::same_as<typename T::vec_type>;
        { comps.queries } -> stl::same_as<typename T::map_type>;
        { comps.fragment } -> stl::same_as<typename T::string_type>;
    };

    /**
     * An output type that is like a vector or a map
     */
    template <typename T>
    concept SegregatedOutput =
      (istl::LinearContainer<T> && !istl::String<T>) || requires { requires T::is_segregated; };

    /// Path can be a vector
    template <typename T>
    concept VectorOutput = istl::LinearContainer<T> && !istl::MapContainer<T> && !istl::String<T>;

    /**
     * This is the output type that the URI parser will be able to put the results of components into.
     */
    template <typename T>
    concept ParsingOutput = istl::StringLike<T> || SegregatedOutput<T> || istl::cvref_as<T, istl::nothing_type>;

    //////////////////////////////////////// ////////////// ////////////////////////////////////////
    //////////////////////////////////////// URI Components ////////////////////////////////////////
    //////////////////////////////////////// ////////////// ////////////////////////////////////////

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
    struct uri_components_u32 {
        using seg_type    = stl::uint32_t; // maximum size of uint32_t is 4GiB of URL
        using string_type = stl::basic_string<CharT, AllocT>;

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
    template <typename CharT = char32_t>
    struct uri_components_u32_view {
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
    template <typename CharT = char32_t>
    struct uri_components_view {
        using string_type  = stl::basic_string_view<CharT>;
        using iterator     = typename string_type::iterator;
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
    template <typename CharT = char32_t, typename AllocT = stl::allocator<CharT>>
    struct uri_components_owning {
        using string_type  = stl::basic_string<CharT, AllocT>;
        using iterator     = typename string_type::iterator;
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
     * Single-Source based URI Components.
     * Let's have one single href, and have components as string views pointing to that source
     */
    template <typename CharT = char32_t, typename AllocT = stl::allocator<CharT>>
    struct uri_components_href {
        using string_type      = stl::basic_string<CharT, AllocT>;
        using string_view_type = stl::basic_string_view<CharT>;
        using char_type        = typename string_type::value_type;
        using size_type        = typename string_type::size_type;

        static constexpr auto max_supported_length = stl::numeric_limits<size_type>::max() - 1;
        static constexpr bool is_nothrow           = stl::is_nothrow_assignable_v<string_type, char_type const*>;
        static constexpr bool is_modifiable        = istl::ModifiableString<string_type>;
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
    template <typename CharT = char32_t, typename AllocT = stl::allocator<CharT>>
    struct uri_components_structured {
        using char_type   = CharT;
        using string_type = stl::basic_string<char_type, AllocT>;
        using size_type   = typename string_type::size_type;
        using vec_type    = stl::vector<string_type, AllocT>;
        using pair_type   = stl::pair<string_type const, string_type>;
        using map_type =
          stl::vector<pair_type, typename stl::allocator_traits<AllocT>::template rebind_alloc<pair_type>>;

        static constexpr auto max_supported_length = stl::numeric_limits<size_type>::max() - 1;
        static constexpr bool is_nothrow           = false;
        static constexpr bool is_modifiable        = istl::ModifiableString<string_type>;
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

    //////////////////////////////////////// /////////////////// ////////////////////////////////////////
    //////////////////////////////////////// Relative Components ////////////////////////////////////////
    //////////////////////////////////////// /////////////////// ////////////////////////////////////////

    template <URIRelativeComponents CompT>
    constexpr void set_min_authority_end(CompT& comps, typename CompT::seg_type const end) noexcept {
        comps.authority_end = comps.authority_end != CompT::omitted ? stl::max(comps.authority_end, end) : end;
    }

    template <URIRelativeComponents CompT>
    constexpr void set_min_uri_end(CompT& comps, typename CompT::seg_type const end) noexcept {
        comps.uri_end = comps.uri_end != CompT::omitted ? stl::max(comps.uri_end, end) : end;
    }

    template <URIRelativeComponents CompT>
    constexpr void set_scheme(CompT& comps, typename CompT::iterator beg, typename CompT::iterator end) noexcept {
        comps.uri_beg    = beg;
        comps.scheme_end = static_cast<typename CompT::seg_type>(end - beg);
        set_min_uri_end(comps.scheme_end);
    }

    template <URIRelativeComponents CompT>
    constexpr void set_path(CompT& comps, typename CompT::seg_type beg, typename CompT::seg_type end) noexcept {
        comps.authority_end = beg;
        set_min_uri_end(end);
    }

    template <URIRelativeComponents CompT>
    constexpr void set_path(CompT& comps, typename CompT::iterator beg, typename CompT::iterator end) noexcept {
        set_path(comps,
                 static_cast<typename CompT::seg_type>(beg - comps.uri_beg),
                 static_cast<typename CompT::seg_type>(end - comps.uri_beg));
    }

    template <URIRelativeComponents CompT>
    constexpr void set_username(CompT& comps, typename CompT::seg_type beg, typename CompT::seg_type end) noexcept {
        comps.authority_start = beg;
        set_min_authority_end(comps, end);
        set_min_uri_end(comps, end);
    }

    template <URIRelativeComponents CompT>
    constexpr void set_username(CompT& comps, typename CompT::iterator beg, typename CompT::iterator end) noexcept {
        set_username(comps,
                     static_cast<typename CompT::seg_type>(beg - comps.uri_beg),
                     static_cast<typename CompT::seg_type>(end - comps.uri_beg));
    }

    template <URIRelativeComponents CompT>
    constexpr void set_password(CompT& comps, typename CompT::seg_type beg, typename CompT::seg_type end) noexcept {
        comps.password_start = beg;
        set_min_authority_end(comps, end);
        set_min_uri_end(comps, end);
    }

    template <URIRelativeComponents CompT>
    constexpr void set_password(CompT& comps, typename CompT::iterator beg, typename CompT::iterator end) noexcept {
        set_password(comps,
                     static_cast<typename CompT::seg_type>(beg - comps.uri_beg),
                     static_cast<typename CompT::seg_type>(end - comps.uri_beg));
    }

    template <URIRelativeComponents CompT>
    constexpr void set_hostname(CompT& comps, typename CompT::seg_type beg, typename CompT::seg_type end) noexcept {
        comps.host_start = beg;
        set_min_authority_end(comps, end);
        set_min_uri_end(comps, end);
    }

    template <URIRelativeComponents CompT>
    constexpr void set_hostname(CompT& comps, typename CompT::iterator beg, typename CompT::iterator end) noexcept {
        set_hostname(comps,
                     static_cast<typename CompT::seg_type>(beg - comps.uri_beg),
                     static_cast<typename CompT::seg_type>(end - comps.uri_beg));
    }

    template <URIRelativeComponents CompT>
    constexpr void set_port(CompT& comps, typename CompT::seg_type start, typename CompT::seg_type end) noexcept {
        comps.port_start = start;
        set_min_authority_end(comps, end);
        set_min_uri_end(comps, end);
    }

    template <URIRelativeComponents CompT>
    constexpr void set_port(CompT& comps, typename CompT::iterator beg, typename CompT::iterator end) noexcept {
        set_port(comps,
                 static_cast<typename CompT::seg_type>(beg - comps.uri_beg),
                 static_cast<typename CompT::seg_type>(end - comps.uri_beg));
    }

    template <URIRelativeComponents CompT>
    constexpr void set_queries(CompT& comps, typename CompT::seg_type start, typename CompT::seg_type end) noexcept {
        comps.queries_start = start;
        set_min_uri_end(comps, end);
    }

    template <URIRelativeComponents CompT>
    constexpr void set_queries(CompT& comps, typename CompT::iterator beg, typename CompT::iterator end) noexcept {
        set_queries(comps,
                    static_cast<typename CompT::seg_type>(beg - comps.uri_beg),
                    static_cast<typename CompT::seg_type>(end - comps.uri_beg));
    }

    template <URIRelativeComponents CompT>
    constexpr void set_fragment(CompT& comps, typename CompT::seg_type start, typename CompT::seg_type end) noexcept {
        comps.fragment_start = start;
        set_min_uri_end(comps, end);
    }

    template <URIRelativeComponents CompT>
    constexpr void set_fragment(CompT& comps, typename CompT::iterator beg, typename CompT::iterator end) noexcept {
        set_fragment(comps,
                     static_cast<typename CompT::seg_type>(beg - comps.uri_beg),
                     static_cast<typename CompT::seg_type>(end - comps.uri_beg));
    }

    template <URIRelativeComponents CompT>
    constexpr void clean_authority_end(CompT& comps) noexcept {
        bool test  = comps.authority_start == CompT::omitted;
        test      &= comps.password_start == CompT::omitted;
        test      &= comps.port_start == CompT::omitted;
        test      &= comps.host_start == CompT::omitted;
        if (test) {
            comps.authority_end = CompT::omitted;
        }
    }

    template <URIRelativeComponents CompT>
    constexpr void clear_scheme(CompT& comps) noexcept {
        comps.scheme_end = CompT::omitted;
    }

    template <URIRelativeComponents CompT>
    constexpr void clear_hostname(CompT& comps) noexcept {
        comps.host_start = CompT::omitted;
        clean_authority_end(comps);
    }

    template <URIRelativeComponents CompT>
    constexpr void clear_port(CompT& comps) noexcept {
        comps.port_start = CompT::omitted;
        clean_authority_end(comps);
    }

    template <URIRelativeComponents CompT>
    constexpr void clear_username(CompT& comps) noexcept {
        comps.authority_start = CompT::omitted;
        clean_authority_end(comps);
    }

    template <URIRelativeComponents CompT>
    constexpr void clear_password(CompT& comps) noexcept {
        comps.password_start = CompT::omitted;
        clean_authority_end(comps);
    }

    template <URIRelativeComponents CompT>
    constexpr void clear_path(CompT& comps) noexcept {
        if (comps.queries_start == CompT::omitted && comps.fragment_start == CompT::omitted) {
            comps.uri_end = stl::min(comps.authority_end, comps.uri_end);
        }
        comps.authority_end = stl::min(comps.authority_end, comps.uri_end);
    }

    template <URIRelativeComponents CompT>
    constexpr void clear_queries(CompT& comps) noexcept {
        if (comps.fragment_start == CompT::omitted) {
            comps.uri_end = stl::min(comps.fragment_start, comps.uri_end);
        }
        comps.queries_start = CompT::omitted;
    }

    template <URIRelativeComponents CompT>
    constexpr void clear_fragment(CompT& comps) noexcept {
        comps.uri_end        = stl::min(comps.fragment_start, comps.uri_end);
        comps.fragment_start = CompT::omitted;
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] constexpr bool has_scheme(CompT const& comps) noexcept {
        return comps.scheme_end != CompT::omitted;
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] constexpr bool has_username(CompT const& comps) noexcept {
        return comps.authority_start != CompT::omitted;
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] constexpr bool has_password(CompT const& comps) noexcept {
        return comps.password_start != CompT::omitted;
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] constexpr bool has_hostname(CompT const& comps) noexcept {
        return comps.host_start != CompT::omitted;
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] constexpr bool has_credentials(CompT const& comps) noexcept {
        return comps.authority_start != CompT::omitted;
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] constexpr bool has_port(CompT const& comps) noexcept {
        return comps.port_start != CompT::omitted;
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] constexpr bool has_path(CompT const& comps) noexcept {
        return comps.authority_end != CompT::omitted;
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] constexpr bool has_queries(CompT const& comps) noexcept {
        return comps.queries_start != CompT::omitted;
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] constexpr bool has_fragment(CompT const& comps) noexcept {
        return comps.fragment_start != CompT::omitted;
    }

    template <typename CharT>
    [[nodiscard]] constexpr stl::basic_string_view<CharT> view(
      [[maybe_unused]] uri_components_u32_view<CharT> const& comps) noexcept {
        return {};
    }

    template <typename CharT>
    [[nodiscard]] constexpr stl::basic_string_view<CharT> view(
      uri_components_u32_view<CharT> const&                   comps,
      typename uri_components_u32_view<CharT>::seg_type const pos,
      typename uri_components_u32_view<CharT>::seg_type const length) noexcept {
        return stl::basic_string_view<CharT>{stl::next(comps.beg, pos), length};
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] constexpr typename CompT::seg_type size(CompT const& comps) noexcept {
        return comps.uri_end == CompT::omitted ? 0 : comps.uri_end;
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] constexpr auto scheme(CompT const& comps) noexcept(CompT::is_nothrow) {
        if (comps.scheme_end == CompT::omitted) {
            return view(comps);
        }
        return view(comps, 0, stl::min(comps.scheme_end, size(comps)));
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] constexpr auto username(CompT const& comps) noexcept(CompT::is_nothrow) {
        if (comps.authority_start == CompT::omitted) {
            return view(comps);
        }
        return view(comps,
                    comps.authority_start,
                    stl::min(comps.password_start - 1, comps.host_start - 1) - comps.authority_start);
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] constexpr auto password(CompT const& comps) noexcept(CompT::is_nothrow) {
        if (comps.password_start == CompT::omitted) {
            return view(comps);
        }
        return view(
          comps,
          comps.password_start,
          stl::min(stl::min(comps.host_start, comps.port_start), comps.authority_end) - 1 - comps.password_start);
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] constexpr auto hostname(CompT const& comps) noexcept(CompT::is_nothrow) {
        if (comps.host_start == CompT::omitted) {
            return view(comps);
        }
        return view(comps,
                    comps.host_start,
                    stl::min(stl::min(comps.port_start - 1, comps.authority_end), size(comps)) - comps.host_start);
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] constexpr auto port(CompT const& comps) noexcept(CompT::is_nothrow) {
        if (comps.port_start == CompT::omitted) {
            return view(comps);
        }
        return view(comps, comps.port_start, stl::min(comps.authority_end, size(comps)) - comps.port_start);
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] constexpr auto path(CompT const& comps) noexcept(CompT::is_nothrow) {
        if (comps.authority_end == CompT::omitted) {
            return view(comps);
        }
        return view(
          comps,
          comps.authority_end,
          stl::min(stl::min(comps.queries_start - 1, comps.fragment_start - 1), size(comps)) - comps.authority_end);
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] constexpr auto queries(CompT const& comps) noexcept(CompT::is_nothrow) {
        if (comps.queries_start == CompT::omitted) {
            return view(comps);
        }
        return view(comps, comps.queries_start, stl::min(comps.fragment_start - 1, size(comps)) - comps.queries_start);
    }

    template <URIRelativeComponents CompT>
    [[nodiscard]] constexpr auto fragment(CompT const& comps) noexcept(CompT::is_nothrow) {
        if (comps.fragment_start == CompT::omitted) {
            return view(comps);
        }
        return view(comps, comps.fragment_start, comps.uri_end - comps.fragment_start);
    }

    //////////////////////////////////////// ///////////////////// ////////////////////////////////////////
    //////////////////////////////////////// Structured Components ////////////////////////////////////////
    //////////////////////////////////////// ///////////////////// ////////////////////////////////////////

    template <istl::String StrT = stl::string, URIStructuredComponents CompT, typename... Args>
    [[nodiscard]] constexpr StrT render_path(CompT const& comps, Args&&... args) {
        StrT out{stl::forward<Args>(args)...};
        if (comps.path.empty()) {
            return out;
        }
        auto seg = comps.path.begin();
        for (;;) {
            out += *seg;
            if (++seg == comps.path.end()) {
                break;
            }
            out += '/';
        }
        return out;
    }

    template <istl::String StrT = stl::string, URIStructuredComponents CompT, typename... Args>
    [[nodiscard]] constexpr StrT render_queries(CompT const& comps, Args&&... args) {
        StrT out{stl::forward<Args>(args)...};
        if (comps.queries.empty()) {
            return out;
        }
        for (auto pos = comps.queries.begin();;) {
            auto const [name, value]  = *pos;
            out                      += name;
            if (!value.empty()) {
                out += '=';
                out += value;
            }
            if (++pos == comps.queries.end()) {
                break;
            }
            out += '&';
        }
        return out;
    }

    //////////////////////////////////////// ////////////////// ////////////////////////////////////////
    //////////////////////////////////////// General Components ////////////////////////////////////////
    //////////////////////////////////////// ////////////////// ////////////////////////////////////////

    template <URIComponents CompT>
    [[nodiscard]] constexpr auto& scheme(CompT&& comp) noexcept {
        return stl::forward<CompT>(comp).scheme;
    }

    template <URIComponents CompT>
    constexpr void scheme(CompT& comp, typename CompT::string_type value) noexcept {
        scheme(comp) = stl::move(value);
    }

    template <URIComponents CompT>
    [[nodiscard]] constexpr auto& username(CompT&& comp) noexcept {
        return stl::forward<CompT>(comp).username;
    }

    template <URIComponents CompT>
    constexpr void username(CompT& comp, typename CompT::string_type value) noexcept {
        username(comp) = stl::move(value);
    }

    template <URIComponents CompT>
    [[nodiscard]] constexpr auto& password(CompT&& comp) noexcept {
        return stl::forward<CompT>(comp).password;
    }

    template <URIComponents CompT>
    constexpr void password(CompT& comp, typename CompT::string_type value) noexcept {
        password(comp) = stl::move(value);
    }

    template <URIComponents CompT>
    [[nodiscard]] constexpr auto& hostname(CompT&& comp) noexcept {
        return stl::forward<CompT>(comp).hostname;
    }

    template <URIComponents CompT>
    constexpr void hostname(CompT& comp, typename CompT::string_type value) noexcept {
        hostname(comp) = stl::move(value);
    }

    template <URIComponents CompT>
    [[nodiscard]] constexpr auto& port(CompT&& comp) noexcept {
        return stl::forward<CompT>(comp).port;
    }

    template <URIComponents CompT>
    constexpr void port(CompT& comp, typename CompT::string_type value) noexcept {
        port(comp) = stl::move(value);
    }

    template <URIComponents CompT>
    [[nodiscard]] constexpr auto& queries(CompT&& comp) noexcept {
        return stl::forward<CompT>(comp).queries;
    }

    template <URIComponents CompT>
    constexpr void queries(CompT& comp, typename CompT::string_type value) noexcept {
        queries(comp) = stl::move(value);
    }

    template <URIComponents CompT>
    [[nodiscard]] constexpr auto& path(CompT&& comp) noexcept {
        return stl::forward<CompT>(comp).path;
    }

    template <URIComponents CompT>
    constexpr void path(CompT& comp, typename CompT::string_type value) noexcept {
        path(comp) = stl::move(value);
    }

    template <URIComponents CompT>
    [[nodiscard]] constexpr auto& fragment(CompT&& comp) noexcept {
        return stl::forward<CompT>(comp).fragment;
    }

    template <URIComponents CompT>
    constexpr void fragment(CompT& comp, typename CompT::string_type value) noexcept {
        fragment(comp) = stl::move(value);
    }

    [[nodiscard]] constexpr bool has_username(URIComponents auto const& comp) noexcept {
        return username(comp).empty();
    }

    [[nodiscard]] constexpr bool has_password(URIComponents auto const& components) noexcept {
        return password(components).empty();
    }

    [[nodiscard]] constexpr bool has_credentials(URIComponents auto const& components) noexcept {
        return has_username(components) || has_password(components);
    }

} // namespace webpp::uri

#endif // WEBPP_URL_COMPONENTS_HPP
