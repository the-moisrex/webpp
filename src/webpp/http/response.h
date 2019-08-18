#ifndef RESPONSE_H
#define RESPONSE_H

#include "body.h"
#include "header.h"
#include <memory>
#include <string>

namespace webpp {

    template <typename Interface>
    class response {
      public:
        using body_type = webpp::body<Interface>;
        using header_type = webpp::header<Interface>;

      private:
        std::unique_ptr<body_type> _body = nullptr;
        std::unique_ptr<header_type> _header = nullptr;
        std::shared_ptr<Interface> interface;

      public:
        response() = default;
        response(std::shared_ptr<Interface> _interface)
            : interface(_interface) {}

        response& headers(header_type&& __header) noexcept {
            _header = std::move(__header);
            return *this;
        }

        response& headers(header_type const& __header) noexcept {
            _header = __header;
            return *this;
        }

        auto const& headers() const noexcept { return _header; }

        auto& headers() noexcept { return _header; }

        /**
         * @brief set the body of the response
         * @param __body
         * @return itself
         */
        response& body(body_type&& __body) noexcept {
            _body = std::move(__body);
            return *this;
        }

        /**
         * @brief set the body of the response
         * @param __body
         * @return itself
         */
        response& body(body_type const& __body) noexcept {
            _body = __body;
            return *this;
        }

        /**
         * @brief get the body of response
         * @return a pointer to the body
         */
        auto& body() & noexcept { return _body; }
    };

} // namespace webpp
#endif // RESPONSE_H
