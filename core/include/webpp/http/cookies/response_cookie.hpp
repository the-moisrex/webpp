// Created by moisrex on 8/19/20.

#ifndef WEBPP_RESPONSE_COOKIES_HPP
#define WEBPP_RESPONSE_COOKIES_HPP

#include "../../std/string.hpp"
#include "../../utils/casts.hpp"
#include "./cookie.hpp"

namespace webpp {

    enum struct same_site_value : stl::uint_fast8_t { not_specified, none, lax, strict };


    template <istl::String StringType = stl::string>
    struct response_cookie {
      private:
        //        void parse_SE_options(istl::StringView auto& str) noexcept {
        //            details::parse_SEvalue(str, name, value);
        //            // todo
        //        }

      public:
        using string_type           = stl::remove_cvref_t<StringType>;
        using char_type             = typename string_type::value_type;
        using string_allocator_type = typename string_type::allocator_type;

        static constexpr auto illegal_chars = charset("()[]/|\\',;");


        template <typename T>
        using allocator_type =
          typename stl::allocator_traits<string_allocator_type>::template rebind_alloc<T>;

        using date_t             = stl::chrono::time_point<stl::chrono::system_clock>;
        using name_t             = string_type;
        using value_t            = string_type;
        using domain_t           = string_type;
        using path_t             = string_type;
        using priority_t         = string_type;
        using expires_t          = date_t;
        using optional_expires_t = stl::optional<date_t>;
        using max_age_t          = unsigned long;
        using same_site_t        = same_site_value;
        using secure_t           = bool;
        using http_only_t        = bool;
        using prefix_t           = bool;
        using encrypted_t        = bool;
        using comment_t          = string_type;
        using version_t          = cookie_version;

        using attrs_t =
          stl::unordered_map<string_type, string_type, stl::hash<string_type>, stl::equal_to<string_type>,
                             allocator_type<stl::pair<const string_type, string_type>>>;


      private:
        name_t             _name;
        value_t            _value;
        domain_t           _domain;
        path_t             _path;
        optional_expires_t _expires;
        comment_t          _comment;
        priority_t         _priority;
        max_age_t          _max_age   = 0;
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
            _comment{alloc},
            attrs{alloc} {};

        constexpr response_cookie(istl::Stringifiable auto&& name,
                                  istl::Stringifiable auto&& value,
                                  string_allocator_type const&     alloc = {}) noexcept
          : _name{istl::stringify(stl::forward<decltype(name)>(name), alloc), alloc},
            _value{istl::stringify(stl::forward<decltype(value)>(value), alloc), alloc},
            _domain{alloc},
            _path{alloc},
            _comment{alloc},
            attrs{alloc} {};


        /**
         * Parse response cookie (A single Set-Cookie header value)
         * There are not many reasons to use this constructor
         * @param source
         */
        explicit response_cookie(istl::StringViewifiable auto&& source,
                                 string_allocator_type const&         alloc = {}) noexcept
          : _name{alloc},
            _value{alloc},
            _domain{alloc},
            _path{alloc},
            _comment{alloc},
            attrs{alloc} {
            auto src = istl::string_viewify(stl::forward<decltype(source)>(source));
            // todo: this doesn't work yet
            // parse_SE_options(src); // parse name, value, and options
        }


        auto const& get_allocator() const noexcept {
            return name().get_allocator(); // what? you've got a better solution? :)
        }

        auto& remove(bool i_remove = true) noexcept {
            if (i_remove) {
                // set the expire date 10 year before now:
                _expires = (stl::chrono::system_clock::now() -
                            stl::chrono::duration<int, stl::ratio<60 * 60 * 24 * 365>>(10));
            } else if (is_removed()) {
                // set the expire date 1 year from now:
                _expires = (stl::chrono::system_clock::now() +
                            stl::chrono::duration<int, stl::ratio<60 * 60 * 24 * 365>>(1));
            }
            // remove max-age if it exists because we're going with expires
            _max_age = 0;
            return *this;
        }

        [[nodiscard]] bool is_removed() const noexcept {
            return *_expires < stl::chrono::system_clock::now();
        }

        /**
         * @brief sets expiration time relative to now.
         */
        template <typename D, typename T>
        inline auto& expires_in(stl::chrono::duration<D, T> const& i_dur) noexcept {
            _expires = stl::chrono::system_clock::now() + i_dur;
            return *this;
        }

#define WEBPP_METHOD_TRIM(name)     \
    auto& trim_##name() noexcept {  \
        ascii::trim(_##name);       \
        return *this;               \
    }                               \
                                    \
    auto& ltrim_##name() noexcept { \
        ascii::ltrim(_##name);      \
        return *this;               \
    }                               \
                                    \
    auto& rtrim_##name() noexcept { \
        ascii::rtrim(_##name);      \
        return *this;               \
    }

        WEBPP_METHOD_TRIM(name)
        WEBPP_METHOD_TRIM(value)
        WEBPP_METHOD_TRIM(domain)
        WEBPP_METHOD_TRIM(path)
        WEBPP_METHOD_TRIM(comment)
        WEBPP_METHOD_TRIM(priority)

#undef WEBPP_METHOD_TRIM
#define WEBPP_METHOD_STRS(name)                     \
    auto const& name() const noexcept {             \
        return _##name;                             \
    }                                               \
                                                    \
    auto& name(name##_t&& i_##name) noexcept {      \
        _##name = stl::move(i_##name);              \
        return *this;                               \
    }                                               \
                                                    \
    auto& name(name##_t const& i_##name) noexcept { \
        _##name = i_##name;                         \
        return *this;                               \
    }

