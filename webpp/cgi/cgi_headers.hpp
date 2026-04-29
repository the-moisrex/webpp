#ifndef WEBPP_HTTP_CGI_HEADERS_HPP
#define WEBPP_HTTP_CGI_HEADERS_HPP

// #include "../http/headers.hpp"
#include "../http/headers/header_concepts.hpp"
#include "../http/protocol/http_limits.hpp"
#include "../std/string_view.hpp"

#include <array>

namespace webpp::http {

    namespace details {
        /**
         * Calculate header id but for CGI-style of headers
         */
        [[nodiscard]] static constexpr header_id_type cgi_header_id(stl::string_view const name) noexcept {
            stl::size_t                              index = 0;
            stl::array<char, max_header_name_length> buffer; // NOLINT(*-init)
            if (name.size() >= max_header_name_length) [[unlikely]] {
                return invalid_header_id;
            }
            for (char cur : name) {
                if (cur == '_') {
                    cur = '-';
                } else if (cur >= 'A' && cur <= 'Z') {
                    cur -= 'A' - 'a';
                } else if (cur < 'a' || cur > 'z') [[unlikely]] {
                    return invalid_header_id;
                }
                buffer.at(index) = cur;
                ++index;
            }
            buffer.at(index) = '\0';
            return header_id(stl::string_view{buffer.data(), buffer.data() + index});
        }
    } // namespace details

    /**
     * Satisfies std::forward_iterator
     */
    struct [[nodiscard]] cgi_headers_iterator {
        using iterator_category = stl::forward_iterator_tag;
        using value_type        = stl::pair<header_id_type, stl::string_view>;
        using difference_type   = stl::ptrdiff_t;
        using pointer           = value_type const*;
        using reference         = value_type const&;

      private:
        char**     envp = nullptr;
        value_type current_header;

        void advance_to_next_valid() {
            static constexpr std::string_view HTTP_prefix = "HTTP_";

            for (; envp != nullptr && *envp != nullptr; ++envp) { // NOLINT(*-pointer-arithmetic)
                std::string_view hdr{*envp};
                auto const       equal_sign = hdr.find('=');

                if (equal_sign == std::string_view::npos) [[unlikely]] {
                    continue;
                }

                auto const name  = hdr.substr(0, equal_sign);
                auto const value = hdr.substr(equal_sign + 1);

                if (name.starts_with(HTTP_prefix)) {
                    auto const parsed_name = name.substr(HTTP_prefix.size());
                    auto const h_id        = details::cgi_header_id(parsed_name);
                    if (h_id == invalid_header_id) [[unlikely]] {
                        continue;
                    }
                    current_header = {h_id, value};
                    return;
                }
                if (name == "CONTENT_LENGTH") {
                    current_header = {header_id("content-length"), value};
                    return;
                }
                if (name == "CONTENT_TYPE") {
                    current_header = {header_id("content-type"), value};
                    return;
                }
            }
            // Reached the end of the environment variables
            envp = nullptr;
        }

      public:
        constexpr cgi_headers_iterator() noexcept                                  = default;
        constexpr cgi_headers_iterator(cgi_headers_iterator const&)                = default;
        constexpr cgi_headers_iterator(cgi_headers_iterator&&) noexcept            = default;
        constexpr cgi_headers_iterator& operator=(cgi_headers_iterator const&)     = default;
        constexpr cgi_headers_iterator& operator=(cgi_headers_iterator&&) noexcept = default;
        constexpr ~cgi_headers_iterator() noexcept                                 = default;

        // Pass `::environ` (POSIX) or `::_environ` (Windows) here
        explicit cgi_headers_iterator(char** env) : envp(env) {
            advance_to_next_valid();
        }

        [[nodiscard]] reference operator*() const noexcept {
            return current_header;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            return &current_header;
        }

        cgi_headers_iterator& operator++() {
            if (envp != nullptr && *envp != nullptr) {
                ++envp; // NOLINT(*-pointer-arithmetic)
                advance_to_next_valid();
            }
            return *this;
        }

        cgi_headers_iterator operator++(int) {
            cgi_headers_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        [[nodiscard]] bool operator==(cgi_headers_iterator const& rhs) const noexcept {
            return envp == rhs.envp;
        }

        [[nodiscard]] bool operator!=(cgi_headers_iterator const& rhs) const noexcept {
            return !(*this == rhs);
        }
    };

    // struct [[nodiscard]] cgi_headers {
    //     [[nodiscard]] stl::string_view get(stl::string_view const name) const noexcept {}

    //     [[nodiscard]] stl::string_view get(header_id_type h_id) const noexcept {}

    //     [[nodiscard]] auto begin() const noexcept {}

    //     [[nodiscard]] auto end() const noexcept {}
    // };
} // namespace webpp::http

#endif // WEBPP_HTTP_CGI_HEADERS_HPP
