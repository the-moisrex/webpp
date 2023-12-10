// Created by moisrex on 8/19/20.

#ifndef WEBPP_RESPONSE_COOKIES_HPP
#define WEBPP_RESPONSE_COOKIES_HPP

#include "../../convert/casts.hpp"
#include "../../memory/allocators.hpp"
#include "../../std/chrono.hpp"
#include "../../std/format.hpp"
#include "../../std/string.hpp"
#include "../../strings/iequals.hpp"
#include "../../strings/parser_utils.hpp"
#include "../../strings/string_tokenizer.hpp"
#include "../codec/tokens.hpp"
#include "cookie.hpp"

#include <iomanip>

namespace webpp::http {

    enum struct same_site_value : stl::uint_fast8_t {
        not_specified,
        none,
        lax,
        strict
    };

    template <istl::String StringType = stl::string>
    struct response_cookie {
        using string_type           = StringType;
        using char_type             = istl::char_type_of_t<string_type>;
        using string_allocator_type = typename string_type::allocator_type;

        static constexpr auto illegal_chars = charset("()[]/|\\',;");

#ifdef WEBPP_UTC_CLOCK_SUPPORTED
        using clock_type = stl::chrono::utc_clock;
#else
        using clock_type = stl::chrono::system_clock;
#endif
        using date_t             = stl::chrono::time_point<clock_type>;
        using name_t             = string_type;
        using value_t            = string_type;
        using domain_t           = string_type;
        using path_t             = string_type;
        using priority_t         = string_type;
        using expires_t          = date_t;
        using optional_expires_t = stl::optional<date_t>;
        using max_age_t          = long; // max age can be negative
        using same_site_t        = same_site_value;
        using secure_t           = bool;
        using http_only_t        = bool;
        using prefix_t           = bool;
        using encrypted_t        = bool;
        using comment_t          = string_type;
        using version_t          = cookie_version;

        using attrs_t = stl::unordered_map<
          string_type,
          string_type,
          stl::hash<string_type>,
          stl::equal_to<string_type>,
          rebind_allocator<string_allocator_type, stl::pair<string_type const, string_type>>>;


        static constexpr max_age_t MAX_AGE_EXISTENCE_VALUE = stl::numeric_limits<max_age_t>::min();
        static constexpr auto      date_format             = "{:%a, %d %b %Y %T} GMT";

      private:
        name_t             _name;
        value_t            _value;
        domain_t           _domain;
        path_t             _path;
        optional_expires_t _expires;
        comment_t          _comment;
        priority_t         _priority;
        max_age_t          _max_age   = MAX_AGE_EXISTENCE_VALUE;
        same_site_t        _same_site = same_site_value::not_specified;
        secure_t           _secure    = false;
        http_only_t        _http_only = false;
        encrypted_t        _encrypted = false; // todo: do we need this?
        prefix_t           _prefix    = false;
        cookie_version     _version   = cookie_version::version_1;

      public:
        // todo: encapsulate this
        attrs_t attrs;

        constexpr response_cookie(string_allocator_type const& alloc = {}) noexcept
          : _name{alloc},
            _value{alloc},
            _domain{alloc},
            _path{alloc},
            _expires{},
            _comment{alloc},
            attrs{alloc} {}

        constexpr response_cookie(istl::Stringifiable auto&&   name,
                                  istl::Stringifiable auto&&   value,
                                  string_allocator_type const& alloc = {}) noexcept
          : _name{istl::stringify_of<name_t>(stl::forward<decltype(name)>(name), alloc), alloc},
            _value{istl::stringify_of<value_t>(stl::forward<decltype(value)>(value), alloc), alloc},
            _domain{alloc},
            _path{alloc},
            _expires{},
            _comment{alloc},
            attrs{alloc} {}

        /**
         * Parse response cookie (A single Set-Cookie header value)
         * There are not many reasons to use this constructor
         * @param source
         */
        template <istl::StringViewifiable StrVT>
            requires(!istl::cvref_as<StrVT, response_cookie>)
        constexpr explicit response_cookie(StrVT&& source, string_allocator_type const& alloc = {}) noexcept
          : _name{alloc},
            _value{alloc},
            _domain{alloc},
            _path{alloc},
            _expires{},
            _comment{alloc},
            attrs{alloc} {
            auto src = istl::string_viewify(stl::forward<decltype(source)>(source));
            parse_set_cookie(src); // parse name, value, and options
            // todo: deal with the error
        }

