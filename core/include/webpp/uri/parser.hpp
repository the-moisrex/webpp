//
// Created by moisrex on 10/30/20.
//

#ifndef WEBPP_PARSER_HPP
#define WEBPP_PARSER_HPP

#include "./structured_uri.hpp"

namespace webpp {

    template <typename ...T>
    struct uri_parser : structured_uri<T...> {


        /**
         * scheme    :    start=0       end=[0]
         * user_info :    start=[1]     end=[2]
         * host      :    start=[2|1]   end=[3|4|...]
         * port      :    start=[3]     end=[4]
         * path      :    start=[4]     end=[5]
         * query     :    start=[5]     end=[6]
         * fragment  :    start=[6]     end=[...]
         *
         * port_end == authority_end == path_start
         */

        /**
         * parse the scheme
         * this method will fill the "authority_start" and "scheme_end" vars
         */
        void parse_scheme() const noexcept {
            if (scheme_end != str_view_t::npos)
                return; // It's already parsed

            auto _data = this->string_view();

            // extracting scheme
            if (ascii::starts_with(_data, "//")) {
                authority_start = 2;
                scheme_end      = data.size(); // so we don't have to check again
                return;
            } else if (const auto colon = _data.find(':'); colon != str_view_t::npos) {
                auto m_scheme = _data.substr(0, colon);
                if (ALPHA<char_type>.contains(m_scheme[0]) &&
                                     m_scheme.substr(1).find_first_not_of(SCHEME_NOT_FIRST.string_view())) {
                    scheme_end = colon;

                    if (_data.substr(colon + 1, 2) == "//") {
                        authority_start = colon + 3;
                    } else {
                        // it should be a URN or an invalid URI at this point
                        authority_start = data.size();
                    }
                    return;
                }
            }

            scheme_end = authority_start = data.size();
        }

        /**
         * parse user info
         */
        void parse_user_info() const noexcept {
            if (user_info_end != str_view_t::npos)
                return; // It's already parsed

            parse_scheme(); // to get "authority_start"

            if (authority_start == data.size()) {
                user_info_end = data.size();
                return; // there's no user_info_end without authority_start
            }

            parse_path(); // to get "authority_end"

            auto _data = (stl::is_same_v<storred_str_t, str_view_t> ? data : str_view_t(data));

            user_info_end = _data.substr(authority_start, authority_end - authority_start).find_first_of('@');
            if (user_info_end == str_view_t::npos) {
                user_info_end = data.size();
            } else {
                user_info_end += authority_start;
            }
        }

        /**
         * parse path
         * this will make sure that the "authority_end" variable is filled
         */
        void parse_path() const noexcept {
            if (authority_end != str_view_t::npos)
                return; // It's already parsed

            parse_scheme(); // to get "authority_start"
            parse_query();  // to get "query_start"

            auto _data = this->string_view();

            auto starting_point =
              authority_start != data.size()
              ? authority_start
              : (scheme_end != data.size() && scheme_end != str_view_t::npos ? scheme_end : 0);
            authority_end = _data.substr(starting_point, query_start - starting_point).find_first_of('/');
            if (authority_end == str_view_t::npos) {
                authority_end = data.size();
            } else {
                authority_end += starting_point;
            }
        }

        /**
         * parse port
         * this makes sure that the "port_start" variable is filled
         */
        void parse_port() const noexcept {
            if (port_start != str_view_t::npos)
                return; // It's already parsed

            parse_user_info(); // to get "authority_start" and "user_info_end"

            if (authority_start == data.size()) {
                port_start = data.size();
                return; // there's no user_info_end without authority_start
            }

            parse_path(); // to get "authority_end"

            auto _data = this->string_view();

            auto starting_point = user_info_end != data.size() ? user_info_end : authority_start;
            port_start = _data.substr(starting_point, authority_end - starting_point).find_last_of(':');
            if (port_start == str_view_t::npos) {
                port_start = data.size(); // there's no port
            } else {
                port_start += starting_point;
                auto str_view = _data.substr(port_start + 1, authority_end - (port_start + 1));
                if (!ascii::is::digit(str_view)) {
                    port_start = data.size();
                }
            }
        }

