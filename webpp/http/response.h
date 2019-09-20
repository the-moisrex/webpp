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
        using header_type = webpp::headers<Interface>;

      private:
        int _status_code = 200;
        body_type _body;
        header_type _header;
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

        /**
         * @brief get status code
         */
        auto status_code() const noexcept { return _status_code; }

        /**
         * @brief set status code
         * @param status_code
         */
        void status_code(decltype(_status_code) __status_code) noexcept {
            _status_code = __status_code;
        }
    };

} // namespace webpp
#endif // RESPONSE_H