      private:
        static constexpr auto semicolon = charset{OWS, charset{';'}};
        static constexpr auto eq_char   = charset{OWS, charset('=', ' ')};

      public:
        /**
         * Parse a string response
         */
        constexpr bool parse_set_cookie(istl::StringView auto src) noexcept {
            using string_view_type = stl::remove_cvref_t<decltype(src)>;

            bool is_valid = true;
            details::parse_SE_value(src, _name, _value, is_valid);
            if (!is_valid) {
                return false;
            }
            string_view_type                   key;
            string_view_type                   value;
            string_tokenizer<string_view_type> tokenizer{src};
            tokenizer.skip(semicolon);
            while (!tokenizer.at_end()) {
                tokenizer.skip(semicolon);
                tokenizer.expect(details::VALID_COOKIE_NAME, key, is_valid, false);
                if (tokenizer.expect(eq_char)) {
                    tokenizer.next(charset{';'}, value, is_valid, false);
                }
                if (key.empty()) {
                    // I'm not putting this after finding the key part because we need to get rid of the value
                    // for the next iteration
                    is_valid = false;
                    continue;
                }
                switch (ascii::to_lower_copy(key[0])) {
                    case 'e':
                        if (ascii::iequals<ascii::char_case_side::second_lowered>(key, "expires")) {
                            using char_traits_type = typename value_t::traits_type;
                            using string_char_type = typename value_t::value_type;
                            stl::
                              basic_istringstream<string_char_type, char_traits_type, string_allocator_type>
                                inp_stream{istl::stringify_of<string_type>(value, this->get_allocator())};
                            expires_t new_expires;
#ifdef WEBPP_UTC_CLOCK_SUPPORTED
                            // todo: use std::chrono::parse, maybe? when it gets implemented
                            stl::chrono::from_stream(is, "%a, %d %b %Y %H:%M:%S GMT", new_expires);
#else
                            tm tmp_time{};
                            inp_stream >> stl::get_time(&tmp_time, "%a, %d %b %Y %H:%M:%S GMT");
                            new_expires = clock_type::from_time_t(stl::mktime(&tmp_time));
#endif
                            _expires = new_expires;
                        } else {
                            attrs.emplace(key, value);
                        }
                        break;
                    case 'c':
                        if (ascii::iequals<ascii::char_case_side::second_lowered>(key, "comment")) {
                            ascii::rtrim(value);
                            _comment = value;
                        } else {
                            attrs.emplace(key, value);
                        }
                        break;
                    case 'd':
                        if (ascii::iequals<ascii::char_case_side::second_lowered>(key, "domain")) {
                            ascii::rtrim(value);
                            _domain = value;
                        } else {
                            attrs.emplace(key, value);
                        }
                        break;
                    case 'p':
                        if (ascii::iequals<ascii::char_case_side::second_lowered>(key, "path")) {
                            ascii::rtrim(value);
                            _path = value; // todo: should we store escaped or unescaped?
                        } else if (ascii::iequals<ascii::char_case_side::second_lowered>(key, "priority")) {
                            ascii::rtrim(value);
                            _priority = value;
                        } else {
                            attrs.emplace(key, value);
                        }
                        break;
                    case 's':
                        if (ascii::iequals<ascii::char_case_side::second_lowered>(key, "secure")) {
                            _secure = true;
                        } else if (ascii::iequals<ascii::char_case_side::second_lowered>(key, "samesite")) {
                            ascii::rtrim(value);
                            if (ascii::iequals<ascii::char_case_side::second_lowered>(value, "strict")) {
                                _same_site = same_site_value::strict;
                            } else if (ascii::iequals<ascii::char_case_side::second_lowered>(value, "lax")) {
                                _same_site = same_site_value::lax;
                            } else if (ascii::iequals<ascii::char_case_side::second_lowered>(value, "none")) {
                                _same_site = same_site_value::none;
                            } else {
                                attrs.emplace(key, value);
                                is_valid = false;
                            }
                        } else {
                            attrs.emplace(key, value);
                        }
                        break;
                    case 'h':
                        if (ascii::iequals<ascii::char_case_side::second_lowered>(key, "httponly")) {
                            _http_only = true;
                        } else {
                            attrs.emplace(key, value);
                        }
                        break;
                    case 'm':
                        if (ascii::iequals<ascii::char_case_side::second_lowered>(key, "max-age")) {
                            max_age(to<max_age_t>(value));
                        } else {
                            attrs.emplace(key, value);
                        }
                        break;
                    case 'v':
                        if (ascii::iequals<ascii::char_case_side::second_lowered>(key, "version")) {
                            ascii::rtrim(value);
                            if (value == "0") {
                                _version = version_t::version_0;
                            } else if (value == "1") {
                                _version = version_t::version_1;
                            } else {
                                attrs.emplace(key, value);
                                is_valid = false;
                            }
                        } else {
                            attrs.emplace(key, value);
                        }
                        break;
                    default: attrs.emplace(key, value);
                }
            }
            return is_valid;
        }

