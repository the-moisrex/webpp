// Created by moisrex on 10/30/20.

#ifndef WEBPP_SCHEME_HPP
#define WEBPP_SCHEME_HPP

#include "../std/string.hpp"
#include "./details/uri_status.hpp"

namespace webpp::uri {

    enum struct scheme_status {
#define webpp_def(status) status = stl::to_underlying(uri::uri_status::status)
        webpp_def(valid), // valid scheme
#undef webpp_def
    };

    /**
     * Get the error message as a string view
     */
    static constexpr stl::string_view to_string(scheme_status status) noexcept {
        switch (status) {
            using enum scheme_status;
            case valid: return "Valid Scheme";
        }
        stl::unreachable();
    }

    /**
     * parse the scheme
     * this method will fill the "authority_start" and "scheme_end" vars
     */
    constexpr scheme_status parse_scheme(const char*& pos, const char* end) noexcept {
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
                m_scheme.substr(1).find_first_not_of(details::SCHEME_NOT_FIRST<char_type>.string_view())) {
                scheme_end = colon;

                if (_data.substr(colon + 1, 2) == "//") {
                    authority_start = colon + 3;
                } else {
                    // it should be a URN or an invalid URI at this point
                    authority_start = data.size();
                    if (!ascii::iequals<ascii::char_case_side::second_lowered>(_data.substr(0, scheme_end),
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




    template <istl::String StringType = stl::string>
    struct basic_scheme : stl::remove_cvref_t<StringType> {
        using string_type = stl::remove_cvref_t<StringType>;

        template <typename... T>
        constexpr basic_scheme(T&&... args) : string_type{stl::forward<T>(args)...} {}



        /**
         * @brief checks if the URI is a relative reference
         */
        [[nodiscard]] constexpr bool is_relative_reference() const noexcept {
            return this->empty();
        }

        /**
         * @return 0 if unknown, otherwise return the port
         */
        [[nodiscard]] constexpr stl::uint16_t known_port() const noexcept {
            // NOLINTBEGIN(*-avoid-magic-numbers)
            switch (this->size()) {
                case 2:
                    if (this->operator[](0) == 'w' && this->operator[](1) == 's')
                        return 80u;
                    break;
                case 3:
                    if (*this == "wss")
                        return 443u;
                    else if (*this == "ftp")
                        return 21;
                    break;
                case 4:
                    if (*this == "http")
                        return 80u;
                    break;
                case 5:
                    if (*this == "https")
                        return 443;
                    break;
            }
            return 0u;
            // NOLINTEND(*-avoid-magic-numbers)
        }

        [[nodiscard]] constexpr bool is_known() const noexcept {
            return known_port() != 0u;
        }

        void append_to(istl::String auto& out) const {
            if (!this->empty()) {
                // out.reserve(out.size() + this->size() + 1);
                out.append(*this);
                out.push_back(':');
                out.append("//");
            } else {
                out.append("//");
            }
        }
    };

} // namespace webpp::uri

#endif // WEBPP_SCHEME_HPP
