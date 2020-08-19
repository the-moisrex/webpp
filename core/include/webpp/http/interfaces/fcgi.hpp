#ifndef WEBPP_INTERFACE_FCGI
#define WEBPP_INTERFACE_FCGI

#include "../../std/internet.hpp"
#include "../../std/set.hpp"
#include "../../std/vector.hpp"
#include "../request.hpp"
#include "../../server/server.hpp"

#include <type_traits>


namespace webpp {

    template <Traits TraitsType, typename /* fixme: RequestExtensionList */ REL, Interface IfaceType>
    struct fcgi_request : public REL {
        using traits_type            = TraitsType;
        using interface_type         = IfaceType;
        using request_extension_list = REL;
        using allocator_type   = typename traits_type::template allocator<typename traits_type::char_type>;
        using application_type = typename interface_type::application_type;
        using logger_type      = typename traits_type::logger;
        using logger_type_ref  = typename traits_type::logger::logger_ref;

      private:
        allocator_type alloc;

      public:
        [[no_unique_address]] logger_type_ref logger;

        fcgi_request(logger_type_ref       logger = logger_type{},
                     allocator_type const& alloc  = allocator_type{}) noexcept
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
        using logger_type_ref  = typename traits_type::logger::logger_ref;

        static constexpr auto default_listen_address = "0.0.0.0";
        static constexpr auto default_listen_port    = 8080u;
        static constexpr auto logging_category       = "FastCGI";

        server_type                           server;
        stl::set<traits_type, endpoint_type>  endpoints;
        application_type                      app;
        [[no_unique_address]] logger_type_ref logger;

        fcgi(logger_type_ref logger = logger_type{}) noexcept : logger{logger} {};

        void operator()() noexcept {
            if (endpoints.empty()) {
                stl::net::error_code ec;
                endpoints.emplace(stl::net::ip::make_address(default_listen_address, ec),
                                  default_listen_port);
                if (!ec) {
                    logger.critical(logging_category,
                                    stl::format("We're not able to listen to {}:{}", default_listen_address,
                                                default_listen_port),
                                    ec);
                    return;
                }
            }
            server();
        }
    };


} // namespace webpp

#endif
