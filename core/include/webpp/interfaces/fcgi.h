#ifndef WEBPP_INTERFACE_FCGI
#define WEBPP_INTERFACE_FCGI

#include "basic_interface.h"
#include "webpp/http/request.h"

namespace webpp {

    class fcgi : public basic_interface<fcgi> {
      public:
        /**
         * The endpoint may change to std::net::ip::tcp::endpoint (I'm not sure
         * about the namespaces); but until 2023, I'm gonna use this instead.
         */
        struct endpoint {
            std::string address;
            uint_fast8_t port;
        };

      private:
        class fcgi_impl;
        std::vector<endpoint> _endpoints;
        std::unique_ptr<fcgi_impl> impl;

      public:
        webpp::router<fcgi> router;

        fcgi() noexcept;

        void operator()() noexcept;

        /**
         * This will only work before you run the operator()
         */
        void add_endpoint(endpoint _endpoint) noexcept {
            _endpoints.push_back(std::move(_endpoint));
        }

        /**
         * Clear the endpoints
         */
        void clear_endpoints() noexcept { _endpoints.clear(); }

        auto const& endpoints() const noexcept { return _endpoints; }
    };

    template <>
    class request_t<fcgi> {
      private:
        /**
         * This is the version 1.0 of the FastCGI Record protocol
         */
        struct fcgi_record {
            using element_t = uint8_t;

            /* Identifies the FastCGI protocol version. This is the version 1 */
            element_t version = 1;

            /* Identifies the FastCGI record type, i.e. the general function
             * that the record performs. Specific record types and their
             * functions are detailed in later sections */
            element_t type;

            /* Identifies the FastCGI request to which the record belongs */
            element_t request_id_b1;
            element_t request_id_b0;

            /* The number of bytes in the contentData component of the record */
            element_t content_length_b1;
            element_t content_length_b0;

            /* The number of bytes in the paddingData component of the record */
            element_t padding_length;

            /* reserved for later use */
            element_t reserved;

            // unsigned char contentData[contentLength];
            // unsigned char paddingData[paddingLength];

            uint16_t request_id() const noexcept {
                return (static_cast<uint16_t>(request_id_b1) << 8u) |
                       request_id_b0;
            }

            uint16_t content_length() const noexcept {
                return (static_cast<uint16_t>(content_length_b1) << 8u) |
                       content_length_b0;
            }
        };

        fcgi_record record;

      public:
    };

} // namespace webpp

#endif
