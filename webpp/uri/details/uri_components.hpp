// Created by moisrex on 10/9/23.

#ifndef WEBPP_URL_COMPONENTS_HPP
#define WEBPP_URL_COMPONENTS_HPP

#include "../../std/concepts.hpp"
#include "../../std/string_like.hpp"
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
        using seg_type = SegType;
        using iterator = Iter;

        /// maximum number that this url component class supports
        static constexpr auto max_supported_length = stl::numeric_limits<seg_type>::max() - 1;

        static constexpr seg_type omitted = stl::numeric_limits<seg_type>::max();

        /// resetting the values of integer types are always noexcept
        static constexpr bool is_nothrow = true;

        static constexpr bool is_modifiable = false;

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
        constexpr void scheme(iterator beg, iterator end) noexcept {
            uri_beg    = beg;
            scheme_end = uri_end = static_cast<seg_type>(end - beg);
        }

        constexpr void path(seg_type beg, seg_type end) noexcept {
            authority_end = beg;
            uri_end       = end;
        }

        constexpr void path(iterator beg, iterator end) noexcept {
            path(static_cast<seg_type>(beg - uri_beg), static_cast<seg_type>(end - uri_beg));
        }

        constexpr void username(seg_type beg, seg_type end) noexcept {
            authority_start = beg;
            uri_end = authority_end = end;
        }

        constexpr void username(iterator beg, iterator end) noexcept {
            username(static_cast<seg_type>(beg - uri_beg), static_cast<seg_type>(end - uri_beg));
        }

        constexpr void password(seg_type beg, seg_type end) noexcept {
            password_start = beg;
            uri_end = authority_end = end;
        }

        constexpr void password(iterator beg, iterator end) noexcept {
            password(static_cast<seg_type>(beg - uri_beg), static_cast<seg_type>(end - uri_beg));
        }

        constexpr void host(seg_type beg, seg_type end) noexcept {
            host_start = beg;
            uri_end    = end;
        }

        constexpr void host(iterator beg, iterator end) noexcept {
            host(static_cast<seg_type>(beg - uri_beg), static_cast<seg_type>(end - uri_beg));
        }

        constexpr void port(seg_type start, seg_type end) noexcept {
            port_start    = start;
            authority_end = uri_end = end;
        }

        constexpr void port(iterator beg, iterator end) noexcept {
            port(static_cast<seg_type>(beg - uri_beg), static_cast<seg_type>(end - uri_beg));
        }

        constexpr void queries(seg_type start, seg_type end) noexcept {
            queries_start = start;
            uri_end       = end;
        }

        constexpr void queries(iterator beg, iterator end) noexcept {
            queries(static_cast<seg_type>(beg - uri_beg), static_cast<seg_type>(end - uri_beg));
        }

        constexpr void fragment(seg_type start, seg_type end) noexcept {
            fragment_start = start;
            uri_end        = end;
        }

        constexpr void fragment(iterator beg, iterator end) noexcept {
            fragment(static_cast<seg_type>(beg - uri_beg), static_cast<seg_type>(end - uri_beg));
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
        [[nodiscard]] constexpr stl::size_t size() const noexcept {
            return uri_end == omitted ? 0 : uri_end;
        }

        template <istl::StringView StrT = stl::string_view>
        [[nodiscard]] constexpr StrT scheme() const noexcept {
            return view<StrT>(0, scheme_end);
        }

        template <istl::StringView StrT = stl::string_view>
        [[nodiscard]] constexpr StrT username() const noexcept {
            return view<StrT>(authority_start, host_start - authority_start);
        }

        template <istl::StringView StrT = stl::string_view>
        [[nodiscard]] constexpr StrT password() const noexcept {
            return view<StrT>(password_start, host_start - password_start);
        }

        template <istl::StringView StrT = stl::string_view>
        [[nodiscard]] constexpr StrT host() const noexcept {
            return view<StrT>(host_start, stl::min(port_start, authority_end) - host_start);
        }

        template <istl::StringView StrT = stl::string_view>
        [[nodiscard]] constexpr StrT port() const noexcept {
            return view<StrT>(port_start, authority_end - port_start);
        }

        template <istl::StringView StrT = stl::string_view>
        [[nodiscard]] constexpr StrT path() const noexcept {
            return view<StrT>(
              authority_end,
              stl::min(stl::min(queries_start, fragment_start), static_cast<seg_type>(size())) -
                authority_end);
        }

        template <istl::StringView StrT = stl::string_view>
        [[nodiscard]] constexpr StrT queries() const noexcept {
            return view<StrT>(queries_start,
                              stl::min(fragment_start, static_cast<seg_type>(size())) - queries_start);
        }

        template <istl::StringView StrT = stl::string_view>
        [[nodiscard]] constexpr StrT fragment() const noexcept {
            return view<StrT>(fragment_start, uri_end);
        }
    };


    template <istl::StringLike StrT, typename Iter>
    struct uri_components<StrT, Iter> {
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

        static constexpr bool is_modifiable = istl::ModifiableString<string_type>;

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
#define webpp_def(field)                                                              \
    template <istl::StringView StrVT = stl::string_view>                              \
    [[nodiscard]] constexpr StrT field() const noexcept {                             \
        return {m_##field.data(), m_##field.size()};                                  \
    }                                                                                 \
                                                                                      \
    constexpr void clear_##field() noexcept {                                         \
        istl::clear(m_##field);                                                       \
    }                                                                                 \
                                                                                      \
    [[nodiscard]] constexpr bool has_##field() const noexcept {                       \
        return !m_##field.empty();                                                    \
    }                                                                                 \
                                                                                      \
    constexpr void field(const_pointer beg, const_pointer end) noexcept(is_nothrow) { \
        istl::assign(m_##field, beg, end);                                            \
    }                                                                                 \
                                                                                      \
    constexpr void field(string_type str) noexcept(is_nothrow) {                      \
        m_##field = stl::move(str);                                                   \
    }                                                                                 \
                                                                                      \
    constexpr string_type& field##_ref() noexcept {                                   \
        return m_##field;                                                             \
    }                                                                                 \
                                                                                      \
    constexpr string_type const& field##_ref() const noexcept {                       \
        return m_##field;                                                             \
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
        template <istl::String StringType = StrT>
        [[nodiscard]] friend constexpr uri_components<StringType>
        uri_components_from(typename StringType::allocator_type const& alloc) noexcept(
          stl::is_nothrow_constructible_v<StringType, typename StringType::allocator_type>) {
            return {.scheme   = StringType{alloc},
                    .username = StringType{alloc},
                    .password = StringType{alloc},
                    .host     = StringType{alloc},
                    .port     = StringType{alloc},
                    .path     = StringType{alloc},
                    .queries  = StringType{alloc},
                    .fragment = StringType{alloc}};
        }

        template <istl::String StringType, stl::integral SegType, istl::StringLike SourceStr>
            requires(stl::same_as<istl::char_type_of_t<StringType>, istl::char_type_of_t<SourceStr>>)
        [[nodiscard]] friend constexpr uri_components<StringType> uri_components_from(
          uri_components<SegType> const&             comps,
          SourceStr const&                           source,
          typename StringType::allocator_type const& alloc =
            {}) noexcept(stl::is_nothrow_constructible_v<StringType, typename StringType::allocator_type>) {
            auto const beg = source.data();
            return {
              .scheme = StringType{beg, comps.scheme_end, alloc},
              .username =
                StringType{beg + comps.authority_start, comps.host_start - comps.authority_start, alloc},
              .password =
                StringType{beg + comps.password_start, comps.host_start - comps.password_start, alloc},
              .host = StringType{beg + comps.host_start, comps.port_start - comps.host_start, alloc},
              .port = StringType{beg + comps.port_start, comps.authority_end - comps.port_start, alloc},
              .path = StringType{beg + comps.authority_end, comps.queries_start - comps.authority_end, alloc},
              .queries =
                StringType{beg + comps.queries_start, comps.fragment_start - comps.queries_start, alloc},
              .fragment =
                StringType{beg + comps.fragment_start, source.size() - comps.fragment_start, alloc}};
        }

        template <istl::StringView StringType, stl::integral SegType, istl::StringLike SourceStr>
            requires(stl::same_as<istl::char_type_of_t<StringType>, istl::char_type_of_t<SourceStr>>)
        [[nodiscard]] friend constexpr uri_components<StringType>
        uri_components_from(uri_components<SegType> const& comps, SourceStr const& source) noexcept {
            auto const beg = source.data();
            return {
              .scheme   = StringType{beg, comps.scheme_end},
              .username = StringType{beg + comps.authority_start, comps.host_start - comps.authority_start},
              .password = StringType{beg + comps.password_start, comps.host_start - comps.password_start},
              .host     = StringType{beg + comps.host_start, comps.port_start - comps.host_start},
              .port     = StringType{beg + comps.port_start, comps.authority_end - comps.port_start},
              .path     = StringType{beg + comps.authority_end, comps.queries_start - comps.authority_end},
              .queries  = StringType{beg + comps.queries_start, comps.fragment_start - comps.queries_start},
              .fragment = StringType{beg + comps.fragment_start, source.size() - comps.fragment_start}};
        }
    };



    using uri_components_view = uri_components<stl::string_view, stl::string_view::const_pointer>;
    using uri_components_u32  = uri_components<stl::uint32_t, const char*>;


    /**
     * A class used during parsing a URI
     */
    template <typename Iter = const char*, typename OutSegType = stl::uint32_t, typename BaseSegType = void>
    struct parsing_uri_context {
        using iterator        = Iter;
        using iterator_traits = stl::iterator_traits<iterator>;
        using char_type       = istl::char_type_of_t<typename iterator_traits::pointer>;
        using out_seg_type    = OutSegType;
        using base_seg_type   = BaseSegType;
        using out_type        = uri::uri_components<out_seg_type, iterator>;
        using base_type       = uri::uri_components<base_seg_type, iterator>;
        using state_type      = stl::underlying_type_t<uri_status>;

        static constexpr bool is_nothrow    = out_type::is_nothrow;
        static constexpr bool has_base_uri  = !stl::is_void_v<BaseSegType>;
        static constexpr bool is_modifiable = out_type::is_modifiable;

        iterator   beg{}; // the beginning of the string, not going to change during parsing
        iterator   pos{}; // current position
        iterator   end{}; // the end of the string
        out_type   out{}; // the output uri components
        state_type status = stl::to_underlying(uri_status::unparsed);
    };

    template <typename Iter, istl::StringLike OutSegType, typename BaseSegType>
    struct parsing_uri_context<Iter, OutSegType, BaseSegType> {
        using iterator        = Iter;
        using iterator_traits = stl::iterator_traits<iterator>;
        using char_type       = istl::char_type_of_t<typename iterator_traits::pointer>;
        using out_seg_type    = OutSegType;
        using base_seg_type   = BaseSegType;
        using out_type        = uri::uri_components<out_seg_type, iterator>;
        using base_type       = uri::uri_components<base_seg_type, iterator>;
        using state_type      = stl::underlying_type_t<uri_status>;

        static constexpr bool is_nothrow    = out_type::is_nothrow;
        static constexpr bool has_base_uri  = !stl::is_void_v<BaseSegType>;
        static constexpr bool is_modifiable = out_type::is_modifiable;

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
        state_type                      status = stl::to_underlying(uri_status::unparsed);
    };


} // namespace webpp::uri

#endif // WEBPP_URL_COMPONENTS_HPP
