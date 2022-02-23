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

#include <list>
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
        struct beast_worker {
            using server_type     = ServerT;
            using traits_type     = typename server_type::traits_type;
            using root_extensions = typename server_type::root_extensions;
            using endpoint_type   = asio::ip::tcp::endpoint;
            using steady_timer    = asio::steady_timer;
            using duration        = typename steady_timer::duration;
            using request_type = simple_request<traits_type, root_extensions, beast_request, root_extensions>;
            using buffer_type  = boost::beast::flat_buffer;
            using app_wrapper_ref     = typename server_type::app_wrapper_ref;
            using allocator_pack_type = typename server_type::allocator_pack_type;
            using char_allocator_type = typename allocator_pack_type::template local_allocator_type<char>;
            using beast_fields_type   = boost::beast::http::basic_fields<char_allocator_type>;
            using beast_body_type     = boost::beast::http::string_body;
            using beast_response_type = boost::beast::http::response<beast_body_type, beast_fields_type>;
            using beast_response_serializer_type =
              boost::beast::http::response_serializer<beast_body_type, beast_fields_type>;
            using beast_request_type = typename request_type::beast_request_type;
            using socket_type        = asio::ip::tcp::socket;

          private:
            server_type&    server; // fixme: race condition
            socket_type     sock;
            steady_timer    timer;
            request_type    req;
            app_wrapper_ref app_ref;
            buffer_type     buf{default_buffer_size}; // fixme: see if this is using our allocator


          public:
            beast_worker(server_type& serv_ref)
              : server{serv_ref},
                sock{server.io},
                timer{server.io, server.timeout},
                req{server},
                app_ref{server.app_ref} {}

            /**
             * Running async_read_request directly in the constructor will not make
             * make_shared (or alike) functions work properly.
             */
            void start() {
                async_accept();
                check_deadline();
            }

          private:
            beast_response_type make_beast_response(beast_request_type breq, HTTPResponse auto&& res) const {
                res.calculate_default_headers();
                beast_response_type bres;
                bres.version(breq.version());
                for (auto const& h : res.headers) {
                    bres.set(h.name, h.value);
                }
                bres.body() = res.body.str();
                // bres.content_length(res.body.size());
                bres.prepare_payload();
                return bres;
            }



            // Asynchronously receive a complete request message.
            void async_read_request() noexcept {
                server.logger.info("Started reading request.");
                boost::beast::http::async_read(
                  sock,
                  buf,
                  req.beast_parser(),
                  [this](boost::beast::error_code ec, [[maybe_unused]] std::size_t bytes_transferred) {
                      if (!ec) [[likely]] {
                          server.logger.info("Recieved a request");
                          async_write_response();
                      } else {
                          server.logger.warning("Connection error.", ec);
                      }
                  });
            }


            void async_write_response() noexcept {
                const auto bres = make_beast_response(req.beast_parser().get(), app_ref(req));
                beast_response_serializer_type str_serializer{bres};
                boost::beast::http::async_write(
                  sock,
                  str_serializer,
                  [this](boost::beast::error_code ec, stl::size_t) noexcept {
                      if (ec) [[unlikely]] {
                          server.logger.warning("Write error on socket.", ec);
                      }
                      sock.shutdown(asio::ip::tcp::socket::shutdown_send, ec);
                      if (ec) [[unlikely]] {
                          server.logger.warning("Error on sending shutdown into socket.", ec);
                      }
                      timer.cancel();
                  });
            }



            void check_deadline() noexcept {
                // The deadline may have moved, so check it has really passed.
                if (timer.expiry() <= stl::chrono::steady_clock::now()) {
                    // Close socket to cancel any outstanding operation.
                    boost::beast::error_code ec;
                    sock.close(ec);
                    if (ec) [[unlikely]] {
                        server.logger.warning("Error on socket close.", ec);
                    }

                    // Sleep indefinitely until we're given a new deadline.
                    timer.expires_at((stl::chrono::steady_clock::time_point::max)());
                }

                timer.async_wait([this](boost::beast::error_code ec) {
                    if (ec) [[unlikely]] {
                        server.logger.warning("Error on timer wait.", ec);
                    }
                    check_deadline();
                });
            }



            void async_accept() noexcept {
                server.logger.info("Accepting Request");
                server.acceptor.async_accept(sock, [this](boost::beast::error_code ec) noexcept {
                    if (!ec) [[likely]] {
                        async_read_request();
                    } else {
                        server.logger.warning("Accepting error", ec);
                        this->async_accept();
                    }
                });
            }
        };


    } // namespace details




    /**
     * Beast Server
     */
    template <Traits TraitsType, typename RootExtensionsT, typename App>
    struct beast_server : public enable_traits<TraitsType> {
        using traits_type         = TraitsType;
        using etraits             = enable_traits<TraitsType>;
        using root_extensions     = RootExtensionsT;
        using steady_timer        = asio::steady_timer;
        using duration            = typename steady_timer::duration;
        using address_type        = asio::ip::address;
        using string_view_type    = traits::string_view<traits_type>;
        using port_type           = unsigned short;
        using thread_pool_type    = asio::thread_pool;
        using endpoint_type       = asio::ip::tcp::endpoint;
        using app_wrapper_ref     = stl::add_lvalue_reference_t<App>;
        using beast_server_type   = beast_server;
        using worker_type         = details::beast_worker<beast_server_type>;
        using acceptor_type       = asio::ip::tcp::acceptor;
        using socket_type         = asio::ip::tcp::socket;
        using allocator_pack_type = typename etraits::allocator_pack_type;
        using char_allocator_type =
          typename etraits::allocator_pack_type::template local_allocator_type<char>;
        using worker_allocator_type =
          typename allocator_pack_type::template local_allocator_type<worker_type>;
        using workers_type = stl::list<worker_type, worker_allocator_type>;

        // each request should finish before this
        duration timeout{stl::chrono::seconds(3)};

      private:
        friend worker_type;

        address_type     bind_address;
        port_type        bind_port = 80;
        asio::io_context io;
        acceptor_type    acceptor;
        thread_pool_type pool;
        stl::size_t      pool_count;
        workers_type     workers;
        app_wrapper_ref  app_ref;


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
            acceptor{io},
            pool{concurrency_hint - 1}, // the main thread is one thread itself
            pool_count{concurrency_hint},
            workers{this->alloc_pack.template local_allocator<worker_type>()},
            app_ref{the_app_ref} {

            // inittialize the workers
            for (stl::size_t index = 0ul; index != pool_count; ++index) {
                workers.emplace_back(*this);
            }
        }


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

            // start accepting in all workers
            for (auto& worker : workers) {
                worker.start();
            }

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
