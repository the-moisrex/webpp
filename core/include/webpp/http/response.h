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
        response(response const &res) noexcept;
        response(response &&res) noexcept;
        response(std::string const &b) noexcept;
        response(std::string &&b) noexcept;

        response &operator=(response const &) = default;
        response &operator=(response &&res) noexcept;
        response &operator=(std::string const &str) noexcept;
        response &operator=(std::string &str) noexcept;

        [[nodiscard]] bool operator==(response const &res) const noexcept;
        [[nodiscard]] bool operator!=(response const &res) const noexcept;

        response &operator<<(std::string_view str) noexcept;

        operator std::string_view() const noexcept;
        operator std::string() const noexcept;

        void calculate_default_headers() noexcept;
    };



} // namespace webpp
#endif // RESPONSE_H
