#ifndef WEBPP_INTERFACE_FCGI
#define WEBPP_INTERFACE_FCGI

#include "../http/request.h"
#include "../std/internet.h"
#include "../std/set.h"
#include "../std/vector.h"
#include "basic_interface.h"
#include "common/server.h"


namespace webpp {

    template <typename Traits>
    class fcgi : public basic_interface<Traits, fcgi<Traits>> {
      public:
        static_assert(
          is_traits_v<Traits>,
          "The specified template parameter is not a valid traits type.");
        using traits     = Traits;
        using endpoint_t = std::net::ip::tcp::endpoint;

      private:
        stl::set<traits, endpoint_t> _endpoints;
        common::server               _server;

        auto get_endpoints() noexcept {
            std::net::ip::tcp::resolver resolver(_server.io);
            std::error_code             ec;
            //        std::net::ip::tcp::resolver::results_type _endpoints;
            //        if (_fcgi->endpoints().empty()) {
            //            _endpoints = resolver.resolve(
            //                default_fcgi_listen_addr,
            //                std::to_string(default_fcgi_listen_port));
            //        } else {
            //            _endpoints = std::net::ip::tcp::v4();
            //        }
            return stl::vector<traits, std::net::ip::tcp::endpoint>{};
        }


      public:
        webpp::router<traits, fcgi> router;

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
                          uint_fast8_t            port) noexcept {
            _endpoints.emplace(std::net::ip::make_address(addr), port);
        }

        /**
         * Clear the endpoints
         */
        void clear_endpoints() noexcept {
            _endpoints.clear();
        }

        /**
         * Return the endpoints.
         */
        auto const& endpoints() const noexcept {
            return _endpoints;
        }
    };

    template <typename Traits>
    class request_t<Traits, fcgi<Traits>> {
      private:
      public:
    };

} // namespace webpp

#endif
