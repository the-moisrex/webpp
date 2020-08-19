// Created by moisrex on 8/19/20.

#ifndef WEBPP_RESPONSE_COOKIES_HPP
#define WEBPP_RESPONSE_COOKIES_HPP

#include "./cookie.hpp"

namespace webpp {

    enum class same_site_value { NONE, LAX, STRICT };

    template <istl::String StringType>
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

        using date_t            = stl::chrono::time_point<stl::chrono::system_clock>;
        using name_t            = string_type;
        using value_t           = string_type;
        using domain_t          = string_type;
        using path_t            = string_type;
        using expires_t         = date_t;
        using optionalexpires_t = stl::optional<date_t>;
        using max_age_t         = unsigned long;
        using same_site_t       = same_site_value;
        using secure_t          = bool;
        using host_only_t       = bool;
        using prefix_t          = bool;
        using encrypted_t       = bool;
        using comment_t         = string_type;

        using attrs_t =
          stl::unordered_map<string_type, string_type, stl::hash<string_type>, stl::equal_to<string_type>,
                             allocator_type<stl::pair<const string_type, string_type>>>;


        name_t            name;
        value_t           value;
        domain_t          domain;
        path_t            path;
        optionalexpires_t expires;
        comment_t         comment;
        max_age_t         max_age   = 0;
        same_site_t       same_site = same_site_value::NONE;
        secure_t          secure    = false;
        host_only_t       host_only = false;
        encrypted_t       encrypted = false; // todo: do we need this?
        prefix_t          prefix    = false;

        // todo: encapsulate this
        attrs_t attrs;

        constexpr response_cookie(string_allocator_type const& alloc = {}) noexcept
          : name{alloc},
            value{alloc},
            domain{alloc},
            path{alloc},
            comment{alloc},
            attrs{alloc} {};


        /**
         * Parse response cookie (A single Set-Cookie header value)
         * There are not many reasons to use this constructor
         * @param source
         */
        explicit response_cookie(istl::ConvertibleToStringView auto&& source,
                                 string_allocator_type const&         alloc = {}) noexcept
          : name{alloc},
            value{alloc},
            domain{alloc},
            path{alloc},
            comment{alloc},
            attrs{alloc} {
            auto src = istl::to_string_view(stl::forward<decltype(source)>(source));
            // todo: this doesn't work yet
            parse_SE_options(src); // parse name, value, and options
        }


        auto& remove(bool i_remove = true) noexcept {
            if (i_remove) {
                // set the expire date 10 year before now:
                expires = (stl::chrono::system_clock::now() -
                           stl::chrono::duration<int, stl::ratio<60 * 60 * 24 * 365>>(10));
            } else if (is_removed()) {
                // set the expire date 1 year from now:
                expires = (stl::chrono::system_clock::now() +
                           stl::chrono::duration<int, stl::ratio<60 * 60 * 24 * 365>>(1));
            }
            // remove max-age if it exists because we're going with expires
            max_age = 0;
            return *this;
        }

        [[nodiscard]] bool is_removed() const noexcept {
            return *expires < stl::chrono::system_clock::now();
        }

        /**
         * @brief sets expiration time relative to now.
         */
        template <typename D, typename T>
        inline auto& expires_in(stl::chrono::duration<D, T> const& i_dur) noexcept {
            expires = stl::chrono::system_clock::now() + i_dur;
            return *this;
        }

        /**
         * @brief decrypt-able encryption
         */
        value_t encrypted_value() const noexcept {
            // todo implement this
        }

        stl::basic_ostream<char_type>& operator<<(stl::basic_ostream<char_type>& out) const noexcept {
            // todo: use std::format instead of streams

            using namespace stl::chrono;
            if (prefix) {
                if (secure)
                    out << "__Secure-";
                else if (host_only)
                    out << "__Host-";
            }
            if (!name.empty()) {
                // FIXME: encode/... name and value here. Programmers are dumb!
                out << name << "=" << value;

                if (!comment.empty())
                    out << "; Comment=" << comment;

                if (!domain.empty())
                    out << "; Domain=" << domain;

                if (!path.empty())
                    out << "; Path=" << path;

                if (expires) {
                    stl::time_t expires_c  = system_clock::to_time_t(*expires);
                    stl::tm     expires_tm = *stl::localtime(&expires_c);
                    char        buff[30];
                    // FIXME: check time zone and see if it's ok
                    //            setlocale(LC_ALL, "en_US.UTF-8");
                    if (strftime(buff, sizeof buff, "%a, %d %b %Y %H:%M:%S GMT", &expires_tm))
                        out << "; Expires=" << buff;
                }

                if (secure)
                    out << "; Secure";

                if (host_only)
                    out << "; HttpOnly";

                if (max_age)
                    out << "; Max-Age=" << max_age;

                if (same_site != same_site_value::NONE)
                    out << "; SameSite=" << (same_site == same_site_value::STRICT ? "Strict" : "Lax");

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
            return name == c.name && value == c.value && prefix == c.prefix && encrypted == c.encrypted &&
                   secure == c.secure && host_only == c.host_only && same_site == c.same_site &&
                   comment == c.comment && expires == c.expires && path == c.path && domain == c.domain &&
                   attrs == c.attrs;
        }

        bool operator<(response_cookie const& c) const noexcept {
            return expires < c.expires;
        }

        bool operator>(response_cookie const& c) const noexcept {
            return expires > c.expires;
        }

        bool operator<=(response_cookie const& c) const noexcept {
            return expires <= c.expires;
        }

        bool operator>=(response_cookie const& c) const noexcept {
            return expires >= c.expires;
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
            return trim_copy(name, name.get_allocator()) == trim_copy(c.name, c.get_allocator()) &&
                   path == c.path && c.domain == domain;
        }

        friend inline void swap(response_cookie& first, response_cookie& second) noexcept {
            using stl::swap;
            swap(first.name, second.name);
            swap(first.value, second.value);
            swap(first.comment, second.comment);
            swap(first.domain, second.domain);
            swap(first.path, second.path);
            swap(first.max_age, second.max_age);
            swap(first.secure, second.secure);
            swap(first.host_only, second.host_only);
            swap(first.expires, second.expires);
            swap(first.encrypted, second.encrypted);
            swap(first.prefix, second.prefix);
            swap(first.same_site, second.same_site);
        }
    };


} // namespace webpp

#endif // WEBPP_RESPONSE_COOKIES_HPP