        constexpr auto get_allocator() const noexcept {
            return name().get_allocator(); // what? you've got a better solution? :)
        }

        constexpr auto& remove(bool i_remove = true) noexcept {
            // NOLINTBEGIN(*-magic-numbers)

            // difference between year_dur and std::chrono::year is the integer type
            using year_dur = stl::chrono::duration<int, std::ratio<31'556'952>>;
            if (i_remove) {
                // set the 'expire' date 10 year before now:
                _expires = (clock_type::now() - year_dur(10));
            } else if (is_removed()) {
                // set the 'expire' date 1 year from now:
                _expires = (clock_type::now() + year_dur(1));
            }
            // remove max-age if it exists because we're going with expires
            _max_age = 0;
            return *this;
            // NOLINTEND(*-magic-numbers)
        }

        [[nodiscard]] constexpr bool is_removed() const noexcept {
            // todo: clang-tidy says "Use nullptr"; why?
            return has_max_age() ? max_age() <= 0 : (_expires && _expires.value() < clock_type::now());
        }

        /**
         * @brief sets expiration time relative to now.
         */
        template <typename D, typename T>
        inline constexpr auto& expires_in(stl::chrono::duration<D, T> const& i_dur) noexcept {
            _expires = clock_type::now() + i_dur;
            return *this;
        }

#define WEBPP_METHOD_TRIM(name)               \
    constexpr auto& trim_##name() noexcept {  \
        ascii::trim(_##name);                 \
        return *this;                         \
    }                                         \
                                              \
    constexpr auto& ltrim_##name() noexcept { \
        ascii::ltrim(_##name);                \
        return *this;                         \
    }                                         \
                                              \
    constexpr auto& rtrim_##name() noexcept { \
        ascii::rtrim(_##name);                \
        return *this;                         \
    }

        WEBPP_METHOD_TRIM(name)
        WEBPP_METHOD_TRIM(value)
        WEBPP_METHOD_TRIM(domain)
        WEBPP_METHOD_TRIM(path)
        WEBPP_METHOD_TRIM(comment)
        WEBPP_METHOD_TRIM(priority)

#undef WEBPP_METHOD_TRIM
#define WEBPP_METHOD_STRS(name)                               \
    constexpr auto const& name() const noexcept {             \
        return _##name;                                       \
    }                                                         \
                                                              \
    constexpr auto& name(name##_t&& i_##name) noexcept {      \
        _##name = stl::move(i_##name);                        \
        return *this;                                         \
    }                                                         \
                                                              \
    constexpr auto& name(name##_t const& i_##name) noexcept { \
        _##name = i_##name;                                   \
        return *this;                                         \
    }

        WEBPP_METHOD_STRS(name)
        WEBPP_METHOD_STRS(value)
        WEBPP_METHOD_STRS(domain)
        WEBPP_METHOD_STRS(path)
        WEBPP_METHOD_STRS(priority)
        WEBPP_METHOD_STRS(comment)
        WEBPP_METHOD_STRS(same_site)
        WEBPP_METHOD_STRS(expires)

#undef WEBPP_METHOD_STRS
#define WEBPP_METHOD_OTHERS(type, name)                  \
    [[nodiscard]] constexpr type name() const noexcept { \
        return _##name;                                  \
    }                                                    \
                                                         \
    constexpr auto& name(type i_##name) noexcept {       \
        _##name = i_##name;                              \
        return *this;                                    \
    }

        WEBPP_METHOD_OTHERS(secure_t, secure)
        WEBPP_METHOD_OTHERS(prefix_t, prefix)
        WEBPP_METHOD_OTHERS(encrypted_t, encrypted)
        WEBPP_METHOD_OTHERS(http_only_t, http_only)
        WEBPP_METHOD_OTHERS(version_t, version)

#undef WEBPP_METHOD_OTHERS

