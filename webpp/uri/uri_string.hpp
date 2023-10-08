#ifndef WEBPP_URI_URI_STRING_HPP
#define WEBPP_URI_URI_STRING_HPP

#include "../convert/casts.hpp"
#include "../ip/ipv4.hpp"
#include "../ip/ipv6.hpp"
#include "../memory/allocators.hpp"
#include "../std/map.hpp"
#include "../std/optional.hpp"
#include "../std/vector.hpp"
#include "../strings/charset.hpp"
#include "../strings/iequals.hpp"
#include "../strings/join.hpp"
#include "../strings/to_case.hpp"
#include "../traits/traits.hpp"
#include "../validators/validators.hpp"
#include "details/constants.hpp"
#include "encoding.hpp"
#include "path.hpp"
#include "queries.hpp"

#include <variant>

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

    struct uri_error_type {
        bool scheme : 1    = false;
        bool user_info : 1 = false;
        bool host : 1      = false;
        bool port : 1      = false;
        bool path : 1      = false;
        bool queries : 1   = false;
        bool fragment : 1  = false;

        constexpr void reset() noexcept {
            scheme    = false;
            user_info = false;
            host      = false;
            port      = false;
            path      = false;
            queries   = false;
            fragment  = false;
        }

        [[nodiscard]] constexpr bool is_success() const noexcept {
            // todo: see if the compiler optimizes this or not
            return !(scheme & user_info & host & port & path & queries & fragment);
        }
    };


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
    template <istl::String StrT = stl::string, istl::StringView StrViewT = istl::string_view_type_of<StrT>>
    struct uri_string : public allocator_holder<typename stl::remove_cvref_t<StrT>::allocator_type> {
        using specified_string_type      = StrT;
        using specified_string_view_type = StrViewT;
        using string_type                = stl::remove_cvref_t<StrT>;
        using string_view_type           = stl::remove_cvref_t<StrViewT>;
        using char_type                  = istl::char_type_of_t<StrT>;
        using allocator_type             = typename string_type::allocator_type;
        using alloc_holder_type          = allocator_holder<allocator_type>;

        static constexpr bool Mutable = !stl::is_const_v<StrT>;

        static_assert(stl::same_as<char_type, istl::char_type_of_t<string_view_type>>,
                      "The specified string types do not have the same character type.");

        /**
         * Getting the appropriate string type to use.
         * If the specified string type cannot be changed, the string_view will be used,
         * otherwise, string itself.
         */
        using stored_str_t = stl::conditional_t<Mutable, string_type, string_view_type>;


      private:
        static constexpr auto max_port_number = 65535;

        /**
         * This is the whole url (if we need to own the uri ourselves)
         */
        stored_str_t data{};

        mutable stl::size_t    scheme_end      = string_view_type::npos;
        mutable stl::size_t    authority_start = string_view_type::npos;
        mutable stl::size_t    user_info_end   = string_view_type::npos;
        mutable stl::size_t    port_start      = string_view_type::npos;
        mutable stl::size_t    authority_end   = string_view_type::npos;
        mutable stl::size_t    query_start     = string_view_type::npos;
        mutable stl::size_t    fragment_start  = string_view_type::npos;
        mutable uri_error_type errors;

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
        constexpr void parse_scheme() const noexcept {
            if (scheme_end != string_view_type::npos)
                return; // It's already parsed

            auto const _data = this->string_view();

            // extracting scheme
            if (ascii::starts_with(_data, "//")) {
                authority_start = 2;
                scheme_end      = data.size(); // so we don't have to check again
                return;
            } else if (const auto colon = _data.find(':'); colon != string_view_type::npos) {
                auto m_scheme = _data.substr(0, colon);
                if (ALPHA<char_type>.contains(m_scheme[0]) &&
                    m_scheme.substr(1).find_first_not_of(
                      details::SCHEME_NOT_FIRST<char_type>.string_view())) {
                    scheme_end = colon;

                    if (_data.substr(colon + 1, 2) == "//") {
                        authority_start = colon + 3;
                    } else {
                        // it should be a URN or an invalid URI at this point
                        authority_start = data.size();
                        if (!ascii::iequals<ascii::char_case_side::second_lowered>(
                              _data.substr(0, scheme_end),
                              "urn")) {
                            errors.scheme = true;
                        }
                    }
                    return;
                } else {
                    errors.scheme = true;
                }
            }

            scheme_end = authority_start = data.size();
        }

        /**
         * parse user info
         */
        constexpr void parse_user_info() const noexcept {
            if (user_info_end != string_view_type::npos)
                return; // It's already parsed

            parse_scheme(); // to get "authority_start"

            if (authority_start == data.size()) {
                user_info_end = data.size();
                return; // there's no user_info_end without authority_start
            }

            auto const _data = this->string_view();
            auto const next_section =
              stl::min(stl::min(stl::min(authority_end, query_start), fragment_start), _data.size());
            auto const user_info_section = _data.substr(authority_start, next_section - authority_start);

            // finding the username
            user_info_end = user_info_section.find_first_of(":[@");
            switch (user_info_end) {
                case 0:
                    if (user_info_section.front() != '[')
                        break; // todo: what about : and @ ??
                    // It's an IPv6
                    [[fallthrough]];
                case string_view_type::npos: user_info_end = data.size(); break;
                default: {
                    switch (user_info_section[user_info_end]) {
                        case '@': {
                            // no password; we only have username
                            // nothing to do
                            user_info_end += authority_start;
                            break;
                        }
                        case ':': {
                            // finding the password
                            user_info_end = user_info_section.find_first_of('@', user_info_end);
                            if (user_info_end == string_view_type::npos) {
                                // it was a port
                                user_info_end = data.size();
                            } else {
                                user_info_end += authority_start;
                            }
                            // todo: evaluate the user info here
                            break;
                        }
                    }
                }
            }
        }

        /**
         * parse path
         * this will make sure that the "authority_end" variable is filled
         */
        constexpr void parse_path() const noexcept {
            if (authority_end != string_view_type::npos)
                return; // It's already parsed

            parse_scheme(); // to get "authority_start"
            parse_query();  // to get "query_start"

            auto const _data = this->string_view();

            const bool is_urn =
              scheme_end != data.size() &&
              ascii::iequals<ascii::char_case_side::second_lowered>(_data.substr(0, scheme_end), "urn");
            stl::size_t starting_point; // NOLINT(cppcoreguidelines-init-variables)
            if (authority_start != data.size()) {
                starting_point        = authority_start;
                auto const host_start = _data.find('@', starting_point);
                if (host_start != string_view_type::npos) {
                    starting_point = host_start;
                }
            } else if (scheme_end != data.size()) {
                starting_point = scheme_end;
            } else {
                authority_end = 0; // probably the whole string is a path
                return;
            }
            authority_end =
              _data.substr(starting_point, query_start - starting_point).find_first_of(is_urn ? ':' : '/');
            if (authority_end == string_view_type::npos) {
                authority_end = data.size();
            } else {
                starting_point += is_urn; // we don't want the first ":" to be in the path of a urn
                authority_end += starting_point;
            }
        }

        /**
         * parse port
         * this makes sure that the "port_start" variable is filled
         */
        constexpr void parse_port() const noexcept {
            if (port_start != string_view_type::npos)
                return; // It's already parsed

            parse_user_info(); // to get "authority_start" and "user_info_end"

            if (authority_start == data.size()) {
                port_start = data.size();
                return; // there's no user_info_end without authority_start
            }

            parse_path(); // to get "authority_end"

            auto const _data = this->string_view();

            auto const starting_point = user_info_end != data.size() ? user_info_end : authority_start;
            auto const host_port      = _data.substr(starting_point, authority_end - starting_point);

            // caution about IPv6; host_port may contain something like [2001:db8::7]:8080
            port_start = host_port.find_last_of(":]");
            if (port_start == string_view_type::npos || host_port.back() == ']') {
                port_start = data.size(); // there's no port
            } else {
                port_start += starting_point;
                const auto port_view = _data.substr(port_start + 1, authority_end - (port_start + 1));
                if (port_view.empty()) {
                    // example.com:/page
                    // port is empty; empty is default
                    return; // nothing to do
                } else if (!ascii::is::digit(port_view)) {
                    port_start  = data.size();
                    errors.port = true;
                } else {
                    // unfortunately this is the best way that I could think of
                    // but if the user wants the port as well, then we have converted this value twice
                    // not efficient; I know.
                    auto const p = to_int(port_view);
                    if (p < 0 || p > max_port_number) {
                        errors.port = true;
                    }
                }
            }
        }

        /**
         * parse fragment (it finds fragment_start)
         */
        constexpr void parse_fragment() const noexcept {
            if (fragment_start != string_view_type::npos)
                return; // It's already parsed

            auto const _data = this->string_view();

            // todo: evaluate fragment here
            fragment_start = _data.find_first_of('#');
            if (fragment_start == string_view_type::npos) {
                fragment_start = data.size();
            }
        }

        /**
         * parse query; it ensures that query_start and fragment_start are changed
         */
        constexpr void parse_query() const noexcept {
            if (query_start != string_view_type::npos)
                return; // there's nothing to do

            parse_fragment();

            auto const _data = this->string_view();
            query_start      = _data.substr(0, fragment_start).find_first_of('?');
            // todo: evaluate queries here
            if (query_start == string_view_type::npos) {
                query_start = data.size();
            }
        }

        /**
         * parse the host
         */
        constexpr void parse_host() const noexcept {
            parse_user_info(); // to get "authority_start" and "user_info_end"
            parse_port();      // to get "port_start" and hopefully "authority_end"
            parse_path();      // to make sure we have "authority_end"
        }

        constexpr void parse_all() const noexcept {
            parse_host();
        }

        /**
         * Remove the cache and make sure calling the functions will cause
         * re-parsing the uri.
         */
        constexpr inline void unparse() const noexcept {
            scheme_end = authority_start = user_info_end = port_start = authority_end = query_start =
              fragment_start = string_view_type::npos;
            errors.reset();
        }

        /**
         * Replace the specified part with the specified replacement
         */
        template <typename RT>
            requires(istl::StringifiableOf<string_type, RT>)
        constexpr void replace_value(stl::size_t start, stl::size_t len, RT&& _replacement) noexcept {
            static_assert(is_mutable(), "You cannot change a const_uri (string_view is not modifiable)");
            auto replacement =
              istl::stringify_of<string_type>(stl::forward<RT>(_replacement), this->get_allocator());
            if (start == string_view_type::npos || len == string_view_type::npos ||
                (len == 0 && replacement.empty()))
                return;
            // todo: check performance of this
            auto const after_replacement_start = stl::min(data.size(), start + len);
            data.reserve(start + replacement.size() + (data.size() - after_replacement_start));
            data = strings::join<stored_str_t>(substr(0, start),               // first part
                                               stl::move(replacement),         // replacement part
                                               substr(after_replacement_start) // the rest
            );
            unparse();
            // TODO: you may want to not unparse everything
        }



      public:
        constexpr uri_string(const char_type* u) noexcept : data(u) {}


        /**
         * @brief parse from string, it will trim the spaces for generality too
         * @param string_view URI string
         */
        constexpr uri_string(stored_str_t const& u) noexcept : data(u) {}

        /**
         * If the user uses this
         */
        constexpr uri_string(stored_str_t&& u) noexcept : data(stl::move(u)) {}

        constexpr uri_string(uri_string const& bu) noexcept
          : alloc_holder_type{bu.get_allocator()},
            data{bu.data},
            scheme_end{bu.scheme_end},
            authority_start{bu.authority_start},
            user_info_end{bu.user_info_end},
            port_start{bu.port_start},
            authority_end{bu.authority_end},
            query_start{bu.query_start},
            fragment_start{bu.fragment_start} {}

        constexpr uri_string(uri_string&& bu) noexcept
          : alloc_holder_type{bu.get_allocator()},
            data{stl::move(bu.data)},
            scheme_end{stl::move(bu.scheme_end)},
            authority_start{stl::move(bu.authority_start)},
            user_info_end{stl::move(bu.user_info_end)},
            port_start{stl::move(bu.port_start)},
            authority_end{stl::move(bu.authority_end)},
            query_start{stl::move(bu.query_start)},
            fragment_start{stl::move(bu.fragment_start)} {}



        // assignment operators
        constexpr uri_string& operator=(uri_string const& u) noexcept {
            if (this != &u) {
                data            = u.data;
                scheme_end      = u.scheme_end;
                authority_start = u.authority_start;
                user_info_end   = u.user_info_end;
                port_start      = u.port_start;
                authority_end   = u.authority_end;
                query_start     = u.query_start;
                fragment_start  = u.fragment_start;
            }
            return *this;
        }

        constexpr uri_string& operator=(uri_string&& u) noexcept {
            if (this != &u) {
                data            = stl::move(u.data);
                scheme_end      = stl::move(u.scheme_end);
                authority_start = stl::move(u.authority_start);
                user_info_end   = stl::move(u.user_info_end);
                port_start      = stl::move(u.port_start);
                authority_end   = stl::move(u.authority_end);
                query_start     = stl::move(u.query_start);
                fragment_start  = stl::move(u.fragment_start);
            }
            return *this;
        }

        //        uri_string& operator=(stored_str_t const& u) noexcept {
        //            data = u;
        //            unparse();
        //            return *this;
        //        }
        //
        //        uri_string& operator=(stored_str_t&& u) noexcept {
        //            data = stl::move(u);
        //            unparse();
        //            return *this;
        //        }

        constexpr uri_string(allocator_type const& inp_alloc = allocator_type{}) noexcept
          : alloc_holder_type{inp_alloc} {
            static_assert(is_mutable(),
                          "You can't modify this basic_uri, there's no point in "
                          "constructing this class with allocator since it can't be changed"
                          " and thus can't be used in any way.");
        }

        constexpr ~uri_string() noexcept = default;

        static constexpr bool is_mutable() noexcept {
            return Mutable;
        }

        template <typename StringType, typename StringViewType>
        constexpr bool operator==(const uri_string<StringType, StringViewType>& other) const noexcept {
            return ascii::iequals(string(), other.string());
        }

        template <typename StringType, typename StringViewType>
        constexpr bool operator!=(const uri_string<StringType, StringViewType>& other) const noexcept {
            return !ascii::iequals(string(), other.string());
        }

        constexpr bool operator==(string_view_type u) const noexcept {
            return ascii::iequals(string(), u);
        }

        constexpr bool operator!=(string_view_type u) const noexcept {
            return !ascii::iequals(string(), u);
        }

        /**
         * Get a part of the uri
         * @param start
         * @param len
         * @return string_view_type
         */
        [[nodiscard]] constexpr string_view_type
        substr(stl::size_t start = 0, stl::size_t len = string_view_type::npos) const noexcept {
            if ((len == 0) || (start > data.size()))
                return {};
            if constexpr (stl::is_same_v<stored_str_t, string_view_type>) {
                return data.substr(start, len);
            } else {
                return string_view_type(data).substr(start, len);
            }
        }

        /**
         * @brief this function is the same as "encodeURI" in javascript
         */
        constexpr void encode_to(istl::String auto& output) const noexcept {
            encode_uri_component(string(), output, details::ALLOWED_CHARACTERS_IN_URI<char_type>);
        }

        /**
         * @brief this function is the same as "decodeURI" in javascript
         * @return this function will return an optional<string> object. it will
         * be nullopt when the uri is not valid and has invalid characters
         */
        [[nodiscard]] constexpr bool decode_to(istl::String auto& output) const noexcept {
            return decode_uri_component(string(), output, details::ALLOWED_CHARACTERS_IN_URI<char_type>);
        }



        ///////////////////////////////////////////// Scheme /////////////////////////////////////////////


        /**
         * @brief checks if the URI is a relative reference
         */
        [[nodiscard]] constexpr bool is_relative_reference() const noexcept {
            return !has_scheme();
        }

        /**
         * @brief check if the specified uri has a scheme or not
         */
        [[nodiscard]] constexpr bool has_scheme() const noexcept {
            parse_scheme();
            return !(scheme_end == data.size() && scheme_end != 0);
        }

        /**
         * @brief scheme
         * @return get scheme
         */
        [[nodiscard]] constexpr string_view_type scheme() const noexcept {
            parse_scheme();
            return scheme_end == data.size() ? string_view_type() : substr(0, scheme_end);
        }

        /**
         * @brief set scheme
         * @param _scheme
         * @throws logic_error if uri is const
         */
        constexpr uri_string& scheme(string_view_type m_scheme) {
            if (ascii::ends_with(m_scheme, ':'))
                m_scheme.remove_suffix(1);
            if (!is::scheme(m_scheme))
                throw stl::invalid_argument("The specified scheme is not valid");
            parse_scheme();
            if (scheme_end != data.size()) {
                replace_value(0,
                              m_scheme.empty() && data.size() > scheme_end + 1 && data[scheme_end] == ':'
                                ? scheme_end + 1
                                : scheme_end,
                              m_scheme);
            } else {
                // the URI doesn't have a scheme now, we have to put it in the right place
                auto scheme_colon = m_scheme.empty() ? "" : strings::join(string_type(m_scheme), ':');
                if (authority_start != data.size()) {
                    replace_value(0,
                                  0,
                                  strings::join(scheme_colon, (ascii::starts_with(data, "//") ? "" : "//")));
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
        constexpr uri_string& clear_scheme() noexcept {
            return scheme("");
        }

        ///////////////////////////////////////////// User Info /////////////////////////////////////////////



        /**
         * @brief checks if the uri has user info or not
         */
        [[nodiscard]] constexpr bool has_user_info() const noexcept {
            parse_user_info();
            return user_info_end != data.size() && authority_start != data.size();
        }

        /**
         * @brief get the user info or an empty value
         */
        [[nodiscard]] constexpr string_view_type user_info_raw() const noexcept {
            parse_user_info();
            return (user_info_end == data.size() || authority_start == data.size())
                     ? string_view_type()
                     : substr(authority_start, user_info_end - authority_start);
        }

        /**
         * @brief decode user_info and return it as a string
         */
        [[nodiscard]] constexpr string_type user_info() const noexcept {
            string_type out{this->get_allocator()};
            if (!decode_uri_component(user_info_raw(), out, details::USER_INFO_NOT_PCT_ENCODED<char_type>)) {
                out.clear();
            }
            return out;
        }

        /**
         * @brief set the user info if it's possible
         */
        constexpr uri_string& user_info(string_view_type info) noexcept {
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
                        replace_value(0, 0, strings::join(string_type("//"), encoded_info, "@"));
                    } else {
                        // there's scheme and we have to put it after that
                        replace_value(scheme_end + 1, 0, strings::join(string_type("//"), encoded_info, "@"));
                    }
                } else {
                    // there's authority start but there's no user_info_end
                    replace_value(authority_start, 0, strings::join(encoded_info, "@"));
                }
            }
            return *this;
        }

        /**
         * @brief clears the user info if exists
         */
        constexpr uri_string& clear_user_info() noexcept {
            return user_info("");
        }


        /**
         * Check if the user info has a username in it or not
         * @return bool
         */
        [[nodiscard]] constexpr bool has_username() const noexcept {
            return user_info_end != data.size();
        }

        /**
         * Get the username in the user info if it exists or otherwise an empty
         * string view
         * @return
         */
        [[nodiscard]] constexpr string_view_type username_raw() const noexcept {
            auto _userinfo = user_info_raw();
            if (auto colon = _userinfo.find(':'); colon != string_view_type::npos)
                _userinfo.remove_suffix(_userinfo.size() - colon);
            return _userinfo;
        }

        /**
         * Get the host version of the username if it exists
         * @return
         */
        [[nodiscard]] constexpr stl::optional<string_type> username() const noexcept {
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
        [[nodiscard]] constexpr bool has_password() const noexcept {
            return user_info_raw().find(':') != string_view_type::npos;
        }

        /**
         * The password in the user info
         * @return
         */
        [[nodiscard]] constexpr string_view_type password_raw() const noexcept {
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
        [[nodiscard]] constexpr stl::optional<string_type> password() const noexcept {
            string_type out{this->get_allocator()};
            if (decode_uri_component(password_raw(), out, details::USER_INFO_NOT_PCT_ENCODED<char_type>)) {
                return out;
            }
            return stl::nullopt;
        }


        ///////////////////////////////////////////// Host /////////////////////////////////////////////

        /**
         * @brief return host as an string_view
         * @return string_view
         */
        [[nodiscard]] constexpr string_view_type host_raw() const noexcept {
            parse_host();
            if (authority_start == data.size()) {
                // there will not be a host without the authority_start
                return {};
            }

            stl::size_t start, len; // NOLINT(cppcoreguidelines-init-variables)

            // you know I could do this in one line of code, but I did this
            // because I don't want you to curse me :)

            // we have authority_start, let's check user_info and port too
            if (user_info_end == data.size()) {
                // there's no user info
                start = authority_start;
            } else {
                // there's a user info
                start = user_info_end;
                if (data[user_info_end] == '@')
                    start++;
            }

            // todo: see if it makes sense to use stl::min/max
            // finding the length
            if (port_start != data.size()) {
                // but there's a port
                len = port_start - start;
            } else if (authority_end != data.size()) {
                // there's a path
                len = authority_end - start;
            } else if (query_start != data.size()) {
                // there's a query
                len = query_start - start;
            } else if (fragment_start != data.size()) {
                // there's a fragment
                len = fragment_start - start;
            } else if (start == data.size()) {
                // there's no host at all
                return {};
            } else {
                // there's no path, query, fragment; nothing; the host continues to the end
                len = data.size() - 1; // till the end
            }

            return substr(start, len);
        }

        /**
         * @brief returns const_ipv4/const_ipv6/hostname; if the URI doesn't
         * include a valid ip/hostname you'll get an empty string. this method
         * will only return the hostname/ip if it's in the correct format and
         * doesn't include invalid syntax.
         * todo: return a host_authority maybe? or a variant<address, string_view>?
         */
        [[nodiscard]] constexpr stl::variant<ipv4, ipv6, string_view_type> host_structured() const noexcept {
            auto _host = host_raw();
            if (webpp::is::ipv4(_host))
                return ipv4(_host);
            if (webpp::is::ipv6(_host))
                return ipv6(_host);
            return _host;
        }

        /**
         * @brief get the host version of hostname/ip of the uri or an empty
         * string if the specified URI does not include a hostname/ip or its
         * hostname has the wrong character encodings.
         * @return string
         */
        [[nodiscard]] constexpr string_type host() const noexcept {
            string_type d_host{this->get_allocator()};
            const auto  raw = host_raw();

            // don't decode the component if it's ipv6
            if (ascii::starts_with(raw, '[') && webpp::is::ipv6(raw)) {
                d_host = raw;
                return d_host;
            }
            if (!decode_uri_component(raw, d_host, details::REG_NAME_NOT_PCT_ENCODED<char_type>)) {
                d_host.clear();
            }
            return d_host;
        }

        /**
         * @brief this method will check if the hostname/ip exists in the uri or not.
         * @return true if it can't find a hostname/ip in the uri or it's empty
         */
        [[nodiscard]] constexpr bool has_host() const noexcept {
            return !host_raw().empty();
        }

        /**
         * @brief set the hostname/ip in the uri if possible
         */
        constexpr uri_string& host(string_view_type new_host) noexcept {
            parse_host();

            // todo: are you sure it can handle punycode as well?
            string_type encoded_host{this->get_allocator()};
            encode_uri_component(new_host, encoded_host, details::REG_NAME_NOT_PCT_ENCODED<char_type>);
            if ((!ascii::starts_with(new_host, '[') || !ascii::ends_with(new_host, ']')) &&
                webpp::is::ipv6(new_host)) {
                encoded_host = strings::join('[', encoded_host, ']');
            }

            if (authority_start == data.size()) {
                // there's no authority start

                if (encoded_host.empty())
                    return *this; // there's nothing to do here. It's already
                // what the user wants

                if (scheme_end == data.size()) {
                    // there's no scheme either, so we just have to add to the
                    // beginning of the string
                    replace_value(0, 0, strings::join("//", encoded_host));
                    return *this;
                } else {
                    // there's a scheme
                    replace_value(scheme_end, 0, strings::join("//", encoded_host));
                    return *this;
                }
            }

            stl::size_t start, finish; // NOLINT(cppcoreguidelines-init-variables)

            // you know I could do this in one line of code, but I did this
            // because I don't want you to curse me :)

            // we have authority_start, let's check user_info and port too
            if (user_info_end == data.size()) {
                // there's no user info
                if (scheme_end == data.size()) {
                    start = 0;
                    if (!new_host.empty() && !ascii::starts_with(string_view_type{encoded_host}, "//")) {
                        encoded_host = strings::join("//", encoded_host);
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
        [[nodiscard]] constexpr stl::variant<ipv4, ipv6, string_type>
        host_structured_decoded() const noexcept {
            if (auto _host_structured = host_structured();
                stl::holds_alternative<string_view_type>(_host_structured)) {
                // convert string_view to string and then decode it
                string_type output{this->get_allocator()};
                if (decode_uri_component(stl::get<string_view_type>(_host_structured),
                                         output,
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
        constexpr uri_string& clear_host() noexcept {
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
        [[nodiscard]] constexpr bool is_ip() const noexcept {
            auto _host = host_raw();
            return webpp::is::ipv4(_host) || (ascii::starts_with(_host, '[') && ascii::ends_with(_host, ']'));
        }

        /**
         * Get the host and split it by dot separator. TLD (Top Level Domain)
         * will be the last one and Second Level Domain will be the one before
         * that and the rest will be subdomains.
         */
        [[nodiscard]] constexpr auto domains() const noexcept {
            using vec_type =
              stl::vector<string_type, typename alloc_holder_type::template allocator_type_as<string_type>>;
            auto     _host = host_raw();
            vec_type subs{this->template get_allocator_as<string_type>()};
            if (_host.empty() || is_ip())
                return subs;
            for (;;) {
                auto dot = _host.find('.');
                auto sub = _host.substr(0, dot);
                if (sub.empty())
                    break;
                subs.emplace_back(sub, this->get_allocator());
                if (dot == string_view_type::npos)
                    break;
                _host.remove_prefix(dot + 1);
            }
            return subs;
        }

        /**
         * Get the TLD (top level domain) or sometimes called extension
         */
        [[nodiscard]] constexpr string_view_type top_level_domain() const noexcept {
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
        constexpr uri_string& top_level_domain(string_view_type tld) noexcept {
            auto _host = host_raw();
            if (_host.empty()) {
                // I've already written that code. Yay, I'm so happy
                host(tld);
            } else if (!is_ip() && !webpp::is::ip(tld)) {
                // cannot put an ip address as a tld, user should use set host
                // instead of this method.
                auto dot   = _host.find_last_of('.');
                auto start = dot != string_view_type::npos ? dot + 1 : 0;
                host(strings::join(string_type(_host.substr(0, start), this->get_allocator()),
                                   string_type(tld, this->get_allocator())));
            }
            return *this;
        }

        /**
         * Check if the specified uri has a top level domain (TLD) or not
         * @return an indication of weather or not the URI has TLD or not
         */
        [[nodiscard]] constexpr bool has_top_level_domain() const noexcept {
            return !top_level_domain().empty();
        }

        /**
         * Get the second level domain out of the host
         */
        [[nodiscard]] constexpr string_view_type second_level_domain() const noexcept {
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
        constexpr uri_string& second_level_domain(string_view_type sld) noexcept {
            auto _host = host_raw();
            if (_host.empty() || is_ip())
                return *this;

            auto last_dot = _host.find_last_of('.');
            if (last_dot == string_view_type::npos) {
                // we have to insert it at the beginning of the host string

                // there's nothing to do it's empty
                if (!sld.empty()) {
                    static_cast<void>(host(strings::join(string_type(sld, this->get_allocator()),
                                                         '.',
                                                         string_type(_host, this->get_allocator()))));
                }
            } else {
                auto bef_last_dot = _host.find_last_of('.', last_dot - 1);
                auto start        = bef_last_dot == string_view_type::npos ? 0 : bef_last_dot + 1;
                if (!sld.empty())
                    static_cast<void>(
                      host(strings::join(string_type(_host.substr(0, start), this->get_allocator()),
                                         string_type(sld, this->get_allocator()),
                                         string_type(_host.substr(last_dot), this->get_allocator()))));
                else
                    static_cast<void>(host(string_type(_host.substr(last_dot + 1), this->get_allocator())));
            }
            return *this;
        }

        /**
         * This method will remove the Second Level Domain and also any
         * Sub-Domains if there are any.
         */
        constexpr uri_string& clear_second_level_domain() noexcept {
            return second_level_domain({});
        }

        /**
         * Check if the specified uri has a second level domain or not.
         * @return An indication of weather or not the URI has SLD or not
         */
        [[nodiscard]] constexpr bool has_second_level_domain() const noexcept {
            return !second_level_domain().empty();
        }

        /**
         * Get the sub-domain (with sub-sub-...-sub-domain)
         * @return
         */
        [[nodiscard]] constexpr string_view_type subdomains() const noexcept {
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
        constexpr uri_string& subdomains(string_view_type sds) noexcept {
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
            static_cast<void>(host(strings::join(string_type(sds), string_type(_host.substr(bef_last_dot)))));
            return *this;
        }

        /**
         * Remove the sub-domains if exists. This method should not have
         * side-effects if there's no sub-domain
         */
        constexpr uri_string& clear_subdomains() noexcept {
            return subdomains({});
        }

        /**
         * Check if the specified uri has at least one sub-domain or not
         * @return an indication of weather or not the URI has subdomains or not
         * TODO: we could be smarter here
         */
        [[nodiscard]] constexpr bool has_subdomains() const noexcept {
            return !subdomains().empty();
        }



        ///////////////////////////////////////////// Port /////////////////////////////////////////////

        /**
         * Get the default port for the specified scheme
         * TODO: make this function a free function
         */
        [[nodiscard]] constexpr uint16_t default_port() const noexcept {
            // todo: use /etc/services instead of manually doing this and revert to manually for constexpr

            // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
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
            // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

            // TODO: add more protocols here
            return 0u;
        }

        /**
         * @brief port number of the uri;
         * @return port number
         * @default 80
         */
        [[nodiscard]] constexpr uint16_t port_uint16() const noexcept {
            if (has_port()) {
                return static_cast<uint16_t>(to_uint(port()));
            }
            return default_port();
        }

        /**
         * Get the port in a string_view format; there's no default value.
         * @return string_view
         */
        [[nodiscard]] constexpr string_view_type port() const noexcept {
            parse_port();

            // there's no port
            if (port_start == data.size())
                return {};

            // don't worry authority_end will be the end of the string anyway
            return substr(port_start + 1,
                          (authority_end == data.size() ? authority_end - 1 : authority_end) -
                            (port_start + 1));
        }

        /**
         * @brief checks if the uri has explicitly specified a port number or
         * not
         * @return bool
         */
        [[nodiscard]] constexpr bool has_port() const noexcept {
            parse_port();
            return port_start != data.size();
        }

        /**
         * @brief set port
         * @param new_port
         */
        constexpr uri_string& port(uint16_t new_port) noexcept {
            string_type str{this->get_allocator()};
            append_to(str, new_port);
            return port(str);
        }

        /**
         * Set new port value
         * @param new_port
         */
        constexpr uri_string& port(string_view_type new_port) noexcept {
            if (ascii::starts_with(new_port, ':'))
                new_port.remove_prefix(1);
            if (!ascii::is::digit(new_port)) {
                errors.port = true;
                return *this;
            }
            parse_port();
            if (port_start == data.size()) {
                // there's no port, I have to insert it myself:
                parse_host();

                if (authority_end != data.size()) {
                    // found it at the end of the line
                    replace_value(authority_end, 0, strings::join(':', string_type(new_port)));
                } else if (user_info_end != data.size()) {
                    // there's authority and there might be a host
                    replace_value(user_info_end + 1,
                                  user_info_end + 1,
                                  strings::join(':', string_type(new_port)));
                } else if (authority_start != data.size()) {
                    // there's a authority_start at least
                    replace_value(authority_start + 1, 0, strings::join(':', string_type(new_port)));
                } else {
                    // there's no authority at all.
                    if (scheme_end == data.size()) {
                        // there's no scheme either
                        replace_value(0, 0, strings::join("//:", string_type(new_port)));
                    } else {
                        // there's scheme
                        replace_value(scheme_end + 1, 0, strings::join("//:", string_type(new_port)));
                    }
                }
            } else {
                // there's a port and we are going to just replace it
                replace_value(port_start + 1,
                              authority_end == data.size() ? authority_end - 1 : authority_end,
                              new_port);
            }
            return *this;
        }

        /**
         * @brief clear the port number from the uri and defaults to 80
         * @return self
         */
        constexpr uri_string& clear_port() {
            return port({});
        }




        ///////////////////////////////////////////// Path /////////////////////////////////////////////




        /**
         * @brief check if the URI has a path or not
         * @return bool true if the URI doesn't have a path
         */
        [[nodiscard]] constexpr bool has_path() const noexcept {
            parse_path();
            return authority_end != data.size();
        }

        /**
         * @brief get path in non-host, string format
         */
        [[nodiscard]] constexpr string_view_type path_raw() const noexcept {
            if (!has_path())
                return {};
            return substr(authority_end, stl::min(query_start, fragment_start) - authority_end);
        }

        /**
         * @brief host path as a string
         * @return std::optional<string> the string might not have the right format
         */
        [[nodiscard]] constexpr string_type path() const noexcept {
            string_type res{this->get_allocator()};
            if (!decode_uri_component(path_raw(), res, details::LEGAL_PATH_CHARS<char_type>)) {
                res.clear();
            }
            return res;
        }

        /**
         * Get the path slugs with the specified container type
         * We know how to get the allocator, don't worry.
         * The specified container's string type should be a string and not a string_view
         */
        template <typename Container = basic_path<string_type>>
        [[nodiscard]] constexpr Container slugs() const noexcept {
            using value_type = typename Container::value_type;
            Container container(this->template get_allocator_as<value_type>());
            if (!extract_slugs_to<Container>(container)) {
                container.clear();
            }
            return container;
        }

        /**
         * Get the non-host slugs.
         * You can use string_view as the underlying string type of the container since we don't
         * decode the string. As long as the class has access to the string_view, this method has too.
         */
        template <typename Container = basic_path<string_type>>
        [[nodiscard]] constexpr Container raw_slugs() const noexcept {
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
         *
         * todo: possible usage of path_view if you have any of those
         */
        template <typename Container = basic_path<string_type>>
        constexpr uri_string const& extract_raw_slugs_to(Container& container) const noexcept {
            auto _path = path_raw();
            if (_path.empty())
                return *this;
            // if (_path.front() == '/') {
            //     container.emplace_back(); // empty string
            // }
            for (;;) {
                const stl::size_t slash_start = _path.find('/');
                const stl::size_t the_size    = stl::min(slash_start, _path.size());
                container.emplace_back(_path.data(), the_size);
                if (slash_start == string_view_type::npos) {
                    break;
                }
                _path.remove_prefix(slash_start + 1);
            }
            return *this;
        }

        /**
         * @brief this one will return a container containing host strings of the path.
         * @attention do not use string_view or any alternatives for this method
         * as this method should own its data.
         */
        template <typename Container = basic_path<string_type>>
        [[nodiscard]] constexpr bool extract_slugs_to(Container& container) const noexcept {
            using vec_str_t       = typename Container::value_type;
            using difference_type = typename Container::difference_type;
            static_assert(istl::String<vec_str_t>,
                          "The specified container doesn't hold a value type that we can understand.");
            const auto beg_it = static_cast<difference_type>(container.size());
            extract_raw_slugs_to(container);
            for (auto it = container.begin() + beg_it; it != container.end(); ++it) {
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
            requires(!istl::StringViewifiable<Container> &&
                     requires(Container c) {
                         c.begin();
                         c.end();
                     })
        constexpr uri_string& path(const Container& m_path) noexcept {
            return path(m_path.begin(), m_path.end());
        }

        template <typename Container>
            requires(!istl::StringViewifiable<Container> &&
                     requires(Container c) {
                         c.begin();
                         c.end();
                     })
        constexpr uri_string& path_raw(const Container& m_path) noexcept {
            return path_raw(m_path.begin(), m_path.end());
        }

        /**
         * Set path by begin and end of an iterator
         */
        template <typename Iter>
        constexpr uri_string& path(const Iter& _start, const Iter& _end) noexcept {
            webpp_static_constexpr double string_reserve_scale = 1.5;

            const auto  almost_end = stl::prev(_end);
            string_type new_path{this->get_allocator()};

            // reserve
            stl::size_t new_path_size = 0;
            for (auto it = _start; it != _end; ++it) {
                new_path_size += ascii::size(*it);
            }
            new_path_size += static_cast<stl::size_t>(stl::distance(_start, _end));
            new_path.reserve(static_cast<stl::size_t>(
              static_cast<double>(new_path_size) *
              string_reserve_scale)); // add 1.5 because encoding is going to need space

            for (auto it = _start; it != almost_end; ++it) {
                encode_uri_component(*it, new_path, details::PCHAR_NOT_PCT_ENCODED<char_type>);
                new_path.append("/");
            }
            // append the last slug
            encode_uri_component(*almost_end, new_path, details::PCHAR_NOT_PCT_ENCODED<char_type>);
            return path_raw(stl::move(new_path));
        }

        template <typename Iter>
        constexpr uri_string& path_raw(const Iter& _start, const Iter& _end) noexcept {
            const auto  almost_end = stl::prev(_end);
            string_type new_path{this->get_allocator()};

            // do it in one allocation
            stl::size_t new_path_size = 0;
            for (auto it = _start; it != _end; ++it) {
                new_path_size += ascii::size(*it);
            }
            new_path_size += static_cast<stl::size_t>(stl::distance(_start, _end));
            if (new_path_size == 0)
                return *this;
            new_path.reserve(new_path_size);

            for (auto it = _start; it != almost_end; ++it) {
                new_path.append(*it);
                new_path.append("/");
            }
            new_path.append(*almost_end);
            return path_raw(stl::move(new_path));
        }

        /**
         * @brief set the path for the uri
         */
        constexpr uri_string& path(string_view_type m_path) noexcept {
            string_type str(this->get_allocator());
            encode_uri_component(
              m_path,
              str,
              charset(details::PCHAR_NOT_PCT_ENCODED<char_type>, charset<char_type, 1>('/')));
            return path_raw(stl::move(str));
        }

        constexpr uri_string& path_raw(string_view_type m_path) noexcept {
            parse_path();
            auto _encoded_path =
              strings::join((!ascii::starts_with(m_path, '/') ? "/" : ""),
                            istl::stringify_of<string_type>(m_path, this->get_allocator()));
            replace_value(authority_end, query_start - authority_end, stl::move(_encoded_path));
            return *this;
        }

        /**
         * @brief clear path from the URI
         */
        constexpr uri_string& clear_path() noexcept {
            return path_raw("");
        }

        /**
         * @brief checks if the path is an absolute path or relative path
         * @return bool
         */
        [[nodiscard]] constexpr bool is_path_absolute() const noexcept {
            return ascii::starts_with(path_raw(), '/');
        }

        /**
         * @brief checks if the path is a relative path or an absolute one
         * @return bool
         */
        [[nodiscard]] constexpr bool is_path_relative() const noexcept {
            return !is_path_absolute();
        }

        /**
         * @brief checks if the uri path is normalized or not (contains relative . or .. paths)
         * @details since this is supposed to be a URI, this check doesn't care if the path is relative or
         * not
         * @return bool
         */
        [[nodiscard]] constexpr bool is_normalized() const noexcept {
            auto m_path = raw_slugs();
            return stl::all_of(m_path.cbegin(), m_path.cend(), [](auto const& p) constexpr noexcept {
                return p != "." && p != "..";
            });
        }

        /**
         * @details This method applies the "remove_dot_segments" routine talked
         * about in RFC 3986 (https://tools.ietf.org/html/rfc3986) to the path
         * segments of the URI, in order to normalize the path
         * (apply and remove "." and ".." segments).
         */
        constexpr uri_string& normalize() noexcept {
            // todo: you can use a better algorithm!
            auto the_slugs = raw_slugs();
            the_slugs.normalize();
            path_raw(the_slugs);
            return *this;
        }



        ///////////////////////////////////////////// Queries /////////////////////////////////////////////


        /**
         * @brief checks if the uri has query or not
         */
        [[nodiscard]] constexpr bool has_queries() const noexcept {
            parse_query();
            return query_start != data.size();
        }

        [[nodiscard]] constexpr string_view_type queries_raw() const noexcept {
            parse_query();
            if (query_start == data.size())
                return {};
            return substr(query_start + 1, fragment_start - query_start - 1);
        }

        /**
         * Get the query in a decoded string format
         */
        [[nodiscard]] constexpr string_type queries_string() const noexcept {
            string_type d_queries{this->get_allocator()};
            if (!decode_uri_component(queries_raw(),
                                      d_queries,
                                      details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>)) {
                d_queries.clear();
            }
            return d_queries;
        }

        template <typename MapType = basic_queries<string_type, allocator_type>>
        [[nodiscard]] constexpr MapType queries() const noexcept {
            MapType res(this->get_allocator());
            extract_queries_to(res);
            return res;
        }

        /**
         * Set queries
         */
        constexpr uri_string& queries(string_view_type m_query) {
            if (!webpp::is::query(m_query))
                throw stl::invalid_argument("The specified string is not a valid query");

            string_type encoded_query(this->get_allocator());
            if (ascii::starts_with(m_query, '?')) {
                encoded_query.append("?");
            }
            encode_uri_component(m_query,
                                 encoded_query,
                                 details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>);

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
                                replace_value(0, 0, strings::join("///", encoded_query));
                            } else {
                                replace_value(scheme_end, 0, strings::join("/", encoded_query));
                            }
                        } else {
                            replace_value(authority_start, 0, strings::join("/", encoded_query));
                        }
                    } else {
                        // we have authority_end
                        if (data[authority_end] == '/') {
                            replace_value(authority_end + 1, 0, encoded_query);
                        } else {
                            replace_value(authority_end + 1, 0, strings::join("/", encoded_query));
                        }
                    }
                }
            } else {
                // we have query
                replace_value(query_start, fragment_start - query_start, encoded_query);
            }
            return *this;
        }

        /**
         * Append queries from a container like std::map or std::multimap
         */
        template <typename MapIter>
        constexpr uri_string& set_queries_from(MapIter const& _queries_begin, MapIter const& _queries_end) {
            static_assert(is_mutable(), "You can't use this method on a non-modifiable uri struct.");
            using map_key_type   = typename MapIter::first_type;
            using map_value_type = typename MapIter::second_type;
            static_assert(istl::StringViewifiable<map_key_type> && istl::StringViewifiable<map_value_type>,
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
                encode_uri_component(it->second,
                                     value,
                                     details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>);
                if (name.empty()) // when name is empty, we just don't care
                    continue;
                _query_data.append(name);
                if (!value.empty()) {
                    _query_data.append("=");
                    _query_data.append(value);
                }
                if (stl::next(it) != _queries_end) {
                    _query_data.append("&");
                }
            }
            set(stl::move(_query_data));
            return *this;
        }

        template <typename MapType>
        constexpr uri_string& set_queries_from(MapType const& _queries) {
            return set_from(_queries.begin(), _queries.end());
        }

        /**
         * @brief clear the query section of the URI
         */
        constexpr uri_string& clear_queries() noexcept {
            return queries("");
        }

        /**
         * Get the query in as a map<string, string>
         * It's also in a decoded format
         */
        template <typename MapType = basic_queries<string_type, allocator_type>>
        constexpr uri_string const& extract_queries_to(MapType& q_structured) const noexcept {
            using map_key_type   = typename MapType::key_type;
            using map_value_type = typename MapType::mapped_type;
            static_assert(istl::String<map_key_type>, "The specified container can't hold the query keys.");
            static_assert(istl::String<map_value_type>,
                          "The specified container can't hold the query values.");
            auto _query = queries_raw();
            for (; !_query.empty();) {
                const auto and_sep = _query.find('&'); // find the delimiter
                const auto eq_sep  = _query.find('=');
                const auto name    = _query.substr(0, stl::min(eq_sep, and_sep));
                if (name.empty()) { // a name should not be empty
                    _query.remove_prefix(stl::max(eq_sep, and_sep) + 1);
                    continue;
                }
                map_value_type d_name(this->get_allocator());
                map_key_type   d_value(this->get_allocator());
                if (!decode_uri_component(name,
                                          d_name,
                                          details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>)) {
                    d_name = name; // just put the non-decoded string there
                }
                if (eq_sep != string_view_type::npos) { // we have a value as well
                    auto const value = _query.substr(eq_sep + 1, and_sep);
                    if (!decode_uri_component(value,
                                              d_value,
                                              details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>)) {
                        d_value = value;
                    }
                }

                if (!d_name.empty())
                    q_structured[d_name] = stl::move(d_value);

                if (and_sep != string_view_type::npos) {
                    _query.remove_prefix(and_sep + 1);
                } else {
                    break;
                }
            }
            return *this;
        }

        constexpr uri_string& append_query(string_view_type key, string_view_type value) noexcept {
            // todo
            return *this;
        }



        ///////////////////////////////////////////// Fragment /////////////////////////////////////////////



        /**
         * @brief get fragment
         */
        [[nodiscard]] constexpr string_view_type fragment() const noexcept {
            parse_fragment();
            return substr(fragment_start + 1);
        }

        /**
         * @brief replace the old fragment
         * todo: write tests for this
         */
        constexpr uri_string& fragment(string_view_type str) noexcept {
            if (str.empty()) {
                // remove fragment
                replace_value(fragment_start, data.size() - fragment_start, "");
                return *this;
            }
            string_type encoded_fragment{this->get_allocator()};
            if (!ascii::starts_with(str, '#'))
                encoded_fragment += '#';
            encode_uri_component(str,
                                 encoded_fragment,
                                 details::QUERY_OR_FRAGMENT_NOT_PCT_ENCODED<char_type>);
            replace_value(fragment_start, data.size() - fragment_start, stl::move(encoded_fragment));
            return *this;
        }

        /**
         * @brief an indication of whether the URI has fragment or not.
         * @return
         */
        [[nodiscard]] constexpr bool has_fragment() const noexcept {
            parse_fragment();
            return fragment_start != data.size();
        }

        /**
         * @brief clear the fragment part of the uri
         * @return
         */
        constexpr uri_string& clear_fragment() noexcept {
            return fragment("");
        }



        /**
         * This method returns an indication of whether or not the URI includes
         * any element that is part of the authority URI.
         * @return bool
         */
        [[nodiscard]] constexpr bool has_authority() const noexcept {
            return has_host() || has_user_info() || has_port();
        }

        /**
         * Get the string representation of the uri
         * @return string
         */
        [[nodiscard]] constexpr stored_str_t const& string() const noexcept {
            return data;
        }

        [[nodiscard]] constexpr stored_str_t const& to_string() const noexcept {
            return data;
        }

        /**
         * Get a string_view version of the uri
         * @return string_view_type
         */
        [[nodiscard]] constexpr string_view_type string_view() const noexcept {
            if constexpr (stl::is_same_v<stored_str_t, string_view_type>) {
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
        [[nodiscard]] constexpr uri_string resolve(const uri_string& relative_uri) const noexcept {
            // Resolve the reference by following the algorithm
            // from section 5.2.2 in
            // RFC 3986 (https://tools.ietf.org/html/rfc3986).
            // todo: the names of these functions may not work
            uri_string target(this->get_allocator());
            if (relative_uri.has_scheme()) {
                target = relative_uri;
                target.normalize();
            } else {
                target.scheme(scheme());
                target.fragment(relative_uri.fragment());
                if (relative_uri.has_host()) {
                    target.host(relative_uri.host_raw());
                    target.port(relative_uri.port());
                    target.user_info(relative_uri.user_info_raw());
                    target.path(relative_uri.path_raw());
                    target.queries(relative_uri.queries_raw());
                    target.normalize();
                } else {
                    target.host(host_raw());
                    target.user_info(user_info_raw());
                    target.port(port());
                    if (!relative_uri.has_path()) {
                        target.path(path_raw());
                        if (relative_uri.has_queries()) {
                            target.queries(relative_uri.queries_raw());
                        } else {
                            target.queries(queries_raw());
                        }
                    } else {
                        target.queries(relative_uri.queries_raw());
                        // RFC describes this as:
                        // "if (R.path starts-with "/") then"
                        if (relative_uri.is_path_absolute()) {
                            target.path(relative_uri.path_raw());
                            target.normalize();
                        } else {
                            // RFC describes this as:
                            // "T.path = merge(Base.path, R.path);"
                            target.path(path_raw());
                            auto target_path       = target.slugs();
                            auto relative_uri_path = relative_uri.slugs();
                            if (target_path.size() > 1) {
                                target_path.pop_back();
                            }
                            stl::copy(relative_uri_path.cbegin(),
                                      relative_uri_path.cend(),
                                      stl::back_inserter(target_path));
                            target.path(target_path);
                            target.normalize();
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
        [[nodiscard]] constexpr bool is_urn() const noexcept {
            return ascii::iequals<ascii::char_case_side::second_lowered>(scheme(), "urn") &&
                   authority_start == data.size();
        }

        /**
         * Check if the specified URI is in fact a URL
         */
        [[nodiscard]] constexpr bool is_url() const noexcept {
            return has_host();
        }

        /**
         * Check if the specified string is a valid URI or not
         */
        [[nodiscard]] constexpr bool is_valid() const noexcept {
            parse_all();
            return errors.is_success();
        }

        [[nodiscard]] constexpr bool has_valid_scheme() const noexcept {
            return !scheme().empty() || errors.scheme;
        }

        [[nodiscard]] constexpr bool has_valid_host() const noexcept {
            return !host_raw().empty() || !errors.host;
        }

        [[nodiscard]] constexpr bool has_valid_path() const noexcept {
            return !path_raw().empty() || !errors.path;
        }

        [[nodiscard]] constexpr bool has_valid_port() const noexcept {
            return !port().empty() || (!errors.port && port_uint16());
        }

        [[nodiscard]] constexpr bool has_valid_queries() const noexcept {
            // todo: evaluate queries as well, query parser doesn't evaluate it
            return !queries_raw().empty() || !errors.queries;
        }

        [[nodiscard]] constexpr bool has_valid_fragment() const noexcept {
            // todo: evaluate fragment as well, fragment parser doesn't evaluate it
            return !fragment().empty() || !errors.fragment;
        }

        [[nodiscard]] constexpr bool has_valid_user_info() const noexcept {
            // todo: evaluate user_info as well, user_info parser doesn't evaluate it
            return !user_info_raw().empty() || !errors.user_info;
        }

        // Get an error string; this should only be used for debugging
        [[nodiscard]] constexpr string_type error_string() const noexcept {
            string_type                               err_str{this->get_allocator()};
            constexpr stl::array<string_view_type, 2> error_indicator{" ", " !"};
            err_str += error_indicator[has_valid_scheme()];
            err_str += "scheme";
            err_str += error_indicator[has_valid_user_info()];
            err_str += "user-info";
            err_str += error_indicator[has_valid_host()];
            err_str += "host";
            err_str += error_indicator[has_valid_port()];
            err_str += "port";
            err_str += error_indicator[has_valid_path()];
            err_str += "path";
            err_str += error_indicator[has_valid_queries()];
            err_str += "queries";
            err_str += error_indicator[has_valid_fragment()];
            err_str += "fragment";
            return err_str;
        }
    };

    template <istl::CharType CharT = char>
    uri_string(CharT const*) -> uri_string<stl::basic_string<CharT>, stl::basic_string_view<CharT>>;

    template <typename CharT = char>
    uri_string(stl::basic_string_view<CharT>)
      -> uri_string<stl::basic_string_view<CharT>, stl::basic_string_view<CharT>>;

    template <typename CharT = char>
    uri_string(stl::basic_string<CharT>)
      -> uri_string<stl::basic_string<CharT>, stl::basic_string_view<CharT>>;

    using mutable_uri = uri_string<stl::string, stl::string_view>;
    using uri_view    = uri_string<const stl::string, stl::string_view>;


    template <typename T>
    concept URIString = requires {
        typename stl::remove_cvref_t<T>::specified_string_type;
        typename stl::remove_cvref_t<T>::specified_string_view_type;
        requires stl::same_as<stl::remove_cvref_t<T>,
                              uri_string<typename stl::remove_cvref_t<T>::specified_string_type,
                                         typename stl::remove_cvref_t<T>::specified_string_view_type>>;
    };


    template <typename Str1T, typename StrView1T, typename Str2T, typename StrView2T>
    [[nodiscard]] constexpr bool equal_path(uri_string<Str1T, StrView1T> const& p1,
                                            uri_string<Str2T, StrView2T> const& p2) noexcept {
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
            if (!stl::all_of(it1, _p1.cend(), [](auto const& a) constexpr noexcept {
                    return stl::empty(a);
                }))
                return false;
        }
        if (it1 != _p2.cend()) {
            if (!stl::all_of(it2, _p2.cend(), [](auto const& a) constexpr noexcept {
                    return stl::empty(a);
                }))
                return false;
        }
        return true;
    }

    [[nodiscard]] constexpr auto equal_path(istl::StringViewifiable auto&& _p1,
                                            istl::StringViewifiable auto&& _p2) noexcept {
        const auto p1 = istl::string_viewify(stl::forward<decltype(_p1)>(_p1));
        const auto p2 = istl::string_viewify(stl::forward<decltype(_p2)>(_p2));
        return p1 == p2 || equal_path(uri_string<const stl::string, decltype(p1)>{p1},
                                      uri_string<const stl::string, decltype(p2)>{p2});
    }

    template <typename StrT, typename StrViewT>
    [[nodiscard]] constexpr auto equal_path(uri_string<StrT, StrViewT> const& p1,
                                            istl::StringViewifiable auto&&    p2) noexcept {
        return p1 == p2 || equal_path(p1, uri_string{stl::forward<decltype(p2)>(p2)});
    }

    template <typename StrT, typename StrViewT>
    [[nodiscard]] constexpr auto equal_path(istl::StringViewifiable auto&&    p1,
                                            uri_string<StrT, StrViewT> const& p2) noexcept {
        return p2 == p1 || equal_path(uri_string{stl::forward<decltype(p1)>(p1)}, p2);
    }

} // namespace webpp::uri

#endif // WEBPP_URI_URI_STRING_HPP
