#ifndef REQUEST_H
#define REQUEST_H

#include "headers.hpp"

namespace webpp {

    class request {
      private:
        webpp::headers request_headers;

      public:
        request() = default;
        request(webpp::headers request_headers) noexcept;

        inline webpp::headers const& headers() const noexcept {
            return request_headers;
        }

        /**
         * @brief header
         * @details this method might return multiple elements.
         * @param key
         */
        inline auto header(std::string const& key) const noexcept {
            return request_headers.find(key);
        }
    };
} // namespace webpp

#endif // REQUEST_H