        /**
         * Check if the specified domain is a valid domain for this cookie
         */
        [[nodiscard]] constexpr bool in_domain(istl::StringViewifiable auto&& the_domain) const noexcept {
            // todo
            return false;
        }

        /**
         * Check if the specified path is valid for this cookie
         */
        [[nodiscard]] constexpr bool in_path(istl::StringViewifiable auto&& the_path) const noexcept {
            // todo
            return false;
        }

        [[nodiscard]] constexpr bool has_expires() const noexcept {
            return static_cast<bool>(_expires); // we can cast optional<...> to bool
        }

        [[nodiscard]] constexpr bool has_max_age() const noexcept {
            return _max_age == MAX_AGE_EXISTENCE_VALUE;
        }

        [[nodiscard]] constexpr max_age_t max_age() const noexcept {
            return _max_age;
        }

        /**
         * If both Expires and Max-Age are set, Max-Age has precedence.
         */
        constexpr void max_age(max_age_t val) noexcept {
            _max_age = val;
            if (has_expires()) {
                _expires = stl::nullopt;
            }
        }

        constexpr void expires_string_to(istl::String auto& result) const {
            if (_expires) {
                // we have to do this because currently STL implementers have not yet specialized
                // std::formatter for utc_clock type
                if constexpr (requires { typename fmt::formatter<date_t, char_type>::type; }) {
                    fmt::format_to(stl::back_inserter(result), date_format, _expires.value());
                } else {
                    fmt::format_to(stl::back_inserter(result),
                                   date_format,
                                   std::chrono::time_point_cast<std::chrono::seconds>(
#ifdef WEBPP_UTC_CLOCK_SUPPORTED
                                     clock_type::to_sys(*_expires)
#else
                                     *_expires
#endif
                                       ));
                }
            }
        }

        template <istl::String StrT = string_type>
        [[nodiscard]] constexpr StrT expires_string(auto&&... args) const {
            StrT result{stl::forward<decltype(args)>(args)...};
            expires_string_to(result);
            return result;
        }

        template <istl::String StrT = string_type>
        [[nodiscard]] constexpr StrT encrypted_value() const noexcept {
            StrT encrypted_value{this->get_allocator()};
            details::encrypt_to(value(), encrypted_value);
            return encrypted_value;
        }

        template <istl::String StrT = string_type>
        [[nodiscard]] constexpr StrT decrypted_value() const noexcept {
            StrT decrypted_value{this->get_allocator()};
            details::decrypt_to(value(), decrypted_value);
            return decrypted_value;
        }

        template <istl::String StrT = string_type>
        [[nodiscard]] constexpr StrT escaped_value() const noexcept {
            StrT encrypted_value{this->get_allocator()};
            details::cookie_value_escape_to(value(), encrypted_value);
            return encrypted_value;
        }

        template <istl::String StrT = string_type>
        [[nodiscard]] constexpr StrT unescaped_value() const noexcept {
            StrT decrypted_value{this->get_allocator()};
            details::cookie_value_unescape_to(value(), decrypted_value); // todo: do something if it fails!
            return decrypted_value;
        }

        constexpr string_type to_string() const {
            string_type res{get_allocator()};
            to_string(res);
            return res;
        }

