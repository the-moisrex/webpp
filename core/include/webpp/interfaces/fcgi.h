#ifndef WEBPP_INTERFACE_FCGI
#define WEBPP_INTERFACE_FCGI

#include "../http/request.h"
#include "../std/internet.h"
#include "basic_interface.h"
#include <set>

namespace webpp {

    class fcgi : public basic_interface<fcgi> {
      public:
        using endpoint_t = std::net::ip::tcp::endpoint;

      private:
        class fcgi_impl;
        std::set<endpoint_t> _endpoints;
        std::unique_ptr<fcgi_impl> impl;

      public:
        webpp::router<fcgi> router;

        fcgi() noexcept;

        void operator()() noexcept;

        /**
         * This will only work before you run the operator()
         */
        void add_endpoint(endpoint_t _endpoint) noexcept {
            _endpoints.insert(std::move(_endpoint));
        }

        /**
         * This will only work before you run the operator()
         */
        void add_endpoint(std::string_view const& addr,
                          uint_fast8_t port) noexcept {
            _endpoints.emplace(std::net::ip::make_address(addr), port);
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
      public:
    };

} // namespace webpp

#endif
