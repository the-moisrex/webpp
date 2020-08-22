// Created by moisrex on 8/19/20.

#ifndef WEBPP_RESPONSE_COOKIES_HPP
#define WEBPP_RESPONSE_COOKIES_HPP

#include "../../std/string.hpp"
#include "./cookie.hpp"

namespace webpp {

    enum class same_site_value { NONE, LAX, STRICT };

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

        template <typename T>
        using allocator_type =
          typename stl::allocator_traits<string_allocator_type>::template rebind_alloc<T>;

        using date_t             = stl::chrono::time_point<stl::chrono::system_clock>;
        using name_t             = string_type;
        using value_t            = string_type;
        using domain_t           = string_type;
        using path_t             = string_type;
        using expires_t          = date_t;
        using optional_expires_t = stl::optional<date_t>;
        using max_age_t          = unsigned long;
        using same_site_t        = same_site_value;
        using secure_t           = bool;
        using host_only_t        = bool;
        using prefix_t           = bool;
        using encrypted_t        = bool;
        using comment_t          = string_type;

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
        max_age_t          _max_age   = 0;
        same_site_t        _same_site = same_site_value::NONE;
        secure_t           _secure    = false;
        host_only_t        _host_only = false;
        encrypted_t        _encrypted = false; // todo: do we need this?
        prefix_t           _prefix    = false;

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

        constexpr response_cookie(istl::ConvertibleToString auto&& name,
                                  istl::ConvertibleToString auto&& value,
                                  string_allocator_type const&     alloc = {}) noexcept
          : _name{istl::to_string(stl::forward<decltype(name)>(name), alloc), alloc},
            _value{istl::to_string(stl::forward<decltype(value)>(value), alloc), alloc},
            _domain{alloc},
            _path{alloc},
            _comment{alloc},
            attrs{alloc} {};


        /**
         * Parse response cookie (A single Set-Cookie header value)
         * There are not many reasons to use this constructor
         * @param source
         */
        explicit response_cookie(istl::ConvertibleToStringView auto&& source,
                                 string_allocator_type const&         alloc = {}) noexcept
          : _name{alloc},
            _value{alloc},
            _domain{alloc},
            _path{alloc},
            _comment{alloc},
            attrs{alloc} {
            auto src = istl::to_string_view(stl::forward<decltype(source)>(source));
            // todo: this doesn't work yet
            // parse_SE_options(src); // parse name, value, and options
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
        trim(_##name);              \
        return *this;               \
    }                               \
                                    \
    auto& ltrim_##name() noexcept { \
        ltrim(_##name);             \
        return *this;               \
    }                               \
                                    \
    auto& rtrim_##name() noexcept { \
        rtrim(_##name);             \
        return *this;               \
    }

        WEBPP_METHOD_TRIM(name)
        WEBPP_METHOD_TRIM(value)
        WEBPP_METHOD_TRIM(domain)
        WEBPP_METHOD_TRIM(path)
        WEBPP_METHOD_TRIM(comment)

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
        WEBPP_METHOD_STRS(comment)
        WEBPP_METHOD_STRS(max_age)
        WEBPP_METHOD_STRS(same_site)
        WEBPP_METHOD_STRS(expires)

#undef WEBPP_METHOD_STRS
#define WEBPP_METHOD_BOOLS(name)         \
    bool name() const noexcept {         \
        return _##name;                  \
    }                                    \
                                         \
    auto& name(bool i_##name) noexcept { \
        _##name = i_##name;              \
        return *this;                    \
    }

        WEBPP_METHOD_BOOLS(secure);
        WEBPP_METHOD_BOOLS(prefix)
        WEBPP_METHOD_BOOLS(encrypted)
        WEBPP_METHOD_BOOLS(host_only)

#undef WEBPP_METHOD_BOOLS

        /**
         * @brief decrypt-able encryption
         */
        value_t encrypted_value() const noexcept {
            // todo implement this
        }

        stl::basic_ostream<char_type>& operator<<(stl::basic_ostream<char_type>& out) const noexcept {
            // todo: use std::format instead of streams

            using namespace stl::chrono;
            if (_prefix) {
                if (_secure)
                    out << "__Secure-";
                else if (_host_only)
                    out << "__Host-";
            }
            if (!_name.empty()) {
                // FIXME: encode/... name and value here. Programmers are dumb!
                out << _name << "=" << _value;

                if (!_comment.empty())
                    out << "; Comment=" << _comment;

                if (!_domain.empty())
                    out << "; Domain=" << _domain;

                if (!_path.empty())
                    out << "; Path=" << _path;

                if (_expires) {
                    stl::time_t expires_c  = system_clock::to_time_t(*_expires);
                    stl::tm     expires_tm = *stl::localtime(&expires_c);
                    char        buff[30];
                    // FIXME: check time zone and see if it's ok
                    //            setlocale(LC_ALL, "en_US.UTF-8");
                    if (strftime(buff, sizeof buff, "%a, %d %b %Y %H:%M:%S GMT", &expires_tm))
                        out << "; Expires=" << buff;
                }

                if (_secure)
                    out << "; Secure";

                if (_host_only)
                    out << "; HttpOnly";

                if (_max_age)
                    out << "; Max-Age=" << _max_age;

                if (_same_site != same_site_value::NONE)
                    out << "; SameSite=" << (_same_site == same_site_value::STRICT ? "Strict" : "Lax");

                // TODO: encode value and check the key here:
                if (!attrs.empty())
                    for (auto const& attr : attrs)
                        out << "; " << attr.first << "=" << attr.second;
            }
            return out;
        }

        //        template <istl::StringView StrViewType>
        //        bool operator==(request_cookie<StrViewType> const& c) const noexcept {
        //            return super::name == c.name && super::value == c.value;
        //        }

        bool operator==(response_cookie const& c) const noexcept {
            return _name == c._name && _value == c._value && _prefix == c._prefix &&
                   _encrypted == c._encrypted && _secure == c._secure && _host_only == c._host_only &&
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

        [[nodiscard]] string_type render() const noexcept {
            // todo: don't use streams here
            stl::basic_ostringstream<char_type> os;
                                                operator<<(os);
            return os.str();
        }

        /**
         * @brief this method will return true if the specified basic_cookie and
         * this basic_cookie have the same  name, domain, and path. so they will
         * replace each other if they put in the same basic_cookie jar
         * @param c
         * @return true if they have the same name, domain, and path
         */
        [[nodiscard]] bool same_as(response_cookie const& c) const noexcept {
            return trim_copy(_name, _name.get_allocator()) == trim_copy(c._name, c._name.get_allocator()) &&
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
            swap(first._host_only, second._host_only);
            swap(first._expires, second._expires);
            swap(first._encrypted, second._encrypted);
            swap(first._prefix, second._prefix);
            swap(first._same_site, second._same_site);
        }
    };


} // namespace webpp

#endif // WEBPP_RESPONSE_COOKIES_HPP
