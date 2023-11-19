// Created by moisrex on 10/9/23.

#ifndef WEBPP_URL_COMPONENTS_HPP
#define WEBPP_URL_COMPONENTS_HPP

#include "../../std/collection.hpp"
#include "../../std/map.hpp"
#include "../../std/string_like.hpp"
#include "../../std/vector.hpp"
#include "./uri_status.hpp"

#include <cstdint>
#include <limits>

namespace webpp::uri {

    template <typename SegType = stl::uint32_t, typename Iter = const char*>
    struct uri_components;

    template <typename T>
    struct is_uri_component : stl::false_type {};

    template <typename SegType, typename Iter>
    struct is_uri_component<uri_components<SegType, Iter>> : stl::true_type {};

    /// == nothing_type
    /// Iterator type has no effect
    template <typename Iter>
    struct uri_components<void, Iter> {};


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
    template <stl::integral SegType, typename Iter>
    struct uri_components<SegType, Iter> {
        using seg_type       = SegType;
        using iterator       = Iter;
        using map_iterator   = seg_type*;
        using map_value_type = seg_type;
        using vec_iterator   = seg_type*;

        /// maximum number that this url component class supports
        static constexpr auto max_supported_length = stl::numeric_limits<seg_type>::max() - 1;

        static constexpr seg_type omitted = stl::numeric_limits<seg_type>::max();

        /// resetting the values of integer types are always noexcept
        static constexpr bool is_nothrow = true;

        static constexpr bool is_modifiable = false;

        static constexpr bool is_segregated = false;

      private:
        iterator uri_beg{};
        seg_type scheme_end      = omitted; // excluding :// stuff
        seg_type authority_start = omitted; // username/host start
        seg_type password_start  = omitted;
        seg_type host_start      = omitted;
        seg_type port_start      = omitted;
        seg_type authority_end   = omitted; // path start
        seg_type queries_start   = omitted; // path end
        seg_type fragment_start  = omitted; // query end
        seg_type uri_end         = omitted; // string end

      public:
        constexpr void set_scheme(iterator beg, iterator end) noexcept {
            uri_beg    = beg;
            scheme_end = uri_end = static_cast<seg_type>(end - beg);
        }

        constexpr void set_path(seg_type beg, seg_type end) noexcept {
            authority_end = beg;
            uri_end       = end;
        }

        constexpr void set_path(iterator beg, iterator end) noexcept {
            set_path(static_cast<seg_type>(beg - uri_beg), static_cast<seg_type>(end - uri_beg));
        }

        constexpr void set_username(seg_type beg, seg_type end) noexcept {
            authority_start = beg;
            uri_end = authority_end = end;
        }

        constexpr void set_username(iterator beg, iterator end) noexcept {
            set_username(static_cast<seg_type>(beg - uri_beg), static_cast<seg_type>(end - uri_beg));
        }

        constexpr void set_password(seg_type beg, seg_type end) noexcept {
            password_start = beg;
            uri_end = authority_end = end;
        }

        constexpr void set_password(iterator beg, iterator end) noexcept {
            set_password(static_cast<seg_type>(beg - uri_beg), static_cast<seg_type>(end - uri_beg));
        }

        constexpr void set_host(seg_type beg, seg_type end) noexcept {
            host_start = beg;
            uri_end    = end;
        }

        constexpr void set_host(iterator beg, iterator end) noexcept {
            set_host(static_cast<seg_type>(beg - uri_beg), static_cast<seg_type>(end - uri_beg));
        }

        constexpr void set_port(seg_type start, seg_type end) noexcept {
            port_start    = start;
            authority_end = uri_end = end;
        }

        constexpr void set_port(iterator beg, iterator end) noexcept {
            set_port(static_cast<seg_type>(beg - uri_beg), static_cast<seg_type>(end - uri_beg));
        }

        constexpr void set_queries(seg_type start, seg_type end) noexcept {
            queries_start = start;
            uri_end       = end;
        }

        constexpr void set_queries(iterator beg, iterator end) noexcept {
            set_queries(static_cast<seg_type>(beg - uri_beg), static_cast<seg_type>(end - uri_beg));
        }

        constexpr void set_fragment(seg_type start, seg_type end) noexcept {
            fragment_start = start;
            uri_end        = end;
        }

