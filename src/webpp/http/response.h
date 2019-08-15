#ifndef RESPONSE_H
#define RESPONSE_H

#include "body.h"
#include "headers.hpp"
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

      public:
        response() = default;

        /**
         * @brief set the body of the response
         * @param __body
         * @return itself
         */
        response* body(body_type&& __body) noexcept {
            _body = std::move(__body);
            return this;
        }

        /**
         * @brief set the body of the response
         * @param __body
         * @return itself
         */
        response* body(body_type const& __body) noexcept {
            _body = __body;
            return this;
        }

        /**
         * @brief get the body of response
         * @return a pointer to the body
         */
        auto& body() & noexcept { return _body; }
    };

} // namespace webpp
#endif // RESPONSE_H
