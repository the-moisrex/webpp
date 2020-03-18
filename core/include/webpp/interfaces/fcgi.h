#ifndef WEBPP_INTERFACE_FCGI
#define WEBPP_INTERFACE_FCGI

#include "basic_interface.h"
#include "webpp/http/request.h"
#include <bits/stdint-uintn.h>

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

        /**
         * Return the endpoints.
         */
        auto const& endpoints() const noexcept { return _endpoints; }
    };

    template <>
    class request_t<fcgi> {
      private:
        /**
         * It is very important that this record_type's size be uint8_t
         */
        enum class record_type : uint8_t {
            begin_request = 1,
            abort_request = 2,
            end_request = 3,
            params = 4,
            std_in = 5,
            std_out = 6,
            std_err = 7,
            data = 8,
            get_values = 9,
            get_values_result = 10,
            unknown_type = 11
        };

        /**
         * This is the version 1.0 of the FastCGI Record protocol
         */
        struct header {

            /* Identifies the FastCGI protocol version. This is the version 1 */
            uint8_t version = 1;

            /* Identifies the FastCGI record type, i.e. the general function
             * that the record performs. Specific record types and their
             * functions are detailed in later sections */
            record_type type;

            /* Identifies the FastCGI request to which the record belongs */
            uint8_t request_id_b1;
            uint8_t request_id_b0;

            /* The number of bytes in the contentData component of the record */
            uint8_t content_length_b1;
            uint8_t content_length_b0;

            /* The number of bytes in the paddingData component of the record */
            uint8_t padding_length;

            /* reserved for later use */
            uint8_t reserved;

            uint16_t request_id() const noexcept {
                return (static_cast<uint16_t>(request_id_b1) << 8u) |
                       request_id_b0;
            }

            void request_id(uint16_t req_id) noexcept {
                request_id_b1 = static_cast<uint8_t>(req_id >> 8u);
                request_id_b0 = static_cast<uint8_t>(req_id);
            }

            uint16_t content_length() const noexcept {
                return (static_cast<uint16_t>(content_length_b1) << 8u) |
                       content_length_b0;
            }

            void content_length(uint16_t _content_length) noexcept {
                content_length_b1 = static_cast<uint8_t>(_content_length >> 8u);
                content_length_b0 = static_cast<uint8_t>(_content_length);
            }

            /**
             * Generally there are two types of records, Managements and the
             * Applocation.
             */
            bool is_management_record() const noexcept {
                return request_id() == 0;
            }
        };

        class begin_request {
          private:
            uint8_t role_b1;
            uint8_t role_b0;
            uint8_t flags;
            uint8_t reserved[5];
          public:
        };

        class end_request {
          private:
            uint8_t app_status_b3;
            uint8_t app_status_b2;
            uint8_t app_status_b1;
            uint8_t app_status_b0;
            uint8_t protocol_status;
            uint8_t reserved[3];
          public:

            void app_status(uint32_t status_code) noexcept {
              app_status_b3 = static_cast<uint8_t>(status_code >> 24u);
              app_status_b2 = static_cast<uint8_t>(status_code >> 16u & 0xFFu);
              app_status_b1 = static_cast<uint8_t>(status_code >> 8u & 0xFFu);
              app_status_b0 = static_cast<uint8_t>(status_code);
            }

        };

      public:
    };

} // namespace webpp

#endif