        constexpr void set_fragment(iterator beg, iterator end) noexcept {
            set_fragment(static_cast<seg_type>(beg - uri_beg), static_cast<seg_type>(end - uri_beg));
        }


        constexpr void clear_scheme() noexcept {
            scheme_end = omitted;
        }

        constexpr void clear_host() noexcept {
            host_start = omitted; // todo: should we reset authority_start and password_start as well?
        }

        constexpr void clear_port() noexcept {
            port_start = omitted;
        }

        constexpr void clear_username() noexcept {
            authority_start = omitted;
        }

        constexpr void clear_password() noexcept {
            password_start = omitted;
        }

        constexpr void clear_path() noexcept {
            authority_end = omitted;
        }

        constexpr void clear_queries() noexcept {
            queries_start = omitted;
        }

        constexpr void clear_fragment() noexcept {
            fragment_start = omitted;
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

        [[nodiscard]] constexpr bool has_host() const noexcept {
            return host_start != omitted;
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

        [[nodiscard]] constexpr bool has_queries() const noexcept {
            return queries_start != omitted;
        }

        [[nodiscard]] constexpr bool has_fragment() const noexcept {
            return fragment_start != omitted;
        }

      private:
        template <istl::StringView StrT = stl::string_view>
        [[nodiscard]] constexpr StrT view(seg_type beg, seg_type size) const noexcept {
            using str_iterator = typename StrT::iterator;
            if constexpr (!stl::same_as<str_iterator, iterator> && requires {
                              StrT{uri_beg.base() + beg, size};
                          }) {
                return StrT{uri_beg.base() + beg, size};
            } else {
                return StrT{uri_beg + beg, size};
            }
        }

      public:
        [[nodiscard]] constexpr seg_type size() const noexcept {
            return uri_end == omitted ? 0 : uri_end;
        }

        template <istl::StringView StrT = stl::string_view>
        [[nodiscard]] constexpr StrT get_scheme() const noexcept {
            if (scheme_end == omitted) {
                return {};
            }
            return view<StrT>(0, stl::min(scheme_end, size()));
        }

        template <istl::StringView StrT = stl::string_view>
        [[nodiscard]] constexpr StrT get_username() const noexcept {
            if (authority_start == omitted) {
                return {};
            }
            return view<StrT>(authority_start,
                              stl::min(password_start - 1, host_start - 1) - authority_start);
        }

        template <istl::StringView StrT = stl::string_view>
        [[nodiscard]] constexpr StrT get_password() const noexcept {
            if (password_start == omitted) {
                return {};
            }
            return view<StrT>(password_start, host_start - 1 - password_start);
        }

        template <istl::StringView StrT = stl::string_view>
        [[nodiscard]] constexpr StrT get_host() const noexcept {
            if (host_start == omitted) {
                return {};
            }
            return view<StrT>(host_start,
                              stl::min(stl::min(port_start - 1, authority_end), size()) - host_start);
        }

        template <istl::StringView StrT = stl::string_view>
        [[nodiscard]] constexpr StrT get_port() const noexcept {
            if (port_start == omitted) {
                return {};
            }
            return view<StrT>(port_start, stl::min(authority_end, size()) - port_start);
        }

        template <istl::StringView StrT = stl::string_view>
        [[nodiscard]] constexpr StrT get_path() const noexcept {
            if (authority_end == omitted) {
                return {};
            }
            return view<StrT>(authority_end,
                              stl::min(stl::min(queries_start - 1, fragment_start - 1), size()) -
                                authority_end);
        }

        template <istl::StringView StrT = stl::string_view>
        [[nodiscard]] constexpr StrT get_queries() const noexcept {
            if (queries_start == omitted) {
                return {};
            }
            return view<StrT>(queries_start, stl::min(fragment_start - 1, size()) - queries_start);
        }

        template <istl::StringView StrT = stl::string_view>
        [[nodiscard]] constexpr StrT get_fragment() const noexcept {
            if (fragment_start == omitted) {
                return {};
            }
            return view<StrT>(fragment_start, uri_end - fragment_start);
        }


        // NOLINTBEGIN(*-macro-usage)
#define webpp_def(field)                           \
    constexpr auto& field##_ref() const noexcept { \
        return *this;                              \
    }



        // they're not used
        webpp_def(scheme)     //
          webpp_def(username) //
          webpp_def(password) //
          webpp_def(host)     //
          webpp_def(port)     //
          webpp_def(path)     //
          webpp_def(queries)  //
          webpp_def(fragment) //

        // NOLINTEND(*-macro-usage)
#undef webpp_def
    };


