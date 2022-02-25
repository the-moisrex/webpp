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
#include asio_include(signal_set)
// clang-format on

#include <boost/beast/core.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/write.hpp>

namespace webpp::http::beast_proto {

    namespace details {

        template <typename ServerT>
        struct beast_worker : ServerT::etraits {
            using server_type     = ServerT;
            using etraits         = typename server_type::etraits;
            using duration        = typename server_type::duration;
            using acceptor_type   = typename server_type::acceptor_type;
            using traits_type     = typename server_type::traits_type;
            using root_extensions = typename server_type::root_extensions;
            using endpoint_type   = asio::ip::tcp::endpoint;
            using steady_timer    = asio::steady_timer;
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
            using stream_type        = boost::beast::tcp_stream;


            static constexpr auto log_cat = "BeastWorker";

          private:
            stl::optional<stream_type>                    stream;
            duration                                      timeout;
            stl::optional<request_type>                   req;
            stl::optional<beast_response_type>            bres;
            stl::optional<beast_response_serializer_type> str_serializer;
            app_wrapper_ref                               app_ref;
            buffer_type buf{default_buffer_size}; // fixme: see if this is using our allocator


          public:
            beast_worker(server_type& server)
              : etraits{server},
                timeout{server.timeout},
                req{server},
                app_ref{server.app_ref} {}

            /**
             * Running async_read_request directly in the constructor will not make
             * make_shared (or alike) functions work properly.
             */
            void start(socket_type&& in_sock) noexcept {
                stream.emplace(stl::move(in_sock));
                async_read_request();
            }

          private:
            void make_beast_response() noexcept {
                const beast_request_type breq = req->beast_parser().get();
                auto                     res  = app_ref(*req);
                bres.emplace();
                res.calculate_default_headers();
                bres->version(breq.version());
                for (auto const& h : res.headers) {
                    bres->set(h.name, h.value);
                }
                bres->body() = res.body.str();
                // bres.content_length(res.body.size());
                bres->prepare_payload();
                str_serializer.emplace(*bres);
            }


            // Asynchronously receive a complete request message.
            void async_read_request() noexcept {
                stream->expires_after(timeout);
                boost::beast::http::async_read(
                  sock,
                  buf,
                  req->beast_parser(),
                  [this](boost::beast::error_code ec, [[maybe_unused]] std::size_t bytes_transferred) {
                      if (!ec) [[likely]] {
                          async_write_response();
                      } else {
                          this->logger.warning(log_cat, "Connection error.", ec);
                      }
                  });
            }


            void async_write_response() noexcept {
                make_beast_response();
                boost::beast::http::async_write(
                  *stream,
                  *str_serializer,
                  [this](boost::beast::error_code ec, stl::size_t) noexcept {
                      if (ec) [[unlikely]] {
                          this->logger.warning(log_cat, "Write error on socket.", ec);
                      } else {
                          // todo: check if we need the else part of this condition to be an else stmt.
                          stream->socket().shutdown(asio::ip::tcp::socket::shutdown_send, ec);
                          if (ec) [[unlikely]] {
                              this->logger.warning(log_cat, "Error on sending shutdown into socket.", ec);
                          }
                      }
                      reset();
                      start();
                  });
            }


            void reset() noexcept {
                boost::beast::error_code ec;
                stream->socket().close(ec);
                if (ec) [[unlikely]] {
                    this->logger.warning(log_cat, "Error on connection closing.", ec);
                }

                // destroy the request type
                req.emplace(*this);


                str_serializer.reset();
                bres.reset();

                // Sleep indefinitely until we're given a new deadline.
                stream->expires_never();
                stream.reset();
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


        static constexpr auto log_cat = "Beast";

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
                this->logger.error(log_cat, "Error while starting io server.", err);
            } catch (...) {
                // todo: possible data race
                this->logger.error(log_cat, "Unknown server error");
            }
            // todo: try running the server again
            return -1;
        }

        void async_accept() noexcept {
            acceptor.async_accept(asio::make_strand(io),
                                  [this](boost::beast::error_code ec, socket_type sock) noexcept {
                                      if (!ec) [[likely]] {
                                          timer.expires_after(timeout);
                                          async_read_request();
                                      } else {
                                          this->logger.warning(log_cat, "Accepting error", ec);
                                          this->async_accept();
                                      }
                                  });
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
            acceptor{asio::make_strand(io)},
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
                this->logger.error(log_cat, "Cannot set address", ec);
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
                this->logger.warning(log_cat, "Cannot enable SSL");
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

            // Capture SIGINT and SIGTERM to perform a clean shutdown
            asio::signal_set signals(io, SIGINT, SIGTERM);
            signals.async_wait([this](boost::beast::error_code const&, int) {
                this->logger.info(log_cat, "Stopping the server, got a signal");

                // Stop the `io_context`. This will cause `run()`
                // to return immediately, eventually destroying the
                // `io_context` and all of the sockets in it.
                io.stop();
            });

            boost::beast::error_code ec;
            const endpoint_type      ep{bind_address, bind_port};

            // open
            acceptor.open(ep.protocol(), ec);
            if (ec) {
                this->logger.error(log_cat,
                                   fmt::format("Cannot open protocol for {}", binded_uri().to_string()),
                                   ec);
                return -1;
            }

            // Allow address reuse
            acceptor.set_option(asio::socket_base::reuse_address(true), ec);
            if (ec) {
                this->logger.error(log_cat,
                                   fmt::format("Cannot set reuse option on {}", binded_uri().to_string()),
                                   ec);
                return -1;
            }

            // bind
            acceptor.bind(ep, ec);
            if (ec) {
                this->logger.error(log_cat, fmt::format("Cannot bind to {}", binded_uri().to_string()), ec);
                return -1;
            }

            // listen
            acceptor.listen(asio::socket_base::max_listen_connections, ec);
            if (ec) {
                this->logger.error(log_cat, fmt::format("Cannot listen to {}", binded_uri().to_string()), ec);
                return -1;
            }

            // We need to be executing within a strand to perform async operations
            // on the I/O objects in this session. Although not strictly necessary
            // for single-threaded contexts, this example code is written to be
            // thread-safe by default.
            asio::dispatch(acceptor.get_executor(),
                           boost::beast::bind_front_handler(&listener::async_accept, this));

            // start accepting in all workers
            for (auto& worker : workers) {
                worker.start();
            }

            this->logger.info(log_cat, fmt::format("Starting beast server on {}", binded_uri().to_string()));
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