        constexpr void to_string(string_type& result) const {
            using namespace stl::chrono;

            result.reserve(result.size() + 256); // NOLINT(*-magic-numbers)
            result.append(_name);
            result.append("=");
            if (_version == cookie_version::version_0) {
                // Netscape cookie
                result.append(encrypted() ? encrypted_value() : _value);
                if (!_domain.empty()) {
                    result.append("; domain=");
                    result.append(_domain);
                }
                if (!_path.empty()) {
                    result.append("; path=");
                    result.append(_path);
                }
                if (!_priority.empty()) {
                    result.append("; priority=");
                    result.append(_priority);
                }
                if (_max_age != MAX_AGE_EXISTENCE_VALUE) {
                    result.append("; expires=");
                    expires_string_to(result);
                }
                switch (_same_site) {
                    using enum same_site_value;
                    case none: result.append("; SameSite=None"); break;
                    case lax: result.append("; SameSite=Lax"); break;
                    case strict: result.append("; SameSite=Strict"); break;
                    case not_specified: break;
                }
                if (_secure) {
                    result.append("; secure");
                }
                if (_http_only) {
                    result.append("; HttpOnly");
                }
            } else {
                // RFC 2109 cookie
                result.append("\"");
                result.append(encrypted() ? encrypted_value() : _value);
                result.append("\"");
                if (!_comment.empty()) {
                    result.append("; comment=\"");
                    result.append(_comment);
                    result.append("\"");
                }
                if (!_domain.empty()) {
                    result.append("; domain=\"");
                    result.append(_domain);
                    result.append("\"");
                }
                if (!_path.empty()) {
                    result.append("; path=\"");
                    result.append(_path);
                    result.append("\"");
                }
                if (!_priority.empty()) {
                    result.append("; priority=\"");
                    result.append(_priority);
                    result.append("\"");
                }

                if (_max_age != -1) {
                    result.append("; max-age=\"");
                    append_to(result, _max_age);
                    result.append("\"");
                } else if (_expires) {
                    result.append("; expires=");
                    expires_string_to(result);
                }

                switch (_same_site) {
                    using enum same_site_value;
                    case none: result.append("; SameSite=None"); break;
                    case lax: result.append("; SameSite=Lax"); break;
                    case strict: result.append("; SameSite=Strict"); break;
                    case not_specified: break;
                }
                if (_secure) {
                    result.append("; secure");
                }
                if (_http_only) {
                    result.append("; HttpOnly");
                }
                result.append("; version=\"1\"");
            }


            // TODO: encode value and check the key here:
            if (!attrs.empty()) {
                for (auto const& attr : attrs) {
                    result.append("; ");
                    result.append(attr.first);
                    result.append('=');
                    result.append(attr.second);
                }
            }
        }

        //        template <istl::StringView StrViewType>
        //        bool operator==(request_cookie<StrViewType> const& c) const noexcept {
        //            return super::name == c.name && super::value == c.value;
        //        }

        [[nodiscard]] constexpr bool operator==(response_cookie const& c) const noexcept {
            return _name == c._name && _value == c._value && _prefix == c._prefix &&
                   _priority == c._priority && _version == c._version && _encrypted == c._encrypted &&
                   _secure == c._secure && _http_only == c._http_only && _same_site == c._same_site &&
                   _comment == c._comment && _expires == c._expires && _path == c._path &&
                   _domain == c._domain && attrs == c.attrs;
        }

        [[nodiscard]] constexpr bool operator!=(response_cookie const& c) const noexcept {
            return !operator==(c);
        }

        [[nodiscard]] constexpr bool operator<(response_cookie const& c) const noexcept {
            return _expires < c._expires;
        }

        [[nodiscard]] constexpr bool operator>(response_cookie const& c) const noexcept {
            return _expires > c._expires;
        }

        [[nodiscard]] constexpr bool operator<=(response_cookie const& c) const noexcept {
            return _expires <= c._expires;
        }

        [[nodiscard]] constexpr bool operator>=(response_cookie const& c) const noexcept {
            return _expires >= c._expires;
        }

        /**
         * @brief this method will return true if the specified basic_cookie and
         * this basic_cookie have the same  name, domain, and path. so they will
         * replace each other if they put in the same basic_cookie jar
         * @param c
         * @return true if they have the same name, domain, and path
         */
        [[nodiscard]] constexpr bool same_as(response_cookie const& c) const noexcept {
            return ascii::trim_copy(_name, _name.get_allocator()) ==
                     ascii::trim_copy(c._name, c._name.get_allocator()) &&
                   _path == c._path && c._domain == _domain;
        }

        friend inline constexpr void swap(response_cookie& first, response_cookie& second) noexcept {
            using stl::swap;
            swap(first._name, second._name);
            swap(first._value, second._value);
            swap(first._comment, second._comment);
            swap(first._domain, second._domain);
            swap(first._path, second._path);
            swap(first._max_age, second._max_age);
            swap(first._secure, second._secure);
            swap(first._http_only, second._http_only);
            swap(first._expires, second._expires);
            swap(first._encrypted, second._encrypted);
            swap(first._prefix, second._prefix);
            swap(first._same_site, second._same_site);
            swap(first._priority, second._priority);
            swap(first._version, second._version);
        }
    };


} // namespace webpp::http

#endif // WEBPP_RESPONSE_COOKIES_HPP