    template <istl::StringLike StrT, typename Iter>
    struct uri_components<StrT, Iter> {
        using string_type    = StrT;
        using iterator       = Iter;
        using seg_type       = string_type;
        using char_type      = typename string_type::value_type;
        using size_type      = typename string_type::size_type;
        using map_iterator   = seg_type*;
        using map_value_type = seg_type;
        using vec_iterator   = seg_type*;

        /// maximum number that this url component class supports
        static constexpr auto max_supported_length = stl::numeric_limits<size_type>::max() - 1;

        /// is resetting the values are noexcept or not
        static constexpr bool is_nothrow = stl::is_nothrow_assignable_v<string_type, char_type const*>;

        static constexpr bool is_modifiable = istl::ModifiableString<string_type>;

        static constexpr bool is_segregated = false;

      private:
        string_type m_scheme{};
        string_type m_username{};
        string_type m_password{};
        string_type m_host{};
        string_type m_port{};
        string_type m_path{};
        string_type m_queries{};
        string_type m_fragment{};

      public:
        // NOLINTBEGIN(*-macro-usage)
#define webpp_def(field)                                                          \
    template <istl::StringView StrVT = stl::basic_string_view<char_type>>         \
    [[nodiscard]] constexpr StrVT get_##field() const noexcept {                  \
        return {m_##field.data(), m_##field.size()};                              \
    }                                                                             \
                                                                                  \
    constexpr void clear_##field() noexcept {                                     \
        istl::clear(m_##field);                                                   \
    }                                                                             \
                                                                                  \
    [[nodiscard]] constexpr bool has_##field() const noexcept {                   \
        return !m_##field.empty();                                                \
    }                                                                             \
                                                                                  \
    constexpr void set_##field(iterator beg, iterator end) noexcept(is_nothrow) { \
        istl::assign(m_##field, beg, end);                                        \
    }                                                                             \
                                                                                  \
    constexpr void set_##field(string_type str) noexcept(is_nothrow) {            \
        m_##field = stl::move(str);                                               \
    }                                                                             \
                                                                                  \
    constexpr string_type& field##_ref() noexcept {                               \
        return m_##field;                                                         \
    }                                                                             \
                                                                                  \
    constexpr string_type const& field##_ref() const noexcept {                   \
        return m_##field;                                                         \
    }



        webpp_def(scheme)     //
          webpp_def(username) //
          webpp_def(password) //
          webpp_def(host)     //
          webpp_def(port)     //
          webpp_def(path)     //
          webpp_def(queries)  //
          webpp_def(fragment) //

        // NOLINTEND(*-macro-usage)
