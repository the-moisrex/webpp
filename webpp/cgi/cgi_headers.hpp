#ifndef WEBPP_HTTP_CGI_HEADERS_HPP
#define WEBPP_HTTP_CGI_HEADERS_HPP

// #include "../http/headers.hpp"
#include "../http/headers/header_concepts.hpp"
#include "../http/protocol/http_limits.hpp"
#include "../std/string_view.hpp"
#include "../strings/charset.hpp"
#include "../utils/environ.hpp"

#include <array>

namespace webpp::http {

    namespace details {
        /**
         * Calculate header id but for CGI-style of headers
         * CGI destroys the distinction between _ and -. Under CGI it is impossible to know whether the original header
         * was x_test or x-test.
         */
        [[nodiscard]] static constexpr header_id_type cgi_header_id(stl::string_view const name) noexcept {
            stl::size_t                              index = 0;
            stl::array<char, max_header_name_length> buffer; // NOLINT(*-init)
            if (name.empty() || name.size() >= max_header_name_length) [[unlikely]] {
                return invalid_header_id;
            }
            for (char cur : name) {
                if (cur == '_') {
                    cur = '-';
                } else if (cur >= 'A' && cur <= 'Z') {
                    cur -= 'A' - 'a'; // tolower
                } else if (!LOWER_ALPHA_DIGIT<char>.contains(cur)) [[unlikely]] {
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
     * Satisfies stl::forward_iterator
     * todo: it is possible to make it a bidirectional iterator
     */
    struct [[nodiscard]] cgi_headers_iterator {
        using iterator_category = stl::forward_iterator_tag;
        using value_type        = stl::pair<header_id_type, stl::string_view>;
        using difference_type   = stl::ptrdiff_t;
        using pointer           = value_type const*;
        using reference         = value_type const&;

      private:
        char**     envp = nullptr;
        value_type current_header{invalid_header_id, {}};

        void advance_to_next_valid() noexcept {
            static constexpr stl::string_view HTTP_prefix = "HTTP_";

            for (; envp != nullptr && *envp != nullptr; ++envp) { // NOLINT(*-pointer-arithmetic)
                stl::string_view hdr{*envp};
                auto const       equal_sign      = hdr.find('=');
                bool const       not_found_equal = equal_sign == stl::string_view::npos;
                auto const       name            = not_found_equal ? hdr : hdr.substr(0, equal_sign);
                auto const       value           = not_found_equal ? stl::string_view{} : hdr.substr(equal_sign + 1);

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
            envp           = nullptr;
            current_header = {invalid_header_id, {}};
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

    /**
     * Implements Headers concepts for CGI protocol which is a provider type of class that gives access to headers.
     */
    struct [[nodiscard]] cgi_headers {
        [[nodiscard]] stl::string_view get(header_id_type const h_id) const noexcept {
            for (auto const [cid, value] : *this) {
                if (cid == h_id) {
                    return value;
                }
            }
            return {};
        }

        [[nodiscard]] stl::string_view get(stl::string_view const name) const noexcept {
            return get(header_id(name));
        }

        // NOLINTBEGIN(*-static)
        cgi_headers_iterator begin() const noexcept {
            return cgi_headers_iterator{get_environ()};
        }

        cgi_headers_iterator end() const noexcept {
            return {};
        }

        // NOLINTEND(*-static)
    };
} // namespace webpp::http

#endif // WEBPP_HTTP_CGI_HEADERS_HPP
