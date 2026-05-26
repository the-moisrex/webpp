#ifndef WEBPP_INTERFACE_FCGI
#define WEBPP_INTERFACE_FCGI

#include "../http/app_wrapper.hpp"
#include "../http/request.hpp"
#include "../std/internet.hpp"
#include "../std/set.hpp"
#include "../traits/enable_traits.hpp"
#include "fcgi_manager.hpp"
#include "fcgi_request.hpp"

namespace webpp::fastcgi {


    template <typename App>
    struct fcgi {
        using endpoint_type    = stl::net::ip::tcp::endpoint;
        using application_type = stl::remove_cvref_t<App>;
        using interface_type   = fcgi<server_type, application_type>;
        using request_type     = simple_request<fcgi_request, interface_type>;
        using allocator_type   = typename app_wrapper_type::allocator_type;
        using app_wrapper_type = http_app_wrapper<application_type>;

        static constexpr auto default_listen_address = "0.0.0.0";
        static constexpr auto default_listen_port    = 8080u;
        static constexpr auto logging_category       = "FastCGI";

        stl::set<endpoint_type> endpoints;
        server_type             server;
        app_wrapper_type        app;

        template <typename... Args>
        explicit fcgi(Args&&... args)
          : super{stl::forward<Args>(args)...},
            app{*this},
            endpoints{}, // todo: pass allocator
            server{*this} {}

        [[nodiscard]] static constexpr bool is_ssl_available() noexcept {
            return false; // it's not, it's FCGI, we just don't know
        }

        void operator()() noexcept {
            if (endpoints.empty()) {
                stl::net::error_code ec;
                endpoints.emplace(stl::net::ip::make_address(default_listen_address, ec), default_listen_port);
                if (!ec) {
                    this->logger.critical(
                      logging_category,
                      fmt::format("We're not able to listen to {}:{}", default_listen_address, default_listen_port),
                      ec);
                    return;
                }
            }
            server();
        }
    };


} // namespace webpp::fastcgi

#endif