#undef webpp_def

          [[nodiscard]] constexpr bool has_credentials() const noexcept {
            return has_username(); // password cannot exist without the username
        }



        /// Create a URI Component using an allocator
        // template <istl::String StringType = StrT>
        // [[nodiscard]] friend constexpr uri_components<StringType>
        // uri_components_from(typename StringType::allocator_type const& alloc) noexcept(
        //   stl::is_nothrow_constructible_v<StringType, typename StringType::allocator_type>) {
        //     return {.scheme   = StringType{alloc},
        //             .username = StringType{alloc},
        //             .password = StringType{alloc},
        //             .host     = StringType{alloc},
        //             .port     = StringType{alloc},
        //             .path     = StringType{alloc},
        //             .queries  = StringType{alloc},
        //             .fragment = StringType{alloc}};
        // }

        // template <istl::String StringType, stl::integral SegType, istl::StringLike SourceStr>
        //     requires(stl::same_as<istl::char_type_of_t<StringType>, istl::char_type_of_t<SourceStr>>)
        // [[nodiscard]] friend constexpr uri_components<StringType> uri_components_from(
        //   uri_components<SegType> const&             comps,
        //   SourceStr const&                           source,
        //   typename StringType::allocator_type const& alloc =
        //     {}) noexcept(stl::is_nothrow_constructible_v<StringType, typename StringType::allocator_type>)
        //     { auto const beg = source.data(); return {
        //       .scheme = StringType{beg, comps.scheme_end, alloc},
        //       .username =
        //         StringType{beg + comps.authority_start, comps.host_start - comps.authority_start, alloc},
        //       .password =
        //         StringType{beg + comps.password_start, comps.host_start - comps.password_start, alloc},
        //       .host = StringType{beg + comps.host_start, comps.port_start - comps.host_start, alloc},
        //       .port = StringType{beg + comps.port_start, comps.authority_end - comps.port_start, alloc},
        //       .path = StringType{beg + comps.authority_end, comps.queries_start - comps.authority_end,
        //       alloc}, .queries =
        //         StringType{beg + comps.queries_start, comps.fragment_start - comps.queries_start, alloc},
        //       .fragment =
        //         StringType{beg + comps.fragment_start, source.size() - comps.fragment_start, alloc}};
        // }

        // template <istl::StringView StringType, stl::integral SegType, istl::StringLike SourceStr>
        //     requires(stl::same_as<istl::char_type_of_t<StringType>, istl::char_type_of_t<SourceStr>>)
        // [[nodiscard]] friend constexpr uri_components<StringType>
        // uri_components_from(uri_components<SegType> const& comps, SourceStr const& source) noexcept {
        //     auto const beg = source.data();
        //     return {
        //       .scheme   = StringType{beg, comps.scheme_end},
        //       .username = StringType{beg + comps.authority_start, comps.host_start -
        //       comps.authority_start}, .password = StringType{beg + comps.password_start, comps.host_start -
        //       comps.password_start}, .host     = StringType{beg + comps.host_start, comps.port_start -
        //       comps.host_start}, .port     = StringType{beg + comps.port_start, comps.authority_end -
        //       comps.port_start}, .path     = StringType{beg + comps.authority_end, comps.queries_start -
        //       comps.authority_end}, .queries  = StringType{beg + comps.queries_start, comps.fragment_start
        //       - comps.queries_start}, .fragment = StringType{beg + comps.fragment_start, source.size() -
        //       comps.fragment_start}};
        // }
    };





    /**
     * URI Components fully separated
     *   - Domains are split into its subdomains
     *   - Path are split into its segments
     *   - Queries are mapped
     */
    template <istl::LinearContainer VecOfStr, istl::MapContainer MapOfStr>
    struct uri_components<VecOfStr, MapOfStr> {
        using vec_type     = VecOfStr;
        using map_type     = MapOfStr;
        using string_type  = typename vec_type::value_type;
        using seg_type     = string_type;
        using iterator     = typename string_type::iterator;
        using char_type    = typename string_type::value_type;
        using size_type    = typename string_type::size_type;
        using map_iterator = typename map_type::iterator;
        using vec_iterator = typename vec_type::iterator;

        // map_type::value_type is const, we need a modifiable name
        using map_value_type = stl::pair<typename map_type::key_type, typename map_type::mapped_type>;


        /// maximum number that this url component class supports
        static constexpr auto max_supported_length = stl::numeric_limits<size_type>::max() - 1;

        /// is resetting the values are noexcept or not
        static constexpr bool is_nothrow = false;

        static constexpr bool is_modifiable = istl::ModifiableString<string_type>;

        static constexpr bool is_segregated = true;

      private:
        string_type m_scheme{};
        string_type m_username{};
        string_type m_password{};
        vec_type    m_host{};
        string_type m_port{};
        vec_type    m_path{};
        map_type    m_queries{};
        string_type m_fragment{};

      public:
        // NOLINTBEGIN(*-macro-usage)
#define webpp_def(field)                                                                            \
    static constexpr bool is_##field##_modifiable = stl::same_as<decltype(m_##field), string_type>; \
                                                                                                    \
    template <istl::StringView StrVT = stl::string_view>                                            \
        requires(is_##field##_modifiable)                                                           \
    [[nodiscard]] constexpr StrVT get_##field() const noexcept {                                    \
        return {m_##field.data(), m_##field.size()};                                                \
    }                                                                                               \
                                                                                                    \
    constexpr void clear_##field() noexcept                                                         \
        requires(is_##field##_modifiable)                                                           \
    {                                                                                               \
        istl::clear(m_##field);                                                                     \
    }                                                                                               \
                                                                                                    \
    constexpr void clear_##field() noexcept                                                         \
        requires(!is_##field##_modifiable)                                                          \
    {                                                                                               \
        istl::collection::clear(m_##field);                                                         \
    }                                                                                               \
                                                                                                    \
    [[nodiscard]] constexpr bool has_##field() const noexcept {                                     \
        return !m_##field.empty();                                                                  \
    }                                                                                               \
                                                                                                    \
    constexpr void set_##field(iterator beg, iterator end) noexcept(is_nothrow)                     \
        requires(is_##field##_modifiable)                                                           \
    {                                                                                               \
        istl::assign(m_##field, beg, end);                                                          \
    }                                                                                               \
                                                                                                    \
    constexpr void set_##field(decltype(m_##field)&& str) noexcept(is_nothrow)                      \
        requires(is_##field##_modifiable)                                                           \
    {                                                                                               \
        m_##field = stl::move(str);                                                                 \
    }                                                                                               \
                                                                                                    \
    constexpr auto& field##_ref() noexcept {                                                        \
        return m_##field;                                                                           \
    }                                                                                               \
                                                                                                    \
    constexpr auto const& field##_ref() const noexcept {                                            \
        return m_##field;                                                                           \
    }



        webpp_def(scheme)     //
          webpp_def(username) //
          webpp_def(password) //
          webpp_def(host)     //
          webpp_def(port)     //
          webpp_def(path)     //
          webpp_def(queries)  //
          webpp_def(fragment) //

        // NOLINTEND(*-macro-usage)
#undef webpp_def

          constexpr void set_host(iterator beg, iterator end) {
            istl::collection::clear(m_host);
            if constexpr (is_modifiable) {
                istl::collection::emplace_one(m_host, beg, end, m_host.get_allocator());
            } else {
                istl::collection::emplace_one(m_host, beg, end);
            }
        }

        constexpr void set_path(iterator beg, iterator end) {
            istl::collection::clear(m_path);
            if constexpr (is_modifiable) {
                istl::collection::emplace_one(m_path, beg, end, m_path.get_allocator());
            } else {
                istl::collection::emplace_one(m_path, beg, end);
            }
        }

        constexpr void set_queries(iterator beg, iterator end) {
            using pack_type  = typename map_type::value_type;
            using key_type   = typename map_type::key_type;
            using value_type = typename map_type::mapped_type;

            // first "name" is chosen for the whole value, because of the algorithm that gets the queries will
            // be correct that way

            istl::collection::clear(m_queries);
            if constexpr (is_modifiable) {
                istl::collection::emplace_one(m_queries,
                                              pack_type{key_type{beg, end, m_queries.get_allocator()},
                                                        value_type{m_queries.get_allocator()}});
            } else {
                istl::collection::emplace_one(m_queries, pack_type{key_type{beg, end}, value_type{}});
            }
        }

        template <istl::String StrT = stl::string, typename... Args>
        [[nodiscard]] constexpr StrT get_host(Args&&... args) const {
            StrT out{stl::forward<Args>(args)...};
            if (m_host.empty()) {
                return out;
            }
            for (auto pos = m_host.begin();;) {
                out += *pos;
                if (++pos == m_host.end()) {
                    break;
                }
                out += '.';
            }
            return out;
        }

        template <istl::String StrT = stl::string, typename... Args>
        [[nodiscard]] constexpr StrT get_path(Args&&... args) const {
            StrT out{stl::forward<Args>(args)...};
            if (m_path.empty()) {
                return out;
            }
            auto seg = m_path.begin();
            for (;;) {
                out += *seg;
                if (++seg == m_path.end()) {
                    break;
                }
                out += '/';
            }
            return out;
        }

        template <istl::String StrT = stl::string, typename... Args>
        [[nodiscard]] constexpr StrT get_queries(Args&&... args) const {
            StrT out{stl::forward<Args>(args)...};
            if (m_queries.empty()) {
                return out;
            }
            for (auto pos = m_queries.begin();;) {
                auto const [name, value] = *pos;
                out += name;
                if (!value.empty()) {
                    out += '=';
                    out += value;
                }
                if (++pos == m_queries.end()) {
                    break;
                }
                out += '&';
            }
            return out;
        }

        [[nodiscard]] constexpr bool has_credentials() const noexcept {
            return has_username(); // password cannot exist without the username
        }
    };





    using uri_components_view = uri_components<stl::string_view, stl::string_view::const_iterator>;
    using uri_components_u32  = uri_components<stl::uint32_t, const char*>;


    /**
     * A class used during parsing a URI
     */
    template <typename OutSegType  = stl::uint32_t,
              typename OutIter     = const char*,
              typename BaseSegType = void,
              typename BaseIter    = OutIter>
    struct parsing_uri_context {
        using out_seg_type    = OutSegType;
        using base_seg_type   = BaseSegType;
        using out_type        = uri_components<out_seg_type, OutIter>;
        using base_type       = uri_components<base_seg_type, BaseIter>;
        using seg_type        = typename out_type::seg_type; // this might be different than OutSegType
        using iterator        = typename out_type::iterator;
        using iterator_traits = stl::iterator_traits<iterator>;
        using char_type       = istl::char_type_of_t<typename iterator_traits::pointer>;
        using state_type      = stl::underlying_type_t<uri_status>;

        static constexpr bool is_nothrow    = out_type::is_nothrow;
        static constexpr bool has_base_uri  = !stl::is_void_v<BaseSegType>;
        static constexpr bool is_modifiable = out_type::is_modifiable;
        static constexpr bool is_segregated = out_type::is_segregated;

        iterator   beg{}; // the beginning of the string, not going to change during parsing
        iterator   pos{}; // current position
        iterator   end{}; // the end of the string
        out_type   out{}; // the output uri components
        state_type status     = stl::to_underlying(uri_status::unparsed);
        bool       is_special = false;
    };

    template <typename OutSegType, istl::StringLike OutIter, typename BaseSegType, typename BaseIter>
    struct parsing_uri_context<OutSegType, OutIter, BaseSegType, BaseIter> {
        using out_seg_type    = OutSegType;
        using base_seg_type   = BaseSegType;
        using out_type        = uri_components<out_seg_type, OutIter>;
        using base_type       = uri_components<base_seg_type, BaseIter>;
        using iterator        = typename out_type::iterator;
        using iterator_traits = stl::iterator_traits<iterator>;
        using char_type       = istl::char_type_of_t<typename iterator_traits::pointer>;
        using state_type      = stl::underlying_type_t<uri_status>;

        static constexpr bool is_nothrow    = out_type::is_nothrow;
        static constexpr bool has_base_uri  = !stl::is_void_v<BaseSegType>;
        static constexpr bool is_modifiable = out_type::is_modifiable;
        static constexpr bool is_segregated = out_type::is_segregated;

        // static_assert(has_base_uri && !is_modifiable,
        //               "If you have Base URI, then you need to make sure the output is overridable "
        //               "(because output's URI's components may come from different places, "
        //               "for example the scheme and domain may come from the base URI while the path "
        //               "is coming from the new string.)");


        // we don't need to know the beginning of the string("beg" field), because the output uri components
        // ("out") are required to know each segment themselves.
        iterator beg{}; // the beginning of the string, not going to change during parsing
        iterator pos{};
        iterator end{};
        out_type out{};
        [[no_unique_address]] base_type base{};
        state_type                      status     = stl::to_underlying(uri_status::unparsed);
        bool                            is_special = false;
    };


    using parsing_uri_context_u32 = parsing_uri_context<stl::uint32_t, const char*>;

    template <typename StrT = stl::string>
    using parsing_uri_context_string = parsing_uri_context<StrT, typename StrT::const_iterator>;

    template <typename CharT = char>
    using parsing_uri_context_view = parsing_uri_context_string<stl::basic_string_view<CharT>>;

    template <istl::StringLike StrT = stl::string, typename Allocator = typename StrT::allocator_type>
    using parsing_uri_context_segregated = parsing_uri_context<
      stl::vector<StrT, typename stl::allocator_traits<Allocator>::template rebind_alloc<StrT>>,
      istl::map_of_strings<StrT, Allocator>>;

    template <typename Allocator = stl::allocator<char>>
    using parsing_uri_context_segregated_view = parsing_uri_context_segregated<stl::string_view, Allocator>;



} // namespace webpp::uri

#endif // WEBPP_URL_COMPONENTS_HPP
