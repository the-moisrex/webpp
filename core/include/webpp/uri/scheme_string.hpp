// Created by moisrex on 11/4/20.

#ifndef WEBPP_SCHEME_STRING_HPP
#define WEBPP_SCHEME_STRING_HPP

#include "./parser.hpp"

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

    template <typename ...T>
    struct uri_scheme : public uri_parser<T...> {
        using super = uri_parser<T...>;
        using string_view_type = typename super::string_view_type;
        using traits_type = typename super::traits_type;

        constexpr uri_scheme(uri_type the_uri) = default;

        auto& operator=(istl::StringViewifiable auto&& new_scheme) noexcept {
            set(stl::forward<decltype(new_scheme)>(new_scheme));
            return *this;
        }

        /**
         * @brief check if the specified uri has a scheme or not
         */
        [[nodiscard]] bool empty() const noexcept {
            this->parse_scheme();
            return this->scheme_end == this->data.size() && this->scheme_end != 0;
        }

        /**
         * @brief scheme
         * @return get scheme
         */
        [[nodiscard]] string_view_type value() const noexcept {
            this->parse_scheme();
            return this->scheme_end == this->data.size() ? string_view_type() : this->substr(0, this->scheme_end);
        }

        /**
         * @brief set scheme
         * @param _scheme
         * @throws logic_error if uri is const
         */
        auto& set(string_view_type m_scheme) {
            if (ascii::ends_with(m_scheme, ':'))
                m_scheme.remove_suffix(1);
            if (!is::scheme(m_scheme))
                throw stl::invalid_argument("The specified scheme is not valid");
            this->parse_scheme();
            if (this->scheme_end != this->data.size()) {
                this->replace_value(0,
                                    m_scheme.empty() && this->data.size() > this->scheme_end + 1 &&
                                    this->data[this->scheme_end] == ':'
                                    ? this->scheme_end + 1
                                    : this->scheme_end,
                                    m_scheme);
            } else {
                // the URI doesn't have a scheme now, we have to put it in the right place
                auto scheme_colon = m_scheme.empty() ? "" : str_t(m_scheme) + ':';
                if (this->authority_start != this->data.size()) {
                    this->replace_value(0, 0,
                                        scheme_colon + (ascii::starts_with(this->data, "//") ? "" : "//"));
                } else {
                    // It's a URN (or URN like URI)
                    this->replace_value(0, 0, scheme_colon);
                }
            }
            return *this;
        }

        /**
         * @brief clear scheme from uri
         */
        auto& clear() noexcept {
            return set("");
        }
    };


}

#endif // WEBPP_SCHEME_STRING_HPP