        WEBPP_METHOD_STRS(name)
        WEBPP_METHOD_STRS(value)
        WEBPP_METHOD_STRS(domain)
        WEBPP_METHOD_STRS(path)
        WEBPP_METHOD_STRS(priority)
        WEBPP_METHOD_STRS(comment)
        WEBPP_METHOD_STRS(max_age)
        WEBPP_METHOD_STRS(same_site)
        WEBPP_METHOD_STRS(expires)

#undef WEBPP_METHOD_STRS
#define WEBPP_METHOD_OTHERS(type, name)  \
    type name() const noexcept {         \
        return _##name;                  \
    }                                    \
                                         \
    auto& name(type i_##name) noexcept { \
        _##name = i_##name;              \
        return *this;                    \
    }

        WEBPP_METHOD_OTHERS(secure_t, secure);
        WEBPP_METHOD_OTHERS(prefix_t, prefix)
        WEBPP_METHOD_OTHERS(encrypted_t, encrypted)
        WEBPP_METHOD_OTHERS(http_only_t, http_only)
        WEBPP_METHOD_OTHERS(version_t, version)

#undef WEBPP_METHOD_OTHERS

        /**
         * @brief decrypt-able encryption
         */
        value_t encrypted_value() const noexcept {
            // todo implement this
        }


        /*
         * Escapes the given string by replacing all
         * non-alphanumeric characters with escape
         * sequences in the form %xx, where xx is the
         * hexadecimal character code.
         *
         * The following characters will be replaced
         * with escape sequences:
         *   - percent sign %
         *   - less-than and greater-than < and >
         *   - curly brackets { and }
         *   - square brackets [ and ]
         *   - parenthesis ( and )
         *   - solidus /
         *   - vertical line |
         *   - reverse solidus (backslash /)
         *   - quotation mark "
         *   - apostrophe '
         *   - circumflex accent ^
         *   - grave accent `
         *   - comma and semicolon , and ;
         *   - whitespace and control characters
         */
        static constexpr std::string escape(const std::string& str) noexcept;

        /*
         * Unescapes the given string by replacing all
         * escape sequences in the form %xx with the
         * respective characters.
         */
        static std::string unescape(const std::string& str);

        string_type to_string() const {
            string_type res{get_allocator()};
            to_string(res);
            return res;
        }

        void to_string(string_type &result) const {
            using namespace stl::chrono;

            result.reserve(result.size() + 256);
            result.append(_name);
            result.append("=");
            if (_version == cookie_version::version_0) {
                // Netscape cookie
                result.append(_value);
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
                if (_max_age != -1) {
                    stl::time_t expires_c  = system_clock::to_time_t(*_expires);
                    result.append("; expires=");
                    stl::format_to(stl::back_inserter(result),
                                   FMT_COMPILE("{:%a, %d %b %Y %H:%M:%S} GMT"),
                                   istl::safe_localtime(expires_c));
                }
                switch (_same_site) {
                    case same_site_value::none: result.append("; SameSite=None"); break;
                    case same_site_value::lax: result.append("; SameSite=Lax"); break;
                    case same_site_value::strict: result.append("; SameSite=Strict"); break;
                    case same_site_value::not_specified: break;
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
                result.append(_value);
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
                    stl::time_t expires_c  = system_clock::to_time_t(*_expires);
                    result.append("; expires=");
                    stl::format_to(stl::back_inserter(result),
                                   FMT_COMPILE("{:%a, %d %b %Y %H:%M:%S} GMT"),
                                   istl::safe_localtime(expires_c));
                }

                switch (_same_site) {
                    case same_site_value::none: result.append("; SameSite=None"); break;
                    case same_site_value::lax: result.append("; SameSite=Lax"); break;
                    case same_site_value::strict: result.append("; SameSite=Strict"); break;
                    case same_site_value::not_specified: break;
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

        bool operator==(response_cookie const& c) const noexcept {
            return _name == c._name && _value == c._value && _prefix == c._prefix &&
                   _priority == c._priority && _version == c._version &&
                   _encrypted == c._encrypted && _secure == c._secure && _http_only == c._http_only &&
                   _same_site == c._same_site && _comment == c._comment && _expires == c._expires &&
                   _path == c._path && _domain == c._domain && attrs == c.attrs;
        }

        bool operator<(response_cookie const& c) const noexcept {
            return _expires < c._expires;
        }

        bool operator>(response_cookie const& c) const noexcept {
            return _expires > c._expires;
        }

        bool operator<=(response_cookie const& c) const noexcept {
            return _expires <= c._expires;
        }

        bool operator>=(response_cookie const& c) const noexcept {
            return _expires >= c._expires;
        }

        /**
         * @brief this method will return true if the specified basic_cookie and
         * this basic_cookie have the same  name, domain, and path. so they will
         * replace each other if they put in the same basic_cookie jar
         * @param c
         * @return true if they have the same name, domain, and path
         */
        [[nodiscard]] bool same_as(response_cookie const& c) const noexcept {
            return ascii::trim_copy(_name, _name.get_allocator()) ==
                     ascii::trim_copy(c._name, c._name.get_allocator()) &&
                   _path == c._path && c._domain == _domain;
        }

        friend inline void swap(response_cookie& first, response_cookie& second) noexcept {
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


} // namespace webpp

#endif // WEBPP_RESPONSE_COOKIES_HPP
