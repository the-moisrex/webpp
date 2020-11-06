// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_HPP
#define WEBPP_URI_HPP

#include "./fragment.hpp"
#include "./host.hpp"
#include "./path.hpp"
#include "./port.hpp"
#include "./queries.hpp"
#include "./scheme.hpp"
#include "./user_info.hpp"

namespace webpp::uri {


    template <istl::String StringType>
    struct basic_uri {
        using string_type    = stl::remove_cvref_t<StringType>;
        using char_type      = typename string_type::value_type;
        using allocator_type = typename stirng_type::allocator_type;

        using scheme_type    = basic_scheme<string_type>;
        using user_info_type = basic_uri_info<string_type>;
        using host_type      = basic_host<string_type>;
        using port_type      = basic_port<string_type>;
        using path_type      = basic_path<string_type>;
        using queries_type   = basic_queries<string_type>;
        using fragment_type  = basic_fragment<stirng_type>;

        scheme_type    scheme{};
        user_info_type user_info{};
        host_type      host{};
        port_type      port{};
        path_type      path{};
        queries_type   queries{};
        fragment_type  fragment{};

        template <istl::String StrT      = StringType,
                  typename AllocatorType = typename stl::remove_cvref_t<StrT>::allocator_type>
        [[nodiscard]] static constexpr auto create(AllocatorType const& alloc) {
            basic_uri<stl::remove_cvref_t<StrT>> u{.scheme{alloc},
                                                   .user_info{.username{alloc}, .password{alloc}},
                                                   .host{alloc},
                                                   .port{alloc},
                                                   .path{alloc},
                                                   .queries{alloc},
                                                   .fragment{alloc}};
            return u;
        }


        void append_to(istl::String auto& out) {
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
        [[nodiscard]] basic_uri resolve(const basic_uri& relative_uri) const noexcept {
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
                        if (relative_uri.path.is_absolute()) {
                            target.path = relative_uri.path;
                            target.path.normalize();
                        } else {
                            // RFC describes this as:
                            // "T.path = merge(Base.path, R.path);"
                            target.path = this->path;
                            if (target.path.size() > 1) {
                                target.path.pop_back();
                            }
                            stl::copy(relative_uri.path.cbegin(), relative_uri.path.cend(),
                                      stl::back_inserter(target.path));
                            target.path.normalize();
                        }
                    }
                }
            }

            return target;
        }
    };

    using uri = basic_uri<stl::string>;
} // namespace webpp::uri

#endif // WEBPP_URI_HPP
