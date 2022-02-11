#ifndef WEBPP_HTTP_PROTO_BEAST_SERVER_HPP
#define WEBPP_HTTP_PROTO_BEAST_SERVER_HPP

#include "../../../configs/constants.hpp"
#include "../../../libs/asio.hpp"
#include "../../../memory/object.hpp"
#include "../../../std/string_view.hpp"
#include "../../../traits/enable_traits.hpp"
#include "../../../uri/uri.hpp"
#include "../../response_concepts.hpp"
#include "beast_request.hpp"

#include <thread>

// clang-format off
#include asio_include(steady_timer)
#include asio_include(ip/address)
#include asio_include(thread_pool)
#include asio_include(post)
#include asio_include(ip/tcp)
// clang-format on

#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/read.hpp>

namespace webpp::http::beast_proto {

    namespace details {

        template <typename ServerT>
        struct beast_session : stl::enable_shared_from_this<beast_session<ServerT>> {
            using server_type     = ServerT;
            using traits_type     = typename server_type::traits_type;
            using root_extensions = typename server_type::root_extensions;
            using acceptor_type   = asio::ip::tcp::acceptor;
            using socket_type     = asio::ip::tcp::socket;
            using endpoint_type   = asio::ip::tcp::endpoint;
            using steady_timer    = asio::steady_timer;
            using duration        = typename steady_timer::duration;
            using request_type = simple_request<traits_type, root_extensions, beast_request, root_extensions>;
            using buffer_type  = boost::beast::flat_buffer;
            using app_wrapper_ref = typename server_type::app_wrapper_ref;

          private:
            server_type&    server;
            socket_type     sock;
            acceptor_type   acceptor;
            steady_timer    timer;
            request_type    req;
            app_wrapper_ref app_ref;
            buffer_type     buf{default_buffer_size}; // fixme: see if this is using our allocator


          public:
            beast_session(server_type& serv_ref)
              : server{serv_ref},
                sock{server.io},
                acceptor{server.io, {server.bind_address, server.port}},
                timer{server.io, server.timeout},
                app_ref{server.app_ref} {}



            // Asynchronously receive a complete request message.
            void async_read_request() {
                boost::beast::http::async_read(
                  sock,
                  buf,
                  req.as_beast_request(),
                  [self = this->shared_from_this()](boost::beast::error_code     ec,
                                                    [[maybe_unused]] std::size_t bytes_transferred) {
                      if (!ec) {
                          self->server.logger.info("Recieved a request");
                          const HTTPResponse auto res = self->app_ref(self->req);
                          // todo
                      } else {
                          self->server.logger.warning("Connection error.", ec);
                      }
                  });
            }
        };

    } // namespace details

    template <Traits TraitsType, typename RootExtensionsT, typename App>
    struct beast_server : public enable_traits<TraitsType> {
        using traits_type       = TraitsType;
        using root_extensions   = RootExtensionsT;
        using steady_timer      = asio::steady_timer;
        using duration          = typename steady_timer::duration;
        using address_type      = asio::ip::address;
        using string_view_type  = traits::string_view<traits_type>;
        using port_type         = unsigned short;
        using thread_pool_type  = asio::thread_pool;
        using endpoint_type     = asio::ip::tcp::endpoint;
        using etraits           = enable_traits<traits_type>;
        using app_wrapper_ref   = stl::add_lvalue_reference_t<App>;
        using beast_server_type = beast_server;
        using session_type      = details::beast_session<beast_server_type>;

        // each request should finish before this
        duration timeout{stl::chrono::seconds(3)};

      private:
        friend session_type;

        address_type     bind_address;
        port_type        bind_port = 80;
        asio::io_context io;
        thread_pool_type pool;
        stl::size_t      pool_count;
        app_wrapper_ref  app_ref;

        void accept() {
            stl::allocate_shared<session_type>(this->allocs.template general_allocator<session_type>(),
                                               *this);
        }

        int start_io() noexcept {
            try {
                io.run();
                return 0;
            } catch (stl::exception const& err) {
                this->logger.error("Error while starting io server.", err);
            } catch (...) {
                // todo: possible data race
                this->logger.error("Unknown server error");
            }
            // todo: try running the server again
            return -1;
        }

      public:
        template <typename ET>
        requires(EnabledTraits<stl::remove_cvref_t<ET>>)
          beast_server(ET&&            et,
                       app_wrapper_ref the_app_ref,
                       stl::size_t     concurrency_hint = stl::thread::hardware_concurrency())
          : etraits{stl::forward<ET>(et)},
            io{static_cast<int>(concurrency_hint)},
            pool{concurrency_hint - 1}, // the main thread is one thread itself
            pool_count{concurrency_hint},
            app_ref{the_app_ref} {}


        beast_server& address(string_view_type addr) noexcept {
            asio::error_code ec;
            bind_address = asio::ip::make_address(istl::to_std_string_view(addr), ec);
            if (ec) {
                this->logger.error("Cannot set address", ec);
            }
            return *this;
        }


        beast_server& port(port_type p) noexcept {
            bind_port = p;
            return *this;
        }

        beast_server& post();
        beast_server& defer();

        [[nodiscard]] bool is_ssl_active() const noexcept {
            return false;
        }


        [[nodiscard]] static constexpr bool is_ssl_available() noexcept {
            return false;
        }

        beast_server& enable_ssl() {
            if constexpr (is_ssl_available()) {
                // todo
                if (bind_port == 80)
                    bind_port = 443;
            } else {
                this->logger.warning("Cannot enable SSL");
            }
            return *this;
        }

        auto binded_uri() const {
            auto u   = object::make_general<uri::uri>(*this);
            u.scheme = is_ssl_active() ? "https" : "http";
            u.host   = bind_address.to_string();
            u.port   = bind_port;
            return u;
        }

        // run the server
        [[nodiscard]] int operator()() noexcept {
            this->logger.info(fmt::format("Starting beast server on {}", binded_uri().to_string()));
            for (stl::size_t id = 1; id != pool_count; id++) {
                asio::post(pool, [this] {
                    start_io();
                });
            }
            for (;;)
                start_io();
        }
    };

} // namespace webpp::http::beast_proto

#endif // WEBPP_HTTP_PROTO_BEAST_SERVER_HPP
