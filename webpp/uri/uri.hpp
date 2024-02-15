// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_HPP
#define WEBPP_URI_HPP

#include "../std/string_view.hpp"
#include "authority.hpp"
#include "fragment.hpp"
#include "host.hpp"
#include "path.hpp"
#include "port.hpp"
#include "queries.hpp"
#include "scheme.hpp"
#include "uri_string.hpp"

namespace webpp::uri {

    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void continue_parsing_uri(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        while (!has_error(ctx.status)) {
            switch (get_value(ctx.status)) {
                using enum uri_status;
                case valid:                  // we're done parsing
                case valid_punycode: return; // todo?
                case valid_authority: parse_authority<Options>(ctx); break;
                case valid_file_host:
                    if constexpr (Options.allow_file_hosts) {
                        parse_file_host<Options>(ctx);
                    } else {
                        stl::unreachable(); // should be impossible.
                    }
                    break;
                case valid_port: parse_port<Options>(ctx); break;
                case valid_authority_end: parse_authority_end<Options>(ctx); break;
                case valid_opaque_path: parse_opaque_path<Options>(ctx); break;
                case valid_path: parse_path<Options>(ctx); break;
                case valid_queries: parse_queries<Options>(ctx); break;
                case valid_fragment: parse_fragment<Options>(ctx); break;
                case unparsed: parse_scheme<Options>(ctx); break; // start from the beginning
                default: stl::unreachable(); break;               // should be impossible
            }
        }
    }

    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void parse_uri(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        continue_parsing_uri<Options>(ctx);
    }

    template <uri_parsing_options Options = uri_parsing_options{}, istl::StringView StrV = stl::string_view>
    static constexpr auto parse_uri(StrV str) noexcept {
        using iterator     = typename StrV::const_iterator;
        using context_type = parsing_uri_context<StrV, iterator>;
        context_type context{.beg = str.begin(),
                             .pos = str.begin(), // current position is start
                             .end = str.end()};
        parse_uri<Options>(context);
        return context;
    }

    template <uri_parsing_options Options = uri_parsing_options{}, istl::StringLike StrT, typename SegType>
    static constexpr auto parse_uri(
      StrT const&                    the_url,
      uri_components<SegType> const& origin_context) noexcept(istl::StringView<StrT>) {
        using iterator             = typename StrT::const_iterator;
        using base_components_type = uri_components<SegType>;
        using base_seg_type        = typename base_components_type::seg_type;
        using context_type         = parsing_uri_context<StrT, iterator, base_seg_type>;

        context_type context{.beg  = the_url.begin(),
                             .pos  = the_url.begin(), // current position is start
                             .end  = the_url.end(),
                             .base = origin_context};
        parse_uri<Options>(context);
        return context;
    }

    template <uri_parsing_options     Options = uri_parsing_options{},
              istl::StringLike        StrT,
              istl::StringViewifiable OStrV>
    static constexpr auto parse_uri(StrT const& the_url, OStrV&& base_uri) noexcept(istl::StringView<StrT>) {
        using iterator = typename StrT::const_iterator;
        static_assert(
          stl::same_as<iterator, typename OStrV::const_iterator>,
          "Origin's string's char type must be the same as the specified URI's string's char type.");
        auto const base         = istl::string_viewify(stl::forward<OStrV>(base_uri));
        using base_context_type = parsing_uri_context<stl::uint32_t, iterator>;

        base_context_type origin_context{.beg = base.begin(), .pos = base.begin(), .end = base.end()};
        parse_uri<Options>(origin_context);

        return parse_uri<Options>(the_url, origin_context.out);
    }

    /**
     * @brief Customization of uri components that holds all of the URI components with all the bells and the
     * whistles
     * @tparam StrT String or String View type
     * @tparam AllocT Allocator type (we're not extractinig it from StrT, because you may pass a string view)
     */
    template <istl::StringLike StrT, Allocator AllocT>
    struct uri_components<StrT, AllocT> {
        using string_type    = StrT;
        using allocator_type = AllocT;

