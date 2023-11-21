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

    template <uri_parsing_options Options = uri_parsing_options{}, typename... T>
    static constexpr void
    continue_parsing_uri(parsing_uri_context<T...>& ctx) noexcept(parsing_uri_context<T...>::is_nothrow) {
        while (!has_error(ctx.status)) {
            switch (get_value(ctx.status)) {
                using enum uri_status;
                case valid: return; // we're done parsing
                case valid_punycode: break;
                case valid_authority: parse_authority<Options>(ctx); break;
                case valid_file_host: parse_file_host<Options>(ctx); break;
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

    template <typename... T>
    static constexpr void
    parse_uri(parsing_uri_context<T...>& ctx) noexcept(parsing_uri_context<T...>::is_nothrow) {
        continue_parsing_uri(ctx);
    }


    template <istl::StringView StrV = stl::string_view>
    static constexpr auto parse_uri(StrV str) noexcept {
        using iterator     = typename StrV::const_iterator;
        using context_type = parsing_uri_context<StrV, iterator>;
        context_type context{.beg = str.begin(),
                             .pos = str.begin(), // current position is start
                             .end = str.end()};
        parse_uri(context);
        return context;
    }


    template <istl::StringLike StrT, typename SegType>
    static constexpr auto
    parse_uri(StrT const&                    the_url,
              uri_components<SegType> const& origin_context) noexcept(istl::StringView<StrT>) {
        using iterator             = typename StrT::const_iterator;
        using base_components_type = uri_components<SegType>;
        using base_seg_type        = typename base_components_type::seg_type;
        using context_type         = parsing_uri_context<StrT, iterator, base_seg_type>;

        context_type context{.beg  = the_url.begin(),
                             .pos  = the_url.begin(), // current position is start
                             .end  = the_url.end(),
                             .base = origin_context};
        parse_uri(context);
        return context;
    }

    template <istl::StringLike StrT, istl::StringViewifiable OStrV>
    static constexpr auto parse_uri(StrT const& the_url,
                                    OStrV&&     origin_url) noexcept(istl::StringView<StrT>) {
        using iterator = typename StrT::const_iterator;
        static_assert(
          stl::same_as<iterator, typename OStrV::const_iterator>,
          "Origin's string's char type must be the same as the specified URI's string's char type.");
        auto const origin       = istl::string_viewify(stl::forward<OStrV>(origin_url));
        using base_context_type = parsing_uri_context<stl::uint32_t, iterator>;

        base_context_type origin_context{.beg = origin.begin(), .pos = origin.begin(), .end = origin.end()};
        parse_uri(origin_context);

        return parse_uri(the_url, origin_context.out);
    }


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
        constexpr explicit(false) basic_uri(T&& uri_str, Allocator const& alloc = {})
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
            return extract_allocator_of_or_default<DefaultAlloc>(scheme,
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
            out.reserve(out.size() +                // the size of out itself
                        scheme.size() +             // the scheme size
                        username.size() +           // the username size
                        password.size() + 1 +       // the password size + 1 character for @
                        host.size() +               // host size
                        port.size() + 1 +           // port size + 1 character for :
                        path.raw_string_size() +    // path size
                        queries.raw_string_size() + // queries size
                        fragment.size()             // fragments size
            );
            scheme.append_to(out);
            username.append_to(out);
            password.append_to(out);
            host.append_to(out);
            port.append_to(out);
            path.append_to(out);
            queries.append_to(out);
            fragment.append_to(out);
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
        [[nodiscard]] constexpr basic_uri resolve(const basic_uri& relative_uri) const noexcept {
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
                    target.host      = relative_uri.host;
                    target.port      = relative_uri.port;
                    target.user_info = relative_uri.user_info;
                    target.path      = relative_uri.path;
                    target.queries   = relative_uri.queries;
                    target.path.normalize();
                } else {
                    target.host      = this->host;
                    target.user_info = this->user_info;
                    target.port      = this->port;
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
            uri_string<const string_type, str_view_t> const uri_str{str_view};
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
