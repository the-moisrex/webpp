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
            body = std::move(body);
            header = std::move(header);
        }

        response &operator=(response const &) = default;

        response &operator=(response &&res) noexcept {
            if (&res != this) {
                body = std::move(res.body);
                header = std::move(res.header);
            }
            return *this;
        }

        response(std::string_view const &res) noexcept : body{res} {}

        [[nodiscard]] bool operator==(response const &res) const noexcept {
            return body == res.body && header == res.header;
        }

        [[nodiscard]] bool operator!=(response const &res) const noexcept {
            return !operator==(res);
        }

        response &operator<<(std::string_view str) noexcept;
    };

} // namespace webpp
#endif // RESPONSE_H
