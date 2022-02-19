#ifndef WEBPP_HTTP_PROTO_BEAST_SERVER_HPP
#define WEBPP_HTTP_PROTO_BEAST_SERVER_HPP

#include "../../../configs/constants.hpp"
#include "../../../libs/asio.hpp"
#include "../../../memory/object.hpp"
#include "../../../std/format.hpp"
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

#include <boost/beast/core.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/write.hpp>

namespace webpp::http::beast_proto {

    namespace details {

        template <typename ServerT>
        struct beast_session : stl::enable_shared_from_this<beast_session<ServerT>> {
            using server_type     = ServerT;
            using traits_type     = typename server_type::traits_type;
            using root_extensions = typename server_type::root_extensions;
            using endpoint_type   = asio::ip::tcp::endpoint;
            using steady_timer    = asio::steady_timer;
            using duration        = typename steady_timer::duration;
            using request_type = simple_request<traits_type, root_extensions, beast_request, root_extensions>;
            using buffer_type  = boost::beast::flat_buffer;
            using app_wrapper_ref           = typename server_type::app_wrapper_ref;
            using beast_response_type       = boost::beast::http::response<boost::beast::http::dynamic_body>;
            using beast_request_type        = typename request_type::beast_request_type;
            using beast_request_body_type   = boost::beast::http::string_body;
            using beast_request_parser_type = boost::beast::http::request_parser<beast_request_body_type>;

          private:
            server_type&    server; // fixme: race condition
            steady_timer    timer;
            request_type    req;
            app_wrapper_ref app_ref;
            buffer_type     buf{default_buffer_size}; // fixme: see if this is using our allocator


          public:
            beast_session(server_type& serv_ref)
              : server{serv_ref},
                timer{server.io, server.timeout},
                req{server},
                app_ref{server.app_ref} {}

            /**
             * Running async_read_request directly in the constructor will not make
             * make_shared (or alike) functions work properly.
             */
            void start() {
                async_read_request();
            }

          private:
            beast_response_type make_beast_response(beast_request_type breq, HTTPResponse auto&& res) const {
                res.calculate_default_headers();
                beast_response_type bres;
                bres.version(breq.version());
                for (auto const& h : res.headers) {
                    bres.set(h.name, h.value);
                }
                boost::beast::ostream(bres.body()) << res.body.str();
                // bres.content_length(res.body.size());
                return bres;
            }



            // Asynchronously receive a complete request message.
            void async_read_request() {
                server.logger.info("Started reading request.");
                auto self = this->shared_from_this();
                boost::beast::http::async_read(
                  server.sock,
                  buf,
                  req.get_parser(),
                  [self](boost::beast::error_code ec, [[maybe_unused]] std::size_t bytes_transferred) {
                      if (!ec) [[likely]] {
                          self->server.logger.info("Recieved a request");
                          self->async_write_response();
                      } else {
                          self->server.logger.warning("Connection error.", ec);
                      }
                  });
            }


            void async_write_response() {
                const auto bres = make_beast_response(req.as_beast_request(), app_ref(req));
                auto       self = this->shared_from_this();
                boost::beast::http::async_write(
                  server.sock,
                  bres,
                  [self](boost::beast::error_code ec, stl::size_t) noexcept {
                      self->server.sock.shutdown(asio::ip::tcp::socket::shutdown_send, ec);
                      self->timer.cancel();
                  });
            }
        };


    } // namespace details




    /**
     * Beast Server
     */
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
        using acceptor_type     = asio::ip::tcp::acceptor;
        using socket_type       = asio::ip::tcp::socket;

        // each request should finish before this
        duration timeout{stl::chrono::seconds(3)};

      private:
        friend session_type;

        address_type     bind_address;
        port_type        bind_port = 80;
        asio::io_context io;
        socket_type      sock;
        acceptor_type    acceptor;
        thread_pool_type pool;
        stl::size_t      pool_count;
        app_wrapper_ref  app_ref;

        void async_accept() noexcept {
            this->logger.info("Accepting Request");
            acceptor.async_accept(sock, [this](boost::beast::error_code ec) noexcept {
                if (!ec) {
                    stl::allocate_shared<session_type>(
                      this->alloc_pack.template general_allocator<session_type>(),
                      *this)
                      ->start();
                } else {
                    this->logger.warning("Accepting error", ec);
                }
                this->async_accept();
            });
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
        beast_server(beast_server const&) = delete;
        beast_server& operator=(beast_server const&) = delete;

        template <typename ET>
        requires(EnabledTraits<stl::remove_cvref_t<ET>>)
          beast_server(ET&&            et,
                       app_wrapper_ref the_app_ref,
                       stl::size_t     concurrency_hint = stl::thread::hardware_concurrency())
          : etraits{stl::forward<ET>(et)},
            io{static_cast<int>(concurrency_hint)},
            sock{io},
            acceptor{io},
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

        beast_server& enable_ssl() noexcept {
            if constexpr (is_ssl_available()) {
                if (!is_ssl_active()) {
                    // todo
                }
                if (bind_port == 80)
                    bind_port = 443;
            } else {
                this->logger.warning("Cannot enable SSL");
            }
            return *this;
        }

        beast_server& disable_ssl() noexcept {
            if constexpr (is_ssl_available()) {
                if (is_ssl_active()) {
                    // todo
                }
                if (bind_port == 443)
                    bind_port = 80;
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
            boost::beast::error_code ec;
            const endpoint_type      ep{bind_address, bind_port};
            acceptor.open(ep.protocol(), ec);
            if (ec) {
                this->logger.error(fmt::format("Cannot open protocol for {}", binded_uri().to_string()), ec);
                return -1;
            }
            acceptor.bind(ep, ec);
            if (ec) {
                this->logger.error(fmt::format("Cannot bind to {}", binded_uri().to_string()), ec);
                return -1;
            }
            acceptor.listen(asio::socket_base::max_listen_connections, ec);
            if (ec) {
                this->logger.error(fmt::format("Cannot listen to {}", binded_uri().to_string()), ec);
                return -1;
            }

            async_accept();

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
