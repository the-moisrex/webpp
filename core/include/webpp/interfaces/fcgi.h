#ifndef WEBPP_INTERFACE_FCGI
#define WEBPP_INTERFACE_FCGI

#include "./common/endpoints.h"
#include "basic_interface.h"
#include "webpp/http/request.h"

namespace webpp {

    class fcgi : public basic_interface<fcgi> {
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
         * This will only work before you run the operator()
         */
        void add_endpoint(std::string addr, uint_fast8_t port) noexcept {
            _endpoints.emplace_back(std::move(addr), port);
        }

        /**
         * Clear the endpoints
         */
        void clear_endpoints() noexcept { _endpoints.clear(); }

        /**
         * Return the endpoints.
         */
        auto const& endpoints() const noexcept { return _endpoints; }

        void disable_multithreading() noexcept {}
    };

    template <>
    class request_t<fcgi> {
      private:
      public:
    };

} // namespace webpp

#endif
