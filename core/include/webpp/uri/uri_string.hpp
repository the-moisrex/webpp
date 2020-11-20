#ifndef WEBPP_UTILS_URI_H
#define WEBPP_UTILS_URI_H

#include "../convert/casts.hpp"
#include "../std/map.hpp"
#include "../std/optional.hpp"
#include "../std/vector.hpp"
#include "../strings/charset.hpp"
#include "../strings/to_case.hpp"
#include "../traits/traits_concepts.hpp"
#include "../utils/allocators.hpp"
#include "../utils/ipv4.hpp"
#include "../utils/ipv6.hpp"
#include "../validators/validators.hpp"
#include "./details/constants.hpp"
#include "./path.hpp"
#include "./queries.hpp"
#include "./encoding.hpp"


/**
 * URI features that we might need separately:
 *   - Full URI parser/creator
 *     - scheme
 *     - user info
 *     - host
 *     - port
 *     - path
 *     - queries
 *     - fragments
 *
 * There are two types of URI types:
 *   1. String Based: designed for parsing the URIs
 *   2. Structured  : designed to change or create URIs
 */

namespace webpp::uri {

    namespace is {
        /**
         * @brief check if scheme is correct or not
         */
        [[nodiscard]] constexpr bool scheme(istl::StringView auto&& _scheme) noexcept {
            using str_v = stl::remove_cvref_t<decltype(_scheme)>;
            return ALPHA<typename str_v::value_type>.contains(_scheme);
        }

    } // namespace is

    /**
     * This class represents a Uniform Resource Identifier (URI),
     * as defined in RFC 3986 (https://tools.ietf.org/html/rfc3986).
     *
     * Most URIs will never change in their life time (at least in webpp
     * project) and they mostly used to get details of the URL we have as a
     * string; so I decided that probably half of the calculations can be done
     * at compile time; so fo that point, I reimplemented the URI class with
     *  and string_view in mind.
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
    template <Traits TraitsType = std_traits, bool Mutable = true>
    struct uri_string : public allocator_holder<typename TraitsType::template allocator<typename TraitsType::char_type>>  {
        using traits_type       = TraitsType;
        using string_type       = typename traits_type::string_type;
        using string_view_type  = typename traits_type::string_view_type;
        using char_type         = typename traits_type::char_type;
        using allocator_type    = typename traits_type::template allocator<char_type>;
        using alloc_holder_type = allocator_holder<allocator_type>;

        /**
         * Getting the appropriate string type to use.
         * If the specified string type cannot be changed, the string_view will be used,
         * otherwise, string itself.
         */
        using storred_str_t = auto_string_type<TraitsType, Mutable>;


      private:
        /**
         * This is the whole url (if we need to own the uri ourselves)
         */
        storred_str_t data{};

        mutable stl::size_t scheme_end      = string_view_type::npos;
        mutable stl::size_t authority_start = string_view_type::npos;
        mutable stl::size_t user_info_end   = string_view_type::npos;
        mutable stl::size_t port_start      = string_view_type::npos;
        mutable stl::size_t authority_end   = string_view_type::npos;
        mutable stl::size_t query_start     = string_view_type::npos;
        mutable stl::size_t fragment_start  = string_view_type::npos;

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
            if (scheme_end != string_view_type::npos)
                return; // It's already parsed

            auto _data = string_view();

