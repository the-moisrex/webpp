#ifndef RESPONSE_H
#define RESPONSE_H

#include "body.h"
#include "header.h"
#include <memory>
#include <string_view>

namespace webpp {

    /**
     * This class owns its data.
     */
    class response {
    public:
        using body_type = webpp::body;
        using header_type = webpp::headers;

        body_type body;
        header_type header;

        response() noexcept = default;

        response(response const &res) noexcept {
            body = res.body;
            header = res.header;
        }

        response(response &&res) noexcept {
            if (this != &res) {
                body = std::move(res.body);
                header = std::move(res.header);
            }
        }

        response &operator=(response const &) = default;

        response &operator=(response &&res) noexcept {
            if (&res != this) {
                body = std::move(res.body);
                header = std::move(res.header);
            }
            return *this;
        }

        response &operator=(std::string const &str) noexcept {
            body.replace_string(str);
            return *this;
        }

        response &operator=(std::string &str) noexcept {
            body.replace_string(std::move(str));
            return *this;
        }

        response(std::string const &b) noexcept : body{b} {}

        response(std::string &&b) noexcept : body{std::move(b)} {}

        [[nodiscard]] bool operator==(response const &res) const noexcept {
            return body == res.body && header == res.header;
        }

        [[nodiscard]] bool operator!=(response const &res) const noexcept {
            return !operator==(res);
        }

        response &operator<<(std::string_view str) noexcept;

        operator std::string_view() const noexcept {
            return body.str();
        }

        operator std::string() const noexcept {
            return std::string(body.str());
        }

        void calculate_default_headers() noexcept;
    };



} // namespace webpp
#endif // RESPONSE_H
