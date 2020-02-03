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

        response() = default;

        response(std::string_view const &res) noexcept : body{res} {}

        response &operator<<(std::string_view str) noexcept;
    };

} // namespace webpp
#endif // RESPONSE_H
