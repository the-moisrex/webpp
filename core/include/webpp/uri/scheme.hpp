// Created by moisrex on 10/30/20.

#ifndef WEBPP_SCHEME_HPP
#define WEBPP_SCHEME_HPP

#include "../std/string_view.hpp"
#include "../strings/charset.hpp"

namespace webpp {

    namespace is {
        /**
         * @brief check if scheme is correct or not
         * @param _scheme
         * @return
         */
        template <Traits TraitsType>
        [[nodiscard]] constexpr bool scheme(typename TraitsType::string_view_type const& _scheme) noexcept {
            return ALPHA<typename TraitsType::char_type>.contains(_scheme);
        }

    } // namespace is


    struct uri_scheme {
        using uri_type = basic_uri;

        uri_type self;

        constexpr uri_scheme(uri_type the_uri) = default;

        auto& operator=(istl::StringViewifiable auto&& new_scheme) noexcept {
            set(stl::forward<decltype(new_scheme)>(new_scheme));
            return *this;
        }

        /**
         * @brief check if the specified uri has a scheme or not
         */
        [[nodiscard]] bool empty() const noexcept {
            self.parse_scheme();
            return self.scheme_end == self.data.size() && self.scheme_end != 0;
        }

        /**
         * @brief scheme
         * @return get scheme
         */
        [[nodiscard]] str_view_t value() const noexcept {
            self.parse_scheme();
            return self.scheme_end == self.data.size() ? str_view_t() : self.substr(0, self.scheme_end);
        }

        /**
         * @brief set scheme
         * @param _scheme
         * @throws logic_error if uri is const
         */
        basic_uri& set(str_view_t m_scheme) {
            if (ascii::ends_with(m_scheme, ':'))
                m_scheme.remove_suffix(1);
            if (!is::scheme<traits_type>(m_scheme))
                throw stl::invalid_argument("The specified scheme is not valid");
            self.parse_scheme();
            if (self.scheme_end != self.data.size()) {
                self.replace_value(0,
                                   m_scheme.empty() && self.data.size() > self.scheme_end + 1 &&
                                   self.data[self.scheme_end] == ':'
                                   ? self.scheme_end + 1
                                   : self.scheme_end,
                                   m_scheme);
            } else {
                // the URI doesn't have a scheme now, we have to put it in the right place
                auto scheme_colon = m_scheme.empty() ? "" : str_t(m_scheme) + ':';
                if (self.authority_start != self.data.size()) {
                    self.replace_value(0, 0,
                                       scheme_colon + (ascii::starts_with(self.data, "//") ? "" : "//"));
                } else {
                    // It's a URN (or URN like URI)
                    self.replace_value(0, 0, scheme_colon);
                }
            }
            return *this;
        }

        /**
         * @brief clear scheme from uri
         */
        basic_uri& clear() noexcept {
            return set("");
        }
    };


}

#endif // WEBPP_SCHEME_HPP
