#ifndef WEBPP_INTERFACE_FCGI
#define WEBPP_INTERFACE_FCGI

#include "basic_interface.h"

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

} // namespace webpp

#endif
