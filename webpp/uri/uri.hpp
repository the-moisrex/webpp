// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_HPP
#define WEBPP_URI_HPP

#include "../std/string_view.hpp"
#include "fragment.hpp"
#include "host.hpp"
#include "path.hpp"
#include "port.hpp"
#include "queries.hpp"
#include "scheme.hpp"
#include "uri_string.hpp"
#include "user_info.hpp"

namespace webpp::uri {

    template <typename... T>
    static constexpr void continue_parsing_uri(uri::parsing_uri_context<T...>& ctx) noexcept(
      uri::parsing_uri_context<T...>::is_nothrow) {
        // todo: it's cool and all, but remove the while loop for more possible optimizations by the compiler
        while (!has_error(ctx.status)) {
            switch (get_value(ctx.status)) {
                using enum uri_status;
                case valid: return; // we're done parsing
                case valid_punycode: break;
                case valid_authority: parse_authority(ctx); break;
                case valid_host: parse_host(ctx); break;
                case valid_port: parse_port(ctx); break;
                case valid_opaque_path: parse_opaque_path(ctx); break;
                case valid_path: parse_path(ctx); break;
                case valid_queries: parse_queries(ctx); break;
                case valid_fragment: parse_fragment(ctx); break;
                default: stl::unreachable();
            }
        }
    }

    template <typename... T>
    static constexpr void
    parse_uri(uri::parsing_uri_context<T...>& ctx) noexcept(uri::parsing_uri_context<T...>::is_nothrow) {
        parse_scheme(ctx);
        continue_parsing_uri(ctx);
    }


    template <istl::String StringType>
    struct basic_uri {
        using string_type    = stl::remove_cvref_t<StringType>;
        using char_type      = typename string_type::value_type;
        using allocator_type = typename string_type::allocator_type;

        using scheme_type    = basic_scheme<string_type>;
        using user_info_type = basic_user_info<string_type>;
        using host_type      = basic_host<string_type>;
        using port_type      = basic_port<string_type>;
        using path_type      = basic_path<string_type>;
        using queries_type   = basic_queries<string_type>;
        using fragment_type  = basic_fragment<string_type>;

        // NOLINTBEGIN(misc-non-private-member-variables-in-classes)
        scheme_type    scheme{};
        user_info_type user_info{};
        host_type      host{};
        port_type      port{};
        path_type      path{};
        queries_type   queries{};
        fragment_type  fragment{};
        // NOLINTEND(misc-non-private-member-variables-in-classes)

        template <istl::String StrT      = StringType,
                  typename AllocatorType = typename stl::remove_cvref_t<StrT>::allocator_type>
        [[nodiscard]] static constexpr auto create(AllocatorType const& alloc = AllocatorType{}) {
            using str_t = stl::remove_cvref_t<StrT>;
            return basic_uri<str_t>{.scheme{alloc},
                                    .user_info{.username = str_t{alloc}, .password = str_t{alloc}},
                                    .host{alloc},
                                    .port{alloc},
                                    .path{alloc},
                                    .queries{alloc},
                                    .fragment{alloc}};
        }


        template <typename DefaultAlloc = allocator_type>
        constexpr auto get_allocator() const noexcept {
            return extract_allocator_of_or_default<DefaultAlloc>(scheme,
                                                                 user_info.username,
                                                                 user_info.password,
                                                                 host,
                                                                 port,
                                                                 path,
                                                                 queries,
                                                                 fragment);
        }


        constexpr void append_to(istl::String auto& out) {
            // estimate the size
            // todo: check if it has a good impact on performance or it's just in the way
            out.reserve(out.size() +                    // the size of out itself
                        scheme.size() +                 // the scheme size
                        user_info.username.size() +     // the username size
                        user_info.password.size() + 1 + // the password size + 1 character for @
                        host.size() +                   // host size
                        port.size() + 1 +               // port size + 1 character for :
                        path.raw_string_size() +        // path size
                        queries.raw_string_size() +     // queries size
                        fragment.size()                 // fragments size
            );
            scheme.append_to(out);
            user_info.append_to(out);
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
            return !host.empty() || !user_info.empty() || !port.empty();
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
        uri_status set_scheme(StrT&& inp_str) noexcept {
            auto const str     = istl::string_viewify(stl::forward<StrT>(inp_str));
            auto       ptr     = str.data();
            auto const ptr_end = ptr + str.size();
            auto const status  = scheme.parse_from(stl::forward<StrT>(inp_str));
            return status;
        }

        template <URIString URIType>
        constexpr basic_uri& extract_from(URIType const& uri_str) {
            scheme             = uri_str.scheme();
            user_info.username = uri_str.username().value_or("");
            user_info.password = uri_str.password().value_or("");
            host               = uri_str.host();
            port               = uri_str.port();
            path               = uri_str.path();
            queries            = uri_str.queries();
            fragment           = uri_str.fragment();
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