        using scheme_type   = basic_scheme<string_type>;
        using host_type     = basic_host<string_type>;
        using username_type = basic_username<string_type>;
        using password_type = basic_password<string_type>;
        using port_type     = basic_port<string_type>;
        using path_type     = basic_path<string_type>;
        using fragment_type = basic_fragment<string_type>;
        using queries_type  = basic_queries<string_type, allocator_type>;

        using vec_type     = typename path_type::vector_type;
        using map_type     = typename queries_type::map_type;
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
        scheme_type   m_scheme{};
        username_type m_username{};
        password_type m_password{};
        host_type     m_hostname{};
        port_type     m_port{};
        path_type     m_path{};
        queries_type  m_queries{};
        fragment_type m_fragment{};

      public:
        // NOLINTBEGIN(*-macro-usage)
#define webpp_def(field)                                                                            \
    static constexpr bool is_##field##_modifiable = stl::same_as<decltype(m_##field), string_type>; \
    static constexpr bool is_##field##_vec        = stl::same_as<decltype(m_##field), vec_type>;    \
                                                                                                    \
    template <istl::StringLike NStrT = stl::string_view, typename... Args>                          \
    [[nodiscard]] constexpr NStrT get_##field(Args&&... args)                                       \
      const noexcept(!istl::ModifiableString<NStrT>) {                                              \
        return m_##field.template as_string<NStrT>(stl::forward<Args>(args)...);                    \
    }                                                                                               \
                                                                                                    \
    constexpr void clear_##field() noexcept {                                                       \
        m_##field.clear();                                                                          \
    }                                                                                               \
                                                                                                    \
    [[nodiscard]] constexpr bool has_##field() const noexcept {                                     \
        return m_##field.has_value();                                                               \
    }                                                                                               \
                                                                                                    \
    constexpr void set_##field(iterator beg, iterator end) noexcept(is_nothrow)                     \
        requires(is_##field##_modifiable)                                                           \
    {                                                                                               \
        m_##field.set_raw_value(beg, end);                                                          \
    }                                                                                               \
                                                                                                    \
    constexpr void set_##field(decltype(m_##field)&& str) noexcept(is_nothrow)                      \
        requires(is_##field##_modifiable)                                                           \
    {                                                                                               \
        m_##field = stl::move(str);                                                                 \
    }                                                                                               \
                                                                                                    \
    constexpr void set_lowered_##field(iterator beg, iterator end) noexcept(is_nothrow)             \
        requires(is_##field##_modifiable)                                                           \
    {                                                                                               \
        if constexpr (is_modifiable) {                                                              \
            ascii::lower_to(m_##field, beg, end);                                                   \
        } else {                                                                                    \
            set_##field(beg, end);                                                                  \
        }                                                                                           \
    }                                                                                               \
                                                                                                    \
    constexpr void set_lowered_##field(string_type str) noexcept(is_nothrow)                        \
        requires(is_##field##_modifiable)                                                           \
    {                                                                                               \
        if constexpr (is_modifiable) {                                                              \
            ascii::lower_to(m_##field, str.begin(), str.end());                                     \
        } else {                                                                                    \
            set_##field(stl::move(str));                                                            \
        }                                                                                           \
    }                                                                                               \
                                                                                                    \
    constexpr auto& field##_ref() noexcept {                                                        \
        return m_##field;                                                                           \
    }                                                                                               \
                                                                                                    \
    constexpr auto const& field##_ref() const noexcept {                                            \
        return m_##field;                                                                           \
    }



        webpp_def(scheme)
        webpp_def(username)
        webpp_def(password)
        webpp_def(hostname)
        webpp_def(port)
        webpp_def(path)
        webpp_def(queries)
        webpp_def(fragment)
#undef webpp_def

        // NOLINTEND(*-macro-usage)

        constexpr void set_hostname(iterator beg, iterator end) {
            istl::clear(m_hostname);
            if constexpr (is_modifiable) {
                istl::emplace_one(m_hostname, beg, end, m_hostname.get_allocator());
            } else {
                istl::emplace_one(m_hostname, beg, end);
            }
        }

        template <istl::String NStrT = stl::string, typename... Args>
        [[nodiscard]] constexpr NStrT get_hostname(Args&&... args) const {
            NStrT out{stl::forward<Args>(args)...};
            if (m_hostname.empty()) {
                return out;
            }
            for (auto pos = m_hostname.begin();;) {
                out += *pos;
                if (++pos == m_hostname.end()) {
                    break;
                }
                out += '.';
            }
            return out;
        }

        [[nodiscard]] constexpr bool has_credentials() const noexcept {
            return has_username() || has_password();
        }
    };

    template <istl::String StringType>
    struct basic_uri {
        using string_type    = stl::remove_cvref_t<StringType>;
        using char_type      = typename string_type::value_type;
        using allocator_type = typename string_type::allocator_type;

        using scheme_type   = basic_scheme<string_type>;
        using username_type = basic_username<string_type>;
        using password_type = basic_password<string_type>;
        using host_type     = basic_host<string_type>;
        using port_type     = basic_port<string_type>;
        using path_type     = basic_path<string_type>;
        using queries_type  = basic_queries<string_type>;
        using fragment_type = basic_fragment<string_type>;

        // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
        scheme_type   scheme{};
        username_type username{};
        password_type password{};
        host_type     host{};
        port_type     port{};
        path_type     path{};
        queries_type  queries{};
        fragment_type fragment{};

        // NOLINTEND(misc-non-private-member-variables-in-classes)


        template <typename T, typename Allocator = allocator_type>
            requires((URIString<T> || istl::StringViewifiable<T>) &&
                     stl::is_constructible_v<allocator_type, Allocator>)
        explicit(false) constexpr basic_uri(T&& uri_str, Allocator const& alloc = {}) // NOLINT(*-explicit-*)
          : scheme{alloc},
            username{alloc},
            password{alloc},
            host{alloc},
            port{alloc},
            path{alloc},
            queries{alloc},
            fragment{alloc} {
            extract_from(stl::forward<T>(uri_str));
        }

        template <typename Allocator = allocator_type>
            requires(stl::is_constructible_v<allocator_type, Allocator>)
        constexpr explicit basic_uri(Allocator const& alloc)
          : scheme{alloc},
            username{alloc},
            password{alloc},
            host{alloc},
            port{alloc},
            path{alloc},
            queries{alloc},
            fragment{alloc} {}

        constexpr basic_uri()
            requires(stl::is_default_constructible_v<string_type>)
        = default;
        constexpr basic_uri(basic_uri const&)                = default;
        constexpr basic_uri(basic_uri&&) noexcept            = default;
        constexpr basic_uri& operator=(basic_uri const&)     = default;
        constexpr basic_uri& operator=(basic_uri&&) noexcept = default;
        constexpr ~basic_uri()                               = default;

        template <istl::String StrT      = StringType,
                  typename AllocatorType = typename stl::remove_cvref_t<StrT>::allocator_type>
        [[nodiscard]] static constexpr auto create(AllocatorType const& alloc = AllocatorType{}) {
            using str_t = stl::remove_cvref_t<StrT>;
            return basic_uri<str_t>{alloc};
        }

        template <typename DefaultAlloc = allocator_type>
        constexpr auto get_allocator() const noexcept {
            return extract_allocator_of_or_default<DefaultAlloc>(
              scheme,
              username,
              password,
              host,
              port,
              path,
              queries,
              fragment);
        }

        constexpr void append_to(istl::String auto& out) {
            // estimate the size
            // todo: check if it has a good impact on performance or it's just in the way
            out.reserve(
              out.size() +                // the size of out itself
              scheme.size() +             // the scheme size
              username.size() +           // the username size
              password.size() + 1 +       // the password size + 1 character for @
              host.size() +               // host size
              port.size() + 1 +           // port size + 1 character for :
              path.raw_string_size() +    // path size
              queries.raw_string_size() + // queries size
              fragment.size()             // fragments size
            );
            scheme.to_string(out, true);
            username.to_string(out);
            password.to_string(out, true);
            host.append_to(out);
            if (port.is_default_port(scheme.view())) {
                port.to_string(out, true);
            }
            path.to_string(out);
            queries.to_string(out);
            fragment.to_string(out, true);
        }

        template <istl::String StrT = string_type>
        [[nodiscard]] constexpr StrT to_string() {
            StrT str{get_allocator<typename StrT::allocator_type>()};
            append_to(str);
            return str;
        }

        /**
         * This method returns an indication of whether or not the URI includes
         * any element that is part of the authority URI.
         * @return bool
         */
        [[nodiscard]] constexpr bool has_authority() const noexcept {
            return !host.empty() || !username.empty() || !password.empty() || !port.empty();
        }

        /**
         * This method resolves the given relative reference, based on the given
         * base URI, returning the resolved target URI.
         *
         * @param[in] relative_uri
         *     This describes how to get to the target starting at the base.
         *
         * @return
         *     The resolved target URI is returned.
         *
         * @note
         *     It only makes sense to call this method on an absolute URI
         *     (in which I mean, the base URI should be absolute,
         *     as in IsRelativeReference() should return false).
         */
        [[nodiscard]] constexpr basic_uri resolve(basic_uri const& relative_uri) const noexcept {
            // Resolve the reference by following the algorithm
            // from section 5.2.2 in
            // RFC 3986 (https://tools.ietf.org/html/rfc3986).
            auto target = create(relative_uri.get_allocator());
            if (relative_uri.has_scheme()) {
                target = relative_uri;
                target.path.normalize();
            } else {
                target.scheme   = this->scheme;
                target.fragment = relative_uri.fragment;
                if (!relative_uri.host.empty()) {
                    target.host     = relative_uri.host;
                    target.port     = relative_uri.port;
                    target.username = relative_uri.username;
                    target.password = relative_uri.password;
                    target.path     = relative_uri.path;
                    target.queries  = relative_uri.queries;
                    target.path.normalize();
                } else {
                    target.host     = this->host;
                    target.username = this->username;
                    target.password = this->password;
                    target.port     = this->port;
                    if (relative_uri.path.empty()) {
                        target.path = this->path;
                        if (!relative_uri.queries.empty()) {
                            target.queries = relative_uri.queries;
                        } else {
                            target.queries = this->queries;
                        }
                    } else {
                        target.queries = relative_uri.queries;
                        // RFC describes this as:
                        // "if (R.path starts-with "/") then"
                        if (relative_uri.path.is_path_absolute()) {
                            target.path = relative_uri.path;
                            target.path.normalize();
                        } else {
                            // RFC describes this as:
                            // "T.path = merge(Base.path, R.path);"
                            target.path = this->path;
                            if (target.path.size() > 1) {
                                target.path.pop_back();
                            }
                            stl::copy(relative_uri.path.cbegin(),
                                      relative_uri.path.cend(),
                                      stl::back_inserter(target.path));
                            target.path.normalize();
                        }
                    }
                }
            }

            return target;
        }

        template <istl::StringViewifiable StrT>
        constexpr uri_status set_scheme(StrT&& inp_str) noexcept {
            return scheme.parse_from(stl::forward<StrT>(inp_str));
        }

        template <URIString URIType>
        constexpr basic_uri& extract_from(URIType const& uri_str) {
            scheme   = uri_str.scheme();
            username = uri_str.username().value_or("");
            password = uri_str.password().value_or("");
            host     = uri_str.host();
            port     = uri_str.port();
            path     = uri_str.path();
            queries  = uri_str.queries();
            fragment = uri_str.fragment();
            return *this;
        }

        template <istl::StringViewifiable StrVT>
            requires(!URIString<StrVT>)
        constexpr basic_uri& extract_from(StrVT&& url_str) {
            auto const str_view = istl::string_viewify(stl::forward<StrVT>(url_str));
            using str_view_t    = stl::remove_cvref_t<decltype(str_view)>;
            uri_string<string_type const, str_view_t> const uri_str{str_view};
            extract_from(uri_str);
            return *this;
        }

        template <typename T>
            requires(URIString<T> || istl::StringViewifiable<T>)
        constexpr basic_uri& operator=(T&& uri_str) {
            extract_from(stl::forward<T>(uri_str));
            return *this;
        }
    };

    using uri = basic_uri<stl::string>;
} // namespace webpp::uri

#endif // WEBPP_URI_HPP