        //        /**
        //         * parsing the authority parts of the basic_uri
        //         */
        //        void parse_authority_full() const noexcept {
        //
        //            parse_scheme();
        //            parse_path();
        //
        //            auto _data = (std::is_same_v<string_type,
        //            str_view_t>
        //                              ? data
        //                              : str_view_t(data))
        //                             .substr(authority_start, authority_end);
        //
        //            auto _authority_start = data.find("//");
        //            if (_authority_start != str_view_t::npos) {
        //                _data.remove_prefix(_authority_start + 2);
        //            }
        //            auto path_start = _data.find('/');
        //
        //            if (_authority_start != str_view_t::npos &&
        //            path_start != 0) {
        //
        //                auto port_start = _data.find(":", 0, path_start);
        //
        //                // extracting user info
        //                if (auto delim = _data.find("@", 0, path_start);
        //                    delim != str_view_t::npos) {
        //
        //                    pieces[_user_info] = delim;
        //                    _data.remove_prefix(delim + 1);
        //                }
        //
        //                // extracting host
        //
        //                /* IP future versions can be specified like this:
        //                 * (RFC 3986)
        //                 *
        //                 * IP-literal = "[" ( IPv6address / IPvFuture  ) "]"
        //                 * IPvFuture = "v" 1*HEXDIG "." 1*( unreserved /
        //                 sub-delims
        //                 * / ":" )
        //                 */
        //                if (ascii::starts_with(_data, '[')) { // IP
        //                Literal
        //                    if (_data.size() > 2 &&
        //                        _data[1] == 'v') { // IPv Future Number
        //                        if (auto dot_delim = _data.find('.');
        //                            dot_delim != str_view_t::npos) {
        //
        //                            auto ipvf_version = _data.substr(2,
        //                            dot_delim); if
        //                            (HEXDIG.contains(ipvf_version)) {
        //
        //                                if (auto ipvf_end = _data.find(']');
        //                                    ipvf_end !=
        //                                    str_view_t::npos) { auto
        //                                    ipvf =
        //                                        _data.substr(dot_delim + 1,
        //                                        ipvf_end);
        //                                    if
        //                                    (IPV_FUTURE_LAST_PART.contains(ipvf))
        //                                    {
        //                                        // returning the ipvf and it's
        //                                        // version
        //                                        _host = _data.substr(1,
        //                                        ipvf_end + 1);
        //                                    }
        //                                } else {
        //                                    // totally not a URI
        //                                    return;
        //                                }
        //                            }
        //                        }
        //                    } else if (_data.size() >= 5) { // IPv6
        //
        //                        if (auto ipv6_end = _data.find(']');
        //                            ipv6_end != str_view_t::npos) {
        //
        //                            if (auto ipv6_view = _data.substr(1,
        //                            ipv6_end);
        //                                is::ipv6(ipv6_view)) {
        //                                // TODO: probably use
        //                                std::variant<ipv6,
        //                                // ipv4, string>
        //                                _host = ipv6_view;
        //                                _data.remove_prefix(ipv6_end + 1);
        //                            }
        //                        } else {
        //                            return; // totally not a valid URI
        //                        }
        //                    }
        //                } else { // Not IP Literal
        //                    auto port_or_path_start =
        //                        port_start != str_view_t::npos ?
        //                        port_start
        //                                                             :
        //                                                             path_start;
        //                    auto hostname = _data.substr(0,
        //                    port_or_path_start);
        //                    // we're not going to decode hostname here. We'll
        //                    do
        //                    // this in another method because this function is
        //                    //  and will only return const stuff
        //
        //                    // we have our answer but we will check for the
        //                    // correctness of the hostname now
        //                    auto HOSTNAME_CHARS =
        //                        charset(REG_NAME_NOT_PCT_ENCODED,
        //                        charset('%'));
        //                    if (HOSTNAME_CHARS.contains(hostname)) {
        //                        _host = hostname;
        //                        if (port_or_path_start !=
        //                        str_view_t::npos)
        //                            _data.remove_prefix(port_or_path_start);
        //                        else {
        //                            return;
        //                        }
        //                    }
        //                }
        //
        //                // extracting port
        //                if (port_start != str_view_t::npos) {
        //                    auto port_end =
        //                        _data.find_first_not_of(DIGIT.string_view());
        //                    _port = _data.substr(port_start + 1, port_end);
        //                    _data.remove_prefix(port_end);
        //                }
        //            }
        //        }

        /**
         * parse fragment (it finds fragment_start)
         */
        void parse_fragment() const noexcept {
            if (fragment_start != str_view_t::npos)
                return; // It's already parsed

            auto _data = this->string_view();

            fragment_start = _data.find_first_of('#');
            if (fragment_start == str_view_t::npos) {
                fragment_start = data.size();
            }
        }

        /**
         * parse query; it ensures that query_start and fragment_start are
         * changed
         */
        void parse_query() const noexcept {
            if (query_start != str_view_t::npos)
                return; // there's nothing to do

            parse_fragment();

            auto _data  = this->string_view();
            query_start = _data.substr(0, fragment_start).find_first_of('?');
            if (query_start == str_view_t::npos) {
                query_start = data.size();
            }
        }

        /**
         * parse the host
         */
        void parse_host() const noexcept {
            parse_user_info(); // to get "authority_start" and "user_info_end"
            parse_port();      // to get "port_start" and hopefully "authority_end"
            parse_path();      // to make sure we have "authority_end"
        }

        /**
         * Remove the cache and make sure calling the functions will cause
         * re-parsing the uri.
         */
        inline void unparse() const noexcept {
            scheme_end = authority_start = user_info_end = port_start = authority_end = query_start =
            fragment_start                                                          = str_view_t::npos;
        }

        /**
         * Replace the specified part with the specified replacement
         */
        void replace_value(stl::size_t start, stl::size_t len, str_view_t const& replacement) noexcept {
            static_assert(is_mutable(), "You cannot change a const_uri (string_view is not "
                                        "modifiable)");
            if (start == str_view_t::npos || len == str_view_t::npos || (len == 0 && replacement.empty()))
                return;
            typename traits_type::stringstream_type _data;
            _data << substr(0, start) << replacement << substr(stl::min(data.size(), start + len));
            data = _data.str();
            unparse();
            // TODO: you may want to not unparse everything
        }


    };

}
#endif // WEBPP_PARSER_HPP