            // extracting scheme
            if (ascii::starts_with(_data, "//")) {
                authority_start = 2;
                scheme_end      = data.size(); // so we don't have to check again
                return;
            } else if (const auto colon = _data.find(':'); colon != string_view_type::npos) {
                auto m_scheme = _data.substr(0, colon);
                if (ALPHA<char_type>.contains(m_scheme[0]) &&
                    m_scheme.substr(1).find_first_not_of(details::SCHEME_NOT_FIRST<char_type>.string_view())) {
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
            if (user_info_end != string_view_type::npos)
                return; // It's already parsed

            parse_scheme(); // to get "authority_start"

            if (authority_start == data.size()) {
                user_info_end = data.size();
                return; // there's no user_info_end without authority_start
            }

            parse_path(); // to get "authority_end"

            auto _data = (stl::is_same_v<storred_str_t, string_view_type> ? data : string_view_type(data));

            user_info_end = _data.substr(authority_start, authority_end - authority_start).find_first_of('@');
            if (user_info_end == string_view_type::npos) {
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
            if (authority_end != string_view_type::npos)
                return; // It's already parsed

            parse_scheme(); // to get "authority_start"
            parse_query();  // to get "query_start"

            auto _data = string_view();

            auto starting_point =
              authority_start != data.size()
              ? authority_start
              : (scheme_end != data.size() && scheme_end != string_view_type::npos ? scheme_end : 0);
            authority_end = _data.substr(starting_point, query_start - starting_point).find_first_of('/');
            if (authority_end == string_view_type::npos) {
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
            if (port_start != string_view_type::npos)
                return; // It's already parsed

            parse_user_info(); // to get "authority_start" and "user_info_end"

            if (authority_start == data.size()) {
                port_start = data.size();
                return; // there's no user_info_end without authority_start
            }

            parse_path(); // to get "authority_end"

            auto _data = string_view();

            auto starting_point = user_info_end != data.size() ? user_info_end : authority_start;
            port_start = _data.substr(starting_point, authority_end - starting_point).find_last_of(':');
            if (port_start == string_view_type::npos) {
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
        //            string_view_type>
        //                              ? data
        //                              : string_view_type(data))
        //                             .substr(authority_start, authority_end);
        //
        //            auto _authority_start = data.find("//");
        //            if (_authority_start != string_view_type::npos) {
        //                _data.remove_prefix(_authority_start + 2);
        //            }
        //            auto path_start = _data.find('/');
        //
        //            if (_authority_start != string_view_type::npos &&
        //            path_start != 0) {
        //
        //                auto port_start = _data.find(":", 0, path_start);
        //
        //                // extracting user info
        //                if (auto delim = _data.find("@", 0, path_start);
        //                    delim != string_view_type::npos) {
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
        //                            dot_delim != string_view_type::npos) {
        //
        //                            auto ipvf_version = _data.substr(2,
        //                            dot_delim); if
        //                            (HEXDIG.contains(ipvf_version)) {
        //
        //                                if (auto ipvf_end = _data.find(']');
        //                                    ipvf_end !=
        //                                    string_view_type::npos) { auto
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
        //                            ipv6_end != string_view_type::npos) {
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
        //                        port_start != string_view_type::npos ?
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
        //                        string_view_type::npos)
        //                            _data.remove_prefix(port_or_path_start);
        //                        else {
        //                            return;
        //                        }
        //                    }
        //                }
        //
        //                // extracting port
        //                if (port_start != string_view_type::npos) {
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
            if (fragment_start != string_view_type::npos)
                return; // It's already parsed

            auto _data = string_view();

            fragment_start = _data.find_first_of('#');
            if (fragment_start == string_view_type::npos) {
                fragment_start = data.size();
            }
        }

        /**
         * parse query; it ensures that query_start and fragment_start are changed
         */
        void parse_query() const noexcept {
            if (query_start != string_view_type::npos)
                return; // there's nothing to do

            parse_fragment();

            auto _data  = string_view();
            query_start = _data.substr(0, fragment_start).find_first_of('?');
            if (query_start == string_view_type::npos) {
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
            fragment_start                                                          = string_view_type::npos;
        }

        /**
         * Replace the specified part with the specified replacement
         */
        void replace_value(stl::size_t start, stl::size_t len, auto&& replacement) noexcept {
            static_assert(is_mutable(), "You cannot change a const_uri (string_view is not modifiable)");
            if (start == string_view_type::npos || len == string_view_type::npos || (len == 0 && replacement.empty()))
                return;
            // todo: check performance of this
            auto const after_replacement_start = stl::min(data.size(), start + len);
            data.reserve(start + replacement.size() + (data.size() - after_replacement_start));
            data = substr(0, start);
            data += stl::forward<decltype(replacement)>(replacement);
            data += substr(after_replacement_start);
            unparse();
            // TODO: you may want to not unparse everything
        }



      public:



        explicit uri_string(auto&& alloc) : alloc_holder_type{alloc} {}
        constexpr uri_string(const char_type* u) noexcept : data(u) {}


        /**
         * @brief parse from string, it will trim the spaces for generality too
         * @param string_view URI string
         */
        constexpr uri_string(storred_str_t const& u) noexcept : data(u) {}

        /**
         * If the user uses this
         */
        constexpr uri_string(storred_str_t&& u) noexcept : data(stl::move(u)) {}

        constexpr uri_string(uri_string const& bu) noexcept
        : data{bu.data},
        scheme_end{bu.scheme_end},
        authority_start{bu.authority_start},
        user_info_end{bu.user_info_end},
        port_start{bu.port_start},
        authority_end{bu.authority_end},
        query_start{bu.query_start},
        fragment_start{bu.fragment_start} {}

        constexpr uri_string(uri_string&& bu) noexcept
        : data{stl::move(bu.data)},
        scheme_end{stl::move(bu.scheme_end)},
        authority_start{stl::move(bu.authority_start)},
        user_info_end{stl::move(bu.user_info_end)},
        port_start{stl::move(bu.port_start)},
        authority_end{stl::move(bu.authority_end)},
        query_start{stl::move(bu.query_start)},
        fragment_start{stl::move(bu.fragment_start)} {}



        // assignment operators
        uri_string& operator=(uri_string const& u) noexcept {
            data            = u.data;
            scheme_end      = u.scheme_end;
            authority_start = u.authority_start;
            user_info_end   = u.user_info_end;
            port_start      = u.port_start;
            authority_end   = u.authority_end;
            query_start     = u.query_start;
            fragment_start  = u.fragment_start;
        }

        uri_string& operator=(uri_string&& u) noexcept {
            data            = stl::move(u.data);
            scheme_end      = stl::move(u.scheme_end);
            authority_start = stl::move(u.authority_start);
            user_info_end   = stl::move(u.user_info_end);
            port_start      = stl::move(u.port_start);
            authority_end   = stl::move(u.authority_end);
            query_start     = stl::move(u.query_start);
            fragment_start  = stl::move(u.fragment_start);
        }

        uri_string& operator=(storred_str_t const& u) noexcept {
            data = u;
            unparse();
            return *this;
        }

        uri_string& operator=(storred_str_t&& u) noexcept {
            data = stl::move(u);
            unparse();
            return *this;
        }

        constexpr uri_string(allocator_type const& alloc = allocator_type{}) noexcept
          : alloc_holder_type{alloc} {
            static_assert(is_mutable(), "You can't modify this basic_uri, there's no point in "
                                        "default constructing this class since it can't be changed"
                                        " and thus can't be used in any way.");
        }

        ~uri_string() noexcept = default;

        static constexpr bool is_mutable() noexcept {
            return Mutable;
        }


        template <bool UMutable = Mutable>
        bool operator==(const uri_string<TraitsType, UMutable>& other) const noexcept {
            return str() == other.str();
        }

        template <bool UMutable = Mutable>
        bool operator!=(const uri_string<TraitsType, UMutable>& other) const noexcept {
            return str() != other.str();
        }

        bool operator==(string_view_type const& u) const noexcept {
            return str() == u;
        }

        bool operator!=(string_view_type const& u) const noexcept {
            return str() != u;
        }

        /**
         * Get a part of the uri
         * @param start
         * @param len
         * @return string_view_type
         */
        [[nodiscard]] string_view_type substr(stl::size_t const& start = 0,
                                        stl::size_t const& len   = string_view_type::npos) const noexcept {
            if (len == 0)
                return {};
            if constexpr (stl::is_same_v<storred_str_t, string_view_type>) {
                return data.substr(start, len);
            } else {
                return string_view_type(data).substr(start, len);
            }
        }

        /**
         * @brief this function is the same as "encodeURI" in javascript
         */
        void encode_to(istl::String auto& output) const noexcept {
            encode_uri_component(str(), output, details::ALLOWED_CHARACTERS_IN_URI<char_type>);
        }

        /**
         * @brief this function is the same as "decodeURI" in javascript
         * @return this function will return an optional<string> object. it will
         * be nullopt when the uri is not valid and has invalid characters
         */
        [[nodiscard]] bool decode_to(istl::String auto& output) const noexcept {
            return decode_uri_component(str(), output, details::ALLOWED_CHARACTERS_IN_URI<char_type>);
        }



        ///////////////////////////////////////////// Scheme /////////////////////////////////////////////


        /**
         * @brief checks if the URI is a relative reference
         */
        [[nodiscard]] bool is_relative_reference() const noexcept {
            return !has_scheme();
        }

        /**
         * @brief check if the specified uri has a scheme or not
         */
        [[nodiscard]] bool has_scheme() const noexcept {
            parse_scheme();
            return !(scheme_end == data.size() && scheme_end != 0);
        }

        /**
         * @brief scheme
         * @return get scheme
         */
        [[nodiscard]] string_view_type scheme() const noexcept {
            parse_scheme();
            return scheme_end == data.size() ? string_view_type() : substr(0, scheme_end);
        }

        /**
         * @brief set scheme
         * @param _scheme
         * @throws logic_error if uri is const
         */
        auto& scheme(string_view_type m_scheme) {
            if (ascii::ends_with(m_scheme, ':'))
                m_scheme.remove_suffix(1);
            if (!is::scheme(m_scheme))
                throw stl::invalid_argument("The specified scheme is not valid");
            parse_scheme();
            if (scheme_end != data.size()) {
                replace_value(0,
                                    m_scheme.empty() && data.size() > scheme_end + 1 &&
                                    data[scheme_end] == ':'
                                    ? scheme_end + 1
                                    : scheme_end,
                                    m_scheme);
            } else {
                // the URI doesn't have a scheme now, we have to put it in the right place
                auto scheme_colon = m_scheme.empty() ? "" : string_type(m_scheme) + ':';
                if (authority_start != data.size()) {
                    replace_value(0, 0,
                                        scheme_colon + (ascii::starts_with(data, "//") ? "" : "//"));
                } else {
                    // It's a URN (or URN like URI)
                    replace_value(0, 0, scheme_colon);
                }
            }
            return *this;
        }

        /**
         * @brief clear scheme from uri
         */
        auto& clear_scheme() noexcept {
            return scheme("");
        }

        ///////////////////////////////////////////// User Info /////////////////////////////////////////////



        /**
         * @brief checks if the uri has user info or not
         */
        [[nodiscard]] bool has_user_info() const noexcept {
            parse_user_info();
            return user_info_end != data.size() && authority_start != data.size();
        }

        /**
         * @brief get the user info or an empty value
         */
        [[nodiscard]] string_view_type user_info_raw() const noexcept {
            parse_user_info();
            return (user_info_end == data.size() || authority_start == data.size())
                   ? string_view_type()
                   : substr(authority_start, user_info_end - authority_start);
        }

        /**
         * @brief decode user_info and return it as a string
         */
        [[nodiscard]] stl::optional<string_type> user_info() const noexcept {
            string_type out{this->get_allocator()};
            if (decode_uri_component(user_info_raw(), out, details::USER_INFO_NOT_PCT_ENCODED<char_type>)) {
                return out;
            }
            return stl::nullopt;
        }

        /**
         * @brief set the user info if it's possible
         */
        uri_string& user_info(string_view_type const& info) noexcept {
            parse_user_info();
            string_type encoded_info{this->get_allocator()};
            encode_uri_component(info, encoded_info, details::USER_INFO_NOT_PCT_ENCODED<char_type>);
            if (user_info_end == data.size() || authority_start == data.size()) {
                // the URI already has user info, I just have to replace it
                replace_value(authority_start, user_info_end - authority_start, encoded_info);
            } else {
                // I don't know where is it, let's find it

                if (authority_start == data.size()) {
                    // this URI doesn't have authority in it, but I'm going to
                    // insert authority into it because the user is obviously
                    // demanding it; of course after this, the URI won't be a
                    // valid URI until the user sets at lease scheme too.

                    parse_scheme(); // to get "scheme_end"
                    if (scheme_end == data.size()) {
                        // there's no scheme either
                        replace_value(0, 0, string_type("//") + encoded_info + "@");
                    } else {
                        // there's scheme and we have to put it after that
                        replace_value(scheme_end + 1, 0, string_type("//") + encoded_info + "@");
                    }
                } else {
                    // there's authority start but there's no user_info_end
                    replace_value(authority_start, 0, encoded_info + "@");
                }
            }
            return *this;
        }

        /**
         * @brief clears the user info if exists
         */
        auto& clear_user_info() noexcept {
            return user_info("");
        }


        /**
         * Check if the user info has a username in it or not
         * @return bool
         */
        [[nodiscard]] bool has_username() const noexcept {
            return user_info_end != data.size();
        }

        /**
         * Get the username in the user info if it exists or otherwise an empty
         * string view
         * @return
         */
        [[nodiscard]] string_view_type username_raw() const noexcept {
            auto _userinfo = user_info_raw();
            if (auto colon = _userinfo.find(':'); colon != string_view_type::npos)
                _userinfo.remove_suffix(_userinfo.size() - colon);
            return _userinfo;
        }

        /**
         * Get the host version of the username if it exists
         * @return
         */
        [[nodiscard]] stl::optional<string_type> username() const noexcept {
            string_type out{this->get_allocator()};
            if (decode_uri_component(username_raw(), out, details::USER_INFO_NOT_PCT_ENCODED<char_type>)) {
                return out;
            }
            return stl::nullopt;
        }

        /**
         * An indication of whether or not the user info has a password
         * @return
         */
        [[nodiscard]] bool has_password() const noexcept {
            return user_info_raw().find(':') != string_view_type::npos;
        }

        /**
         * The password in the user info
         * @return
         */
        [[nodiscard]] string_view_type password_raw() const noexcept {
            auto _user_info = user_info_raw();
            if (auto found = _user_info.find(':'); found != string_view_type::npos) {
                return _user_info.substr(found + 1);
            }
            return {};
        }

        /**
         * The host version of the password
         * @return
         */
        [[nodiscard]] stl::optional<string_type> password() const noexcept {
            string_type out{this->get_allocator()};
            if (decode_uri_component(password_raw(), out, details::USER_INFO_NOT_PCT_ENCODED<char_type>)){
                return out;
            }
            return stl::nullopt;
        }


        ///////////////////////////////////////////// Host /////////////////////////////////////////////

        /**
         * @brief return host as an string_view
         * @return string_view
         */
        [[nodiscard]] string_view_type host_raw() const noexcept {
            parse_host();
            if (authority_start == data.size()) {
                // there will not be a host without the authority_start
                return {};
            }

            stl::size_t start, len;

            // you know I could do this in one line of code, but I did this
            // because I don't want you to curse me :)

            // we have authority_start, let's check user_info and port too
            if (user_info_end == data.size()) {
                // there's no user info
                start = authority_start;
            } else {
                // there's a user info
                start = user_info_end;
            }

            if (port_start != data.size()) {
                // but there's a port
                len = port_start - start;
            } else {
                // there's no port either
                if (authority_end != data.size()) {
                    // there's a path
                    len = authority_end - start;
                } else {
                    // there's no path either
                    len = data.size() - 1; // till the end
                }
            }

            return substr(start, len);
        }

        /**
         * @brief returns const_ipv4/const_ipv6/hostname; if the URI doesn't
         * include a valid ip/hostname you'll get an empty string. this method
         * will only return the hostname/ip if it's in the correct format and
         * doesn't include invalid syntax.
         * @return
         */
        [[nodiscard]] stl::variant<ipv4<traits_type>, ipv6<traits_type>, string_view_type>
        host_structured() const noexcept {
            auto _host = host_raw();
            if (webpp::is::ipv4(_host))
                return ipv4<traits_type>(_host);
            if (webpp::is::ipv6(_host))
                return ipv6<traits_type>(_host);
            return _host;
        }

        /**
         * @brief get the host version of hostname/ip of the uri or an empty
         * string if the specified URI does not include a hostname/ip or its
         * hostname has the wrong character encodings.
         * @return string
         */
        [[nodiscard]] stl::optional<string_type> host() const noexcept {
            string_type d_host{this->get_allocator()};
            if (decode_uri_component(host_raw(), d_host, details::REG_NAME_NOT_PCT_ENCODED<char_type>))
                return d_host;
            return stl::nullopt;
        }

        /**
         * @brief this method will check if the hostname/ip exists in the uri or not.
         * @return true if it can't find a hostname/ip in the uri or it's empty
         */
        [[nodiscard]] bool has_host() const noexcept {
            return !host_raw().empty();
        }

        /**
         * @brief set the hostname/ip in the uri if possible
         */
        auto& host(string_view_type const& new_host) noexcept {
            parse_host();

            // todo: are you sure it can handle punycode as well?
            string_type encoded_host{this->get_allocator()};
            encode_uri_component(new_host, encoded_host, details::REG_NAME_NOT_PCT_ENCODED<char_type>);
            if ((!ascii::starts_with(new_host, '[') || !ascii::ends_with(new_host, ']')) &&
                webpp::is::ipv6(new_host)) {
                encoded_host = '[' + encoded_host + ']';
            }

            if (authority_start == data.size()) {
                // there's no authority start

                if (encoded_host.empty())
                    return *this; // there's nothing to do here. It's already
                // what the user wants

                if (scheme_end == data.size()) {
                    // there's no scheme either, so we just have to add to the
                    // beginning of the string
                    replace_value(0, 0, string_type("//") + encoded_host);
                    return *this;
                } else {
                    // there's a scheme
                    replace_value(scheme_end, 0, string_type("//") + encoded_host);
                    return *this;
                }
            }

            stl::size_t start, finish;

            // you know I could do this in one line of code, but I did this
            // because I don't want you to curse me :)

            // we have authority_start, let's check user_info and port too
            if (user_info_end == data.size()) {
                // there's no user info
                if (scheme_end == data.size()) {
                    start = 0;
                    if (!new_host.empty() && !ascii::starts_with(string_view_type{encoded_host}, "//")) {
                        encoded_host = "//" + encoded_host;
                    }
                } else {
                    start = authority_start;
                }
            } else {
                // there's a user info
                start = user_info_end;
            }

            if (port_start != data.size()) {
                // but there's a port
                finish = port_start;
            } else {
                // there's no port either
                if (authority_end != data.size()) {
                    // there's a path
                    finish = authority_end;
                } else {
                    // there's no path either
                    finish = data.size() - 1; // till the end
                }
            }

            replace_value(start, finish - start, encoded_host);

            return *this;
        }

        /**
         * @brief get the hostname/ipv4/ipv6 from the URI. if the URI doesn't
         * include a hostname/ip or its hostname/ip is not in a valid shape, it
         * will return an empty string
         * @return string/ipv4/ipv6
         */
        [[nodiscard]] stl::variant<ipv4<traits_type>, ipv6<traits_type>, string_type>
        host_structured_decoded() const noexcept {
            if (auto _host_structured = host_structured();
              stl::holds_alternative<string_view_type>(_host_structured)) {
                // convert string_view to string and then decode it
                string_type output{this->get_allocator()};
                if (decode_uri_component(stl::get<string_view_type>(_host_structured), output,
                                         details::REG_NAME_NOT_PCT_ENCODED<char_type>)) {
                    return output;
                }
                return {}; // as in an empty one
            } else {
                return _host_structured;
            }
        }

        /**
         * @brief clear host part from URI
         */
        auto& clear_host() noexcept {
            return host("");
        }

        /**
         * Check if the specified host is an IP address or not
         * This method check if the specified host is a IPv4 Address or has a
         * valid syntax for either IPv6 or Future version of IP addresses which
         * also starts with "[" and ends with "]" with an unknown syntax (as of
         * writing this code obv)
         * @return an indication of weather or not the specified Hostname is a
         * valid IP address or not
         */
        [[nodiscard]] bool is_ip() const noexcept {
            auto _host = host_raw();
            return webpp::is::ipv4(_host) || (ascii::starts_with(_host, '[') && ascii::ends_with(_host, ']'));
        }

        /**
         * Get the host and split it by dot separator. TLD (Top Level Domain)
         * will be the last one and Second Level Domain will be the one before
         * that and the rest will be subdomains.
         */
        [[nodiscard]] istl::vector<traits_type, string_type> domains() const noexcept {
            auto _host = host_raw();
            if (_host.empty() || is_ip())
                return {};
            istl::vector<traits_type, string_type> subs{this->template get_allocator_as<string_type>()};
            for (;;) {
                auto dot = _host.find('.');
                auto sub = _host.substr(0, dot);
                if (sub.empty())
                    break;
                subs.emplace_back(sub, this->template get_allocator_as<char_type>());
                if (dot == string_view_type::npos)
                    break;
                _host.remove_prefix(dot + 1);
            }
            return subs;
        }

        /**
         * Get the TLD (top level domain) or sometimes called extension
         */
        [[nodiscard]] string_view_type top_level_domain() const noexcept {
            auto _host = host_raw();
            if (_host.empty() || is_ip())
                return {};
            auto dot = _host.find_last_of('.');
            return _host.substr(dot != string_view_type::npos ? dot + 1 : 0);
        }

        /**
         * Set the TLD (Top Level Domain) in the uri
         * @param tld
         * @return
         */
        auto& top_level_domain(string_view_type tld) noexcept {
            auto _host = host_raw();
            if (_host.empty()) {
                // I've already written that code. Yay, I'm so happy
                host(tld);
            } else if (!is_ip() && !webpp::is::ip(tld)) {
                // cannot put an ip address as a tld, user should use set host
                // instead of this method.
                auto dot   = _host.find_last_of('.');
                auto start = dot != string_view_type::npos ? dot + 1 : 0;
                host(string_type(_host.substr(0, start), this->get_allocator()) + string_type(tld, this->get_allocator()));
            }
            return *this;
        }

        /**
         * Check if the specified uri has a top level domain (TLD) or not
         * @return an indication of weather or not the URI has TLD or not
         */
        [[nodiscard]] bool has_top_level_domain() const noexcept {
            return !top_level_domain().empty();
        }

        /**
         * Get the second level domain out of the host
         */
        [[nodiscard]] string_view_type second_level_domain() const noexcept {
            auto _host = host_raw();
            if (_host.empty() || is_ip())
                return {};
            auto last_dot = _host.find_last_of('.');
            if (last_dot == string_view_type::npos)
                return {};
            auto bef_last_dot = _host.find_last_of('.', last_dot - 1);
            auto start        = bef_last_dot == string_view_type::npos ? 0 : bef_last_dot + 1;
            auto sld          = _host.substr(start, last_dot - start);
            return sld;
        }

        /**
         * Set the second level domain to the specified string.
         * Attention: this method will only work if Top Level Domain already
         * exists
         * @param sld
         */
        auto& second_level_domain(string_view_type sld) noexcept {
            auto _host = host_raw();
            if (_host.empty() || is_ip())
                return *this;

            auto last_dot = _host.find_last_of('.');
            if (last_dot == string_view_type::npos) {
                // we have to insert it at the beginning of the host string

                // there's nothing to do it's empty
                if (!sld.empty()) {
                    static_cast<void>(host(string_type(sld, this->get_allocator()) + '.' + string_type(_host, this->get_allocator())));
                }
            } else {
                auto bef_last_dot = _host.find_last_of('.', last_dot - 1);
                auto start        = bef_last_dot == string_view_type::npos ? 0 : bef_last_dot + 1;
                if (!sld.empty())
                    static_cast<void>(
                      host(string_type(_host.substr(0, start), this->get_allocator()) + string_type(sld, this->get_allocator()) + string_type(_host.substr(last_dot), this->get_allocator())));
                else
                    static_cast<void>(host(string_type(_host.substr(last_dot + 1), this->get_allocator())));
            }
            return *this;
        }

        /**
         * This method will remove the Second Level Domain and also any
         * Sub-Domains if there are any.
         */
        auto& clear_second_level_domain() noexcept {
            return second_level_domain({});
        }

        /**
         * Check if the specified uri has a second level domain or not.
         * @return An indication of weather or not the URI has SLD or not
         */
        [[nodiscard]] bool has_second_level_domain() const noexcept {
            return !second_level_domain().empty();
        }

        /**
         * Get the sub-domain (with sub-sub-...-sub-domain)
         * @return
         */
        [[nodiscard]] string_view_type subdomains() const noexcept {
            auto _host = host_raw();
            if (_host.empty() || is_ip())
                return {};
            auto last_dot = _host.find_last_of('.');
            if (last_dot == string_view_type::npos)
                return {};
            auto bef_last_dot = _host.find_last_of('.', last_dot - 1);
            if (bef_last_dot == string_view_type::npos)
                return {};
            return _host.substr(0, bef_last_dot);
        }

        /**
         * Set the sub-domain part of the host name
         * Attention: this method will only work if Top Level Domain and Second
         * Level Domain already exists
         * @param sds
         */
        auto& subdomains(string_view_type sds) noexcept {
            auto _host = host_raw();
            if (_host.empty() || is_ip())
                return *this;
            auto last_dot = _host.find_last_of('.');
            if (last_dot == string_view_type::npos)
                return *this;
            auto bef_last_dot = _host.find_last_of('.', last_dot - 1);
            if (bef_last_dot == string_view_type::npos)
                return *this;
            if (sds.empty()) // special check for when we want to remove the SDS
                bef_last_dot++;
            static_cast<void>(host(string_type(sds) + string_type(_host.substr(bef_last_dot))));
            return *this;
        }

        /**
         * Remove the sub-domains if exists. This method should not have
         * side-effects if there's no sub-domain
         */
        auto& clear_subdomains() noexcept {
            return subdomains({});
        }

        /**
         * Check if the specified uri has at least one sub-domain or not
         * @return an indication of weather or not the URI has subdomains or not
         * TODO: we could be smarter here
         */
        [[nodiscard]] bool has_subdomains() const noexcept {
            return !subdomains().empty();
        }



        ///////////////////////////////////////////// Port /////////////////////////////////////////////

        /**
         * Get the default port for the specified scheme
         * TODO: make this function a free function
         */
        [[nodiscard]] uint16_t default_port() const noexcept {
            auto _scheme = scheme();
            if (_scheme == "http")
                return 80u;
            if (_scheme == "https")
                return 443u;
            if (_scheme == "ftp")
                return 21u;
            if (_scheme == "ssh")
                return 22u;
            if (_scheme == "telnet")
                return 23u;
            if (_scheme == "ftps")
                return 990u;

            // TODO: add more protocols here
            return 0u;
        }

        /**
         * @brief port number of the uri;
         * @return port number
         * @default 80
         */
        [[nodiscard]] uint16_t port_uint16() const noexcept {
            if (has_port()) {
                return static_cast<uint16_t>(to_uint(port()));
            }
            return default_port();
        }

        /**
         * Get the port in a string_view format; there's no default value.
         * @return string_view
         */
        [[nodiscard]] string_view_type port() const noexcept {
            parse_port();

            // there's no port
            if (port_start == data.size())
                return {};

            // don't worry authority_end will be the end of the string anyway
            return substr(port_start + 1, (authority_end == data.size() ? authority_end - 1 : authority_end) -
                                          (port_start + 1));
        }

        /**
         * @brief checks if the uri has explicitly specified a port number or
         * not
         * @return bool
         */
        [[nodiscard]] bool has_port() const noexcept {
            parse_port();
            return port_start != data.size();
        }

        /**
         * @brief set port
         * @param new_port
         */
        auto& port(uint16_t new_port) noexcept {
            return port(to_str_copy<traits_type>(new_port));
        }

        /**
         * Set new port value
         * @param new_port
         */
        uri_string& port(string_view_type new_port) noexcept {
            if (ascii::starts_with(new_port, ':'))
                new_port.remove_prefix(1);
            if (!ascii::is::digit(new_port))
                throw stl::invalid_argument("The specified port is not valid");
            parse_port();
            if (port_start == data.size()) {
                // there's no port, I have to insert it myself:
                parse_host();

                if (authority_end != data.size()) {
                    // found it at the end of the line
                    replace_value(authority_end, 0, ":" + string_type(new_port));
                } else if (user_info_end != data.size()) {
                    // there's authority and there might be a host
                    replace_value(user_info_end + 1, user_info_end + 1, ":" + string_type(new_port));
                } else if (authority_start != data.size()) {
                    // there's a authority_start at least
                    replace_value(authority_start + 1, 0, ":" + string_type(new_port));
                } else {
                    // there's no authority at all.
                    if (scheme_end == data.size()) {
                        // there's no scheme either
                        replace_value(0, 0, "//:" + string_type(new_port));
                    } else {
                        // there's scheme
                        replace_value(scheme_end + 1, 0, "//:" + string_type(new_port));
                    }
                }
            } else {
                // there's a port and we are going to just replace it
                replace_value(port_start + 1,
                              authority_end == data.size() ? authority_end - 1 : authority_end, new_port);
            }
            return *this;
        }

        /**
         * @brief clear the port number from the uri and defaults to 80
         * @return self
         */
        uri_string& clear_port() noexcept {
            return port({});
        }




        ///////////////////////////////////////////// Path /////////////////////////////////////////////




        /**
         * @brief check if the URI has a path or not
         * @return bool true if the URI doesn't have a path
         */
        [[nodiscard]] bool has_path() const noexcept {
            parse_path();
            return authority_end != data.size();
        }

        /**
         * @brief get path in non-host, string format
         */
        [[nodiscard]] string_view_type path_raw() const noexcept {
            if (!has_path())
                return {};
            return substr(authority_end,
                                stl::min(query_start, fragment_start) - authority_end);
        }

        /**
         * @brief host path as a string
         * @return std::optional<string> the string might not have the right format
         */
        [[nodiscard]] stl::optional<string_type> path() const noexcept {
            string_type res{this->get_allocator()};
            if (!decode_uri_component(path_raw(), res, details::LEGAL_PATH_CHARS<char_type>)) {
                return stl::nullopt;
            }
            return res;
        }

        /**
         * Get the host slugs with the specified container type
         * We know how to get the allocator, don't worry.
         * The specified container's string type should be a string and not a string_view
         */
        template <typename Container = basic_path<string_type, allocator_type>>
        [[nodiscard]] Container slugs() const noexcept {
            using value_type = typename Container::value_type;
            Container container(this->template get_allocator_as<value_type>());
            if (!extract_slugs_to<Container>(container)) {
                return container; // yes, an empty container
            }
            return container;
        }

        /**
         * Get the non-host slugs.
         * You can use string_view as the underlying string type of the container since we don't
         * decode the string. As long as the class has access to the string_view, this method has too.
         */
        template <typename Container = basic_path<string_type, allocator_type>>
        [[nodiscard]] Container raw_slugs() const noexcept {
            Container container(this->get_allocator());
            extract_raw_slugs_to<Container>(container);
            return container;
        }

        /**
         * @brief get the path as the specified type
         * @details this method will returns a vector/list/... of string/string_views
         * this method does not just response to the fact that Container should
         * be an std container, but if string/string_view is presented as a
         * container, it will return the whole path.
         */
        template <typename Container = basic_path<string_view_type, allocator_type>>
        auto& extract_raw_slugs_to(Container& container) const noexcept {
            auto _path = path_raw();
            if (_path.empty())
                return *this;
            stl::size_t slash_start      = 0;
            stl::size_t last_slash_start = 0;
            auto        _path_size       = _path.size();
            if (_path.front() == '/')
                container.emplace_back(); // empty string
            do {
                slash_start = _path.find('/', last_slash_start + 1);
                container.emplace_back(_path.data() + last_slash_start + 1,
                                       stl::min(slash_start, _path_size) - last_slash_start -
                                       1);
                // if (slash_start != string_view_type::npos)
                // _path.remove_prefix(slash_start + 1);
                // else
                // _path.remove_prefix(_path.size());
                last_slash_start = slash_start;
            } while (slash_start != string_view_type::npos);
            return *this;
        }

        /**
         * @brief this one will return a container containing host strings of the path.
         * @attention do not use string_view or any alternatives for this method
         * as this method should own its data.
         */
        template <typename Container = basic_path<string_type, allocator_type>>
        [[nodiscard]] bool extract_slugs_to(Container& container) const noexcept {
            using vec_str_t = typename Container::value_type;
            static_assert(istl::String<vec_str_t>,
                          "The specified container doesn't hold a value type that we can understand.");
            for (auto it = container.begin(); it != container.end(); ++it) {
                vec_str_t tmp(container.get_allocator());
                if (!decode_uri_component(*it, tmp, details::PCHAR_NOT_PCT_ENCODED<char_type>)) {
                    return false;
                }
                it->swap(tmp);
            }
            return true;
        }

        /**
         * @brief set path
         */
        template <typename Container>
        auto& set_path_from(const Container& m_path) noexcept {
            return path(m_path.begin(), m_path.end());
        }

        /**
         * Set path by begin and end of an iterator
         */
        template <typename Iter>
        auto& set_path_from(const Iter& _start, const Iter& _end) noexcept {
            const auto almost_end = stl::prev(_end);
            string_type      new_path{this->get_allocator()};
            for (auto it = _start; it != almost_end; ++it) {
                encode_uri_component(*it, new_path, details::PCHAR_NOT_PCT_ENCODED<char_type>);
                new_path.append('/');
            }
            // append the last slug
            encode_uri_component(*almost_end, new_path, details::PCHAR_NOT_PCT_ENCODED<char_type>);
            return set(stl::move(new_path));
        }

        /**
         * @brief set the path for the uri
         */
        auto& path(istl::StringViewifiable auto&& m_path) noexcept {
            parse_path();
            string_type str(this->get_allocator());
            encode_uri_component(m_path, str, charset(details::PCHAR_NOT_PCT_ENCODED<char_type>, charset<char_type, 1>('/')));
            auto _encoded_path = (ascii::starts_with(m_path, '/') ? "" : "/") + str;
            replace_value(authority_end, query_start - authority_end, _encoded_path);
            return *this;
        }

        /**
         * @brief clear path from the URI
         */
        auto& clear_path() noexcept {
            return path("");
        }

        /**
         * @brief checks if the path is an absolute path or relative path
         * @return bool
         */
        [[nodiscard]] bool is_absolute() const noexcept {
            return ascii::starts_with(path_raw(), '/');
        }

        /**
         * @brief checks if the path is a relative path or an absolute one
         * @return bool
         */
        [[nodiscard]] bool is_relative() const noexcept {
            return !is_absolute();
        }

        /**
         * @brief checks if the uri path is normalized or not (contains relative
         * . or .. paths)
         * @return bool
         */
        [[nodiscard]] bool is_normalized() const noexcept {
            auto m_path = raw_slugs();
            return m_path.cend() != stl::find_if(m_path.cbegin(), m_path.cend(), [](auto const& p) {
              return p == "." || p == "..";
            });
        }

        /**
         * @details This method applies the "remove_dot_segments" routine talked
         * about in RFC 3986 (https://tools.ietf.org/html/rfc3986) to the path
         * segments of the URI, in order to normalize the path
         * (apply and remove "." and ".." segments).
         */
        auto& normalize() noexcept {
            // TODO
            return *this;
        }



        ///////////////////////////////////////////// Queries /////////////////////////////////////////////


        /**
         * @brief checks if the uri has query or not
         */
        [[nodiscard]] bool has_queries() const noexcept {
            parse_query();
            return query_start != data.size();
        }

        [[nodiscard]] string_view_type queries_raw() const noexcept {
            parse_query();
            if (query_start == data.size())
                return {};
            return substr(query_start + 1, fragment_start - query_start - 1);
        }

        /**
         * Get the query in a decoded string format
         */
        [[nodiscard]] stl::optional<string_type> queries_string() const noexcept {
            string_type d_queries{this->get_allocator()};
            if (!decode_uri_component(queries_raw(), d_queries, details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>)) {
                return stl::nullopt;
            }
            return d_queries;
        }

        template <typename MapType = basic_queries<string_type, allocator_type>>
        [[nodiscard]] auto queries() const noexcept {
            MapType res(this->get_allocator());
            extract_queries_to(res);
            return res;
        }

        /**
         * Set queries
         */
        auto& queries(istl::StringViewifiable auto&& _queries) {
            auto m_query = istl::string_viewify(stl::forward<decltype(_queries)>(_queries));
            if (!webpp::is::query(m_query))
                throw stl::invalid_argument("The specified string is not a valid query");

            string_type encoded_query(this->get_allocator());
            if (ascii::starts_with(m_query, '?')) {
                encoded_query.append('?');
            }
            encode_uri_component(m_query, encoded_query, details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>);

            parse_query();

            if (query_start != data.size()) {
                // we don't have a query
                if (fragment_start != data.size()) {
                    replace_value(fragment_start, 0, encoded_query);
                } else {
                    parse_path();
                    if (authority_end == data.size()) {
                        // we don't even have authority_end
                        parse_scheme();
                        if (authority_start == data.size()) {
                            // there's no authority_start
                            if (scheme_end == data.size()) {
                                // it's an empty string
                                replace_value(0, 0, "///" + encoded_query);
                            } else {
                                replace_value(scheme_end, 0, "/" + encoded_query);
                            }
                        } else {
                            replace_value(authority_start, 0, "/" + encoded_query);
                        }
                    } else {
                        // we have authority_end
                        if (data[authority_end] == '/') {
                            replace_value(authority_end + 1, 0, encoded_query);
                        } else {
                            replace_value(authority_end + 1, 0, "/" + encoded_query);
                        }
                    }
                }
            } else {
                // we have query
                replace_value(query_start, fragment_start - query_start,
                                    encoded_query);
            }
            return *this;
        }

        /**
         * Append queries from a container like std::map or std::multimap
         */
        template <typename MapIter>
        auto& set_queries_from(MapIter const& _queries_begin, MapIter const& _queries_end) {
            static_assert(is_mutable(), "You can't use this method on a non-modifiable uri struct.");
            using map_key_type   = typename MapIter::first_type;
            using map_value_type = typename MapIter::second_type;
            static_assert(istl::StringViewifiable<map_key_type> &&
                          istl::StringViewifiable<map_value_type>,
                          "The specified map is not valid");

            stl::size_t reserved_size = 0;
            for (auto it = _queries_begin; it != _queries_end; ++it)
                reserved_size += it->first->size() + it->second->size() + 2;
            string_type _query_data(this->get_allocator());
            _query_data.reserve(reserved_size);

            for (auto it = _queries_begin; it != _queries_end; ++it) {
                string_type name(this->get_allocator());
                string_type value(this->get_allocator());
                encode_uri_component(it->first, name, details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>);
                encode_uri_component(it->second, value, details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>);
                if (name.empty()) // when name is empty, we just don't care
                    continue;
                _query_data.append(name);
                if (!value.empty()) {
                    _query_data.append('=');
                    _query_data.append(value);
                }
                if (stl::next(it) != _queries_end) {
                    _query_data.append('&');
                }
            }
            set(stl::move(_query_data));
            return *this;
        }

        template <typename MapType>
        auto& set_queries_from(MapType const& _queries) {
            return set_from(_queries.begin(), _queries.end());
        }

        /**
         * @brief clear the query section of the URI
         */
        auto& clear_queries() noexcept {
            return queries("");
        }

        /**
         * Get the query in as a map<string, string>
         * It's also in a decoded format
         */
        template <typename MapType = basic_queries<string_type, allocator_type>>
        auto& extract_queries_to(MapType& q_structured) const noexcept {
            using map_key_type   = typename MapType::key_type;
            using map_value_type = typename MapType::mapped_type;
            static_assert(istl::String<map_key_type>,
                          "The specified container can't hold the query keys.");
            static_assert(istl::String<map_value_type>,
                          "The specified container can't hold the query values.");
            stl::size_t last_and_sep = 0;
            auto        _query       = queries_raw();
            do {
                auto and_sep = _query.find('&', last_and_sep); // find the delimiter
                auto eq_sep  = _query.find("=", last_and_sep, and_sep - last_and_sep);
                auto name    = _query.substr(last_and_sep + 1, stl::min(eq_sep, and_sep));
                last_and_sep = and_sep;
                if (name.empty()) // a name should not be empty
                    continue;
                string_type d_value(this->get_allocator());
                string_type d_name(this->get_allocator());
                if (and_sep != string_view_type::npos) { // we have a value as well
                    d_value = _query.substr(eq_sep + 1, and_sep);
                }
                if (!decode_uri_component(name, d_name, details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>)) {
                    d_name = name; // just put the non-decoded string there
                }
                if (!d_name.empty()) {
                    map_value_type new_value(q_structured.get_allocator());
                    if (decode_uri_component(d_value, new_value, details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>)) {
                        q_structured[d_name] = stl::move(new_value);
                    } else {
                        q_structured[d_name] = stl::move(d_value); // just put the non-decoded value here
                    }
                }
            } while (last_and_sep != string_view_type::npos);
            return *this;
        }

        auto& append_query(istl::StringViewifiable auto&& key,
                           istl::StringViewifiable auto&& value) noexcept {
            // todo
        }



        ///////////////////////////////////////////// Fragment /////////////////////////////////////////////



        /**
         * @brief get fragment
         */
        [[nodiscard]] string_view_type fragment() const noexcept {
            parse_fragment();
            return substr(fragment_start + 1);
        }

        /**
         * @brief replace the old fragment
         * todo: write tests for this
         */
        auto& fragment(istl::StringViewifiable auto&& _str) const noexcept {
            auto str = istl::string_viewify(stl::forward<decltype(_str)>(_str));
            if (str.empty()) {
                // remove fragment
                replace_value(fragment_start, data.size() - fragment_start, "");
                return *this;
            }
            string_type encoded_fragment{this->get_allocator()};
            if (!ascii::starts_with(str, '#'))
                encoded_fragment += '#';
            encode_uri_component(str, encoded_fragment, details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>);
            replace_value(fragment_start, data.size() - fragment_start, stl::move(encoded_fragment));
            return *this;
        }

        /**
         * @brief an indication of whether the URI has fragment or not.
         * @return
         */
        [[nodiscard]] bool has_fragment() const noexcept {
            parse_fragment();
            return fragment_start != data.size();
        }

        /**
         * @brief clear the fragment part of the uri
         * @return
         */
        uri_string& clear_fragment() noexcept {
            return fragment("");
        }



        /**
         * This method returns an indication of whether or not the URI includes
         * any element that is part of the authority URI.
         * @return bool
         */
        [[nodiscard]] bool has_authority() const noexcept {
            return has_host() || has_user_info() || has_port();
        }

        /**
         * Get the string representation of the uri
         * @return string
         */
        [[nodiscard]] storred_str_t const& str() const noexcept {
            return data;
        }

        /**
         * Get a string_view version of the uri
         * @return string_view_type
         */
        [[nodiscard]] string_view_type string_view() const noexcept {
            if constexpr (stl::is_same_v<storred_str_t, string_view_type>) {
                return data;
            } else {
                return string_view_type(data);
            }
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
        [[nodiscard]] uri_string resolve(const uri_string& relative_uri) const noexcept {
            // Resolve the reference by following the algorithm
            // from section 5.2.2 in
            // RFC 3986 (https://tools.ietf.org/html/rfc3986).
            uri_string target;
            if (relative_uri.has_scheme()) {
                target = relative_uri;
                target.normalize_path();
            } else {
                target.scheme(scheme());
                target.fragment(relative_uri.fragment());
                if (relative_uri.has_host()) {
                    target.host(relative_uri.host());
                    target.port(relative_uri.port());
                    target.user_info(relative_uri.user_info());
                    target.path(relative_uri.path());
                    target.set_queries(relative_uri.queries_raw());
                    target.normalize_path();
                } else {
                    target.host(host());
                    target.user_info(user_info());
                    target.port(port());
                    if (!relative_uri.has_path()) {
                        target.path(path_raw());
                        if (relative_uri.has_queries()) {
                            target.set_queries(relative_uri.queries_raw());
                        } else {
                            target.set_queries(queries_raw());
                        }
                    } else {
                        target.set_queries(relative_uri._queries);
                        // RFC describes this as:
                        // "if (R.path starts-with "/") then"
                        if (relative_uri.is_absolute()) {
                            target.path(relative_uri._path);
                            target.normalize_path();
                        } else {
                            // RFC describes this as:
                            // "T.path = merge(Base.path, R.path);"
                            target.path(path());
                            auto target_path       = target.path_structured();
                            auto relative_uri_path = relative_uri.path_structured();
                            if (target_path.size() > 1) {
                                target_path.pop_back();
                            }
                            stl::copy(relative_uri_path.cbegin(), relative_uri_path.cend(),
                                      stl::back_inserter(target_path));
                            target.path(target_path);
                            target.normalize_path();
                        }
                    }
                }
            }

            return target;
        }

        /**
         * Check if the specified URI is in fact a URN
         *
         * URN syntax based on https://tools.ietf.org/html/rfc8141#section-2
         *
         *      namestring    = assigned-name
         *                      [ rq-components ]
         *                      [ "#" f-component ]
         *      assigned-name = "urn" ":" NID ":" NSS
         *      NID           = (alphanum) 0*30(ldh) (alphanum)
         *      ldh           = alphanum / "-"
         *      NSS           = pchar *(pchar / "/")
         *      rq-components = [ "?+" r-component ]
         *                      [ "?=" q-component ]
         *      r-component   = pchar *( pchar / "/" / "?" )
         *      q-component   = pchar *( pchar / "/" / "?" )
         *      f-component   = fragment
         *
         */
        [[nodiscard]] bool is_urn() const noexcept {
            return scheme() == "urn" && authority_start == data.size();
        }

        /**
         * Check if the specified URI is in fact a URL
         */
        [[nodiscard]] bool is_url() const noexcept {
            return has_host();
        }

        /**
         * Check if the specified string is a valid URI or not
         */
        [[nodiscard]] bool is_valid() const noexcept {
            return has_scheme() || has_authority() || has_path() || has_fragment();
        }

    };

    template <istl::CharType CharT = char>
    uri_string(CharT const*) -> uri_string<basic_std_traits<CharT>, false>;
    // basic_uri(const char[])->basic_uri<std::string_view>;

    template <typename CharT = char>
    uri_string(stl::basic_string_view<CharT>) -> uri_string<basic_std_traits<CharT>, false>;

    template <typename CharT = char>
    uri_string(stl::basic_string<CharT>) -> uri_string<basic_std_traits<CharT>, true>;

    using uri_view  = uri_string<std_traits, false>;


    template <Traits TraitsType, bool Mutable1, bool Mutable2>
    bool operator==(uri_string<TraitsType, Mutable1> const& one,
                    uri_string<TraitsType, Mutable2> const& two) noexcept {
        return one.operator==(two.str());
    }


    //    template <typename CharT>
    //    bool operator==(uri<CharT> const&       one,
    //                    const_uri<CharT> const& two) noexcept {
    //        return one.operator==(two.str());
    //    }
    //
    //    template <typename CharT>
    //    bool operator==(const_uri<CharT> const& one,
    //                    uri<CharT> const&       two) noexcept {
    //        return one.operator==(two.str());
    //    }
    //
    template <Traits TraitsType, bool Mutable1, bool Mutable2>
    [[nodiscard]] bool equal_path(uri_string<TraitsType, Mutable1> const& p1,
                                  uri_string<TraitsType, Mutable2> const& p2) noexcept {
        auto _p1 = p1.slugs();
        auto _p2 = p2.slugs();
        auto it2 = _p2.cbegin();
        auto it1 = _p1.cbegin();
        while (it1 != _p1.cend() && it2 != _p2.cend()) {
            if (*it1 != *it2) {
                if (stl::empty(*it1)) {
                    ++it1;
                    continue;
                } else if (stl::empty(*it2)) {
                    ++it2;
                    continue;
                }

                return false;
            }
            ++it1;
            ++it2;
        }

        if (it1 != _p1.cend()) {
            if (!stl::all_of(
                  it1, _p1.cend(), [](auto const& a) constexpr noexcept { return stl::empty(a); }))
                return false;
        }
        if (it1 != _p2.cend()) {
            if (!stl::all_of(
                  it2, _p2.cend(), [](auto const& a) constexpr noexcept { return stl::empty(a); }))
                return false;
        }
        return true;
    }

    [[nodiscard]] inline auto equal_path(stl::string_view const& p1, stl::string_view const& p2) noexcept {
        return p1 == p2 || equal_path<std_traits, false, false>(uri_view{p1}, uri_view{p2});
    }

    template <Traits TraitsType, bool Mutable>
    [[nodiscard]] inline auto equal_path(uri_string<TraitsType, Mutable> const&        p1,
                                         typename TraitsType::string_view_type const& p2) noexcept {
        return p1 == p2 || equal_path<TraitsType, false>(p1, uri_view{p2});
    }

    template <Traits TraitsType, bool Mutable>
    [[nodiscard]] inline auto equal_path(typename TraitsType::string_view_type const& p1,
                                         uri_string<TraitsType, Mutable> const&        p2) noexcept {
        return p2 == p1 || equal_path<TraitsType, false>(uri_view{p1}, p2);
    }

} // namespace webpp

#endif // WEBPP_UTILS_URI_H
