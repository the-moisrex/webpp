#ifndef WEBPP_RESPONSE_H
#define WEBPP_RESPONSE_H

#include "body.h"
#include "header.h"

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>

namespace webpp {

    /**
     * This class owns its data.
     */
    template <typename Traits = std_traits>
    class response {
      public:
        using traits     = Traits;
        using body_t     = webpp::body;
        using header_t   = webpp::headers<Traits, true, header_type::response>;
        using str_view_t = typename traits::string_view_type;
        using str_t      = typename traits::string_type;

        body_t   body;
        header_t header;

        response() noexcept                    = default;
        response(response const& res) noexcept = default;
        response(response&& res) noexcept      = default;
        response(str_t const& b) noexcept : body(b) {
        }
        response(str_t&& b) noexcept : body(std::move(b)) {
        }


        response& operator=(response const&) = default;
        response& operator=(response&& res) noexcept = default;
        response& operator=(str_t const& str) noexcept {
            body.replace_string(str);
            return *this;
        }
        response& operator=(str_t&& str) noexcept {
            body.replace_string(std::move(str));
            return *this;
        }

        [[nodiscard]] bool operator==(response const& res) const noexcept {
            return body == res.body && header == res.header;
        }
        [[nodiscard]] bool operator!=(response const& res) const noexcept {
            return body != res.body || header != res.header;
        }

        response& operator<<(str_view_t const& str) noexcept {
            body << str;
            return *this;
        }

        operator str_view_t() const noexcept {
            return body.str();
        }
        operator str_t() const noexcept {
            return str_t{body.str()};
        }

        void calculate_default_headers() noexcept {
            // todo: use C++20 header.contains instead when possible
            if (header.find("Content-Type") == header.cend())
                header.emplace("Content-Type", "text/html; charset=utf-8");

            if (header.find("Content-Length") == header.cend())
                header.emplace(
                  "Content-Length",
                  std::to_string(body.str().size() * sizeof(char)));
        }


        // static methods:
        static response file(std::filesystem::path const& file) noexcept;
        static response image(std::string_view const& file) noexcept;
        static response json_file(std::string_view const& file) noexcept;
    };

} // namespace webpp
#endif // WEBPP_RESPONSE_H
