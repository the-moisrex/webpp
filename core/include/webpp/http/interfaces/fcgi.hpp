#ifndef WEBPP_INTERFACE_FCGI
#define WEBPP_INTERFACE_FCGI

#include "../../std/internet.hpp"
#include "../../std/set.hpp"
#include "../../std/vector.hpp"
#include "../request.hpp"
#include "./common/server.hpp"


namespace webpp {

    template <Traits TraitsType, typename /* fixme: RequestExtensionList */ REL, Interface IfaceType>
    struct fcgi_request : public REL {
        using traits_type            = TraitsType;
        using interface_type         = IfaceType;
        using request_extension_list = REL;
        using allocator_type   = typename traits_type::template allocator<typename traits_type::char_type>;
        using application_type = typename interface_type::application_type;
        using logger_type      = typename traits_type::logger;

      private:
        allocator_type alloc;

      public:
        logger_type& logger;

        fcgi_request(logger_type& logger, allocator_type const& alloc = allocator_type{}) noexcept
          : alloc(alloc),
            logger{logger} {}

        auto const& get_allocator() const noexcept {
            return alloc;
        }
    };


    template <Traits TraitsType, typename App, typename EList = empty_extension_pack>
    struct fcgi {
        using traits_type      = stl::remove_cvref_t<TraitsType>;
        using endpoint_type    = stl::net::ip::tcp::endpoint;
        using server_type      = common::server<traits_type>;
        using application_type = stl::remove_cvref_t<App>;
        using extension_list   = stl::remove_cvref_t<EList>;
        using interface_type   = fcgi<traits_type, application_type, extension_list>;
        using request_type     = simple_request<traits_type, fcgi_request, interface_type, extension_list>;
        using logger_type      = typename traits_type::logger;

        static constexpr auto default_listen_address = "0.0.0.0";
        static constexpr auto default_listen_port    = 8080u;

        server_type                          server;
        stl::set<traits_type, endpoint_type> endpoints;
        application_type                     app;
        logger_type                          logger;

        fcgi(logger_type logger = logger_type{}) noexcept : logger{logger} {};

        void operator()() noexcept {
            if (endpoints.empty()) {
                stl::net::error_code ec;
                endpoints.emplace(stl::net::ip::make_address(default_listen_address, ec),
                                  default_listen_port);
                if (!ec) {
                    logger.terminate(stl::format("We're not able to listen to {}:{}", default_listen_address,
                                                 default_listen_port),
                                     ec);
                    return;
                }
            }
            server();
        }

        auto resolve_endpoints() noexcept {
            stl::net::ip::tcp::resolver resolver(server.io);
            stl::error_code             ec;
            //        std::net::ip::tcp::resolver::results_type _endpoints;
            //        if (_fcgi->endpoints().empty()) {
            //            _endpoints = resolver.resolve(
            //                default_fcgi_listen_addr,
            //                std::to_string(default_fcgi_listen_port));
            //        } else {
            //            _endpoints = std::net::ip::tcp::v4();
            //        }
            return stl::vector<traits_type, stl::net::ip::tcp::endpoint>{};
        }
    };


} // namespace webpp

#endif
