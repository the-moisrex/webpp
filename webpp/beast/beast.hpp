// Created by moisrex on 11/7/20.

#ifndef WEBPP_BEAST_HPP
#define WEBPP_BEAST_HPP

#include "../http/protocol/common_http_protocol.hpp"
#include "../http/response.hpp"
#include "../std/string_view.hpp"
#include "beast_body_communicator.hpp"
#include "beast_server.hpp"

namespace webpp {

    /**
     * Beast Server
     */
    template <Application App, Traits TraitsType = default_traits>
    struct beast : http::common_http_protocol<TraitsType, App> {
        using application_type             = stl::remove_cvref_t<App>;
        using traits_type                  = TraitsType;
        using common_http_protocol_type    = http::common_http_protocol<TraitsType, App>;
        using etraits                      = typename common_http_protocol_type::etraits;
        using protocol_type                = beast<application_type, traits_type>;
        using duration                     = typename stl::chrono::steady_clock::duration;
        using address_type                 = asio::ip::address;
        using string_type                  = traits::string<traits_type>;
        using string_view_type             = traits::string_view<traits_type>;
        using port_type                    = stl::uint16_t;
        using endpoint_type                = asio::ip::tcp::endpoint;
        using acceptor_type                = asio::ip::tcp::acceptor;
        using socket_type                  = asio::ip::tcp::socket;
        using thread_worker_type           = beast_proto::thread_worker<protocol_type>;
        using http_worker_type             = beast_proto::http_worker<protocol_type>;
        using char_allocator_type          = traits::allocator_type_of<traits_type>;
        using thread_worker_allocator_type = traits::allocator_type_of<traits_type, thread_worker_type>;
        using thread_pool_type             = asio::thread_pool;
        using char_type                    = traits::char_type<traits_type>;
        using fields_allocator_type        = traits::allocator_type_of<traits_type, char_type>;
        using fields_provider              = http::header_fields_provider<http::header_field_of<traits_type>>;
        using request_body_communicator    = beast_proto::beast_request_body_communicator<protocol_type>;
        using request_headers_type         = http::request_headers<fields_provider>;
        using request_body_type            = http::request_body<traits_type, request_body_communicator>;
        using request_type =
          http::simple_request<beast_proto::beast_request, request_headers_type, request_body_type>;
        using response_type = http::simple_response<traits_type>;


        static constexpr auto        log_cat                   = "Beast";
        static constexpr port_type   default_http_port         = 80U;
        static constexpr port_type   default_https_port        = 443U;
        static constexpr stl::size_t default_http_worker_count = 20;

      private:
        using super = http::common_http_protocol<TraitsType, App>;

        friend http_worker_type;
        friend thread_worker_type;

        address_type       bind_address;
        port_type          bind_port = default_http_port;
        asio::io_context   io{static_cast<int>(stl::thread::hardware_concurrency())};
        acceptor_type      acceptor;
        stl::size_t        http_worker_count{default_http_worker_count};
        stl::size_t        thread_worker_count{stl::thread::hardware_concurrency()};
        thread_pool_type   pool{stl::thread::hardware_concurrency() - 1}; // there's a main thread too
        thread_worker_type thread_workers;
        stl::mutex         app_call_mutex;
        bool               synced = false;

        // each request should finish before this
        duration timeout_val{stl::chrono::seconds(3)};

        void async_accept() noexcept {
            acceptor.async_accept(asio::make_strand(io),
                                  [this](boost::beast::error_code ec, socket_type sock) {
                                      if (!ec) [[likely]] {
                                          // todo: start_work may throw errors, deal with them
                                          thread_workers.start_work(stl::move(sock));
                                      } else [[unlikely]] {
                                          this->logger.warning(log_cat, "Accepting error", ec);
                                      }
                                      this->async_accept();
                                  });
        }

        // call the app
        http::HTTPResponse auto call_app(request_type& req) noexcept {
            if (synced) {
                stl::scoped_lock lock{app_call_mutex};
                return stl::invoke(this->app, req);
            }
            return stl::invoke(this->app, req);
        }

        template <typename>
        friend struct http_worker;


      public:
        beast(beast const&)            = delete;
        beast(beast&&)                 = delete;
        beast& operator=(beast const&) = delete;
        beast& operator=(beast&&)      = delete;
        ~beast()                       = default;

        template <typename... Args>
        explicit beast(Args&&... args)
          : super{stl::forward<Args>(args)...},
            acceptor{asio::make_strand(io)},
            thread_workers{*this} {}

        beast& address(string_view_type addr) noexcept {
            asio::error_code err;
            bind_address = asio::ip::make_address(istl::to_std_string_view(addr), err);
            if (err) {
                this->logger.error(log_cat, "Cannot set address", err);
            }
            return *this;
        }

        beast& port(port_type const inp_port) noexcept {
            bind_port = inp_port;
            return *this;
        }

        beast& timeout(duration const dur) noexcept {
            timeout_val = dur;
            return *this;
        }

        [[nodiscard]] duration timeout() const noexcept {
            return timeout_val;
        }

        beast& set_worker_count(stl::size_t const val) {
            http_worker_count = val;
            return *this;
        }

        [[nodiscard]] bool is_ssl_active() const noexcept {
            return false;
        }

        [[nodiscard]] static constexpr bool is_ssl_available() noexcept {
            return false;
        }

        beast& enable_ssl() noexcept {
            if constexpr (is_ssl_available()) {
                if (!is_ssl_active()) {
                    // todo
                }

                // port mapping
                if (bind_port == default_http_port) {
                    bind_port = default_https_port;
                }

            } else {
                this->logger.warning(log_cat, "Cannot enable SSL");
            }
            return *this;
        }

        beast& disable_ssl() noexcept {
            if constexpr (is_ssl_available()) {
                if (is_ssl_active()) {
                    // todo
                }

                // port mapping
                if (bind_port == default_https_port) {
                    bind_port = default_http_port;
                }
            }
            return *this;
        }

        beast& enable_sync() noexcept {
            synced = true;
            return *this;
        }

        beast& disable_sync() noexcept {
            synced = false;
            return *this;
        }

        [[nodiscard]] auto bound_uri() {
            uri::basic_uri<string_type> res_url{get_alloc_for<string_type>(*this)};
            res_url.scheme = is_ssl_active() ? "https" : "http";
            res_url.host   = bind_address.to_string();
            res_url.port   = bind_port;
            return res_url;
        }

        [[nodiscard]] constexpr string_view_type server_name() const noexcept {
            return "Beast";
        }

        // run the server
        [[nodiscard]] int operator()() noexcept {
            // Capture SIGINT and SIGTERM to perform a clean shutdown
            asio::signal_set signals(io, SIGINT, SIGTERM);
            signals.async_wait([this](boost::beast::error_code const&, int) {
                this->logger.info(log_cat, "Stopping the server, got a signal");

                // Stop the `io_context`. This will cause `run()`
                // to return immediately, eventually destroying the
                // `io_context` and all the sockets in it.
                io.stop();
                thread_workers.stop();
                pool.stop();
            });

            boost::beast::error_code err;
            endpoint_type const      endp{bind_address, bind_port};

            // open
            acceptor.open(endp.protocol(), err);
            if (err) {
                this->logger.error(log_cat,
                                   fmt::format("Cannot open protocol for {}", bound_uri().to_string()),
                                   err);
                return -1;
            }

            // Allow address reuse
            acceptor.set_option(asio::socket_base::reuse_address(true), err);
            if (err) {
                this->logger.error(log_cat,
                                   fmt::format("Cannot set reuse option on {}", bound_uri().to_string()),
                                   err);
                return -1;
            }

            // bind
            acceptor.bind(endp, err);
            if (err) {
                this->logger.error(log_cat, fmt::format("Cannot bind to {}", bound_uri().to_string()), err);
                return -1;
            }

            // listen
            acceptor.listen(asio::socket_base::max_listen_connections, err);
            if (err) {
                this->logger.error(log_cat, fmt::format("Cannot listen to {}", bound_uri().to_string()), err);
                return -1;
            }

            // We need to be executing within a strand to perform async operations
            // on the I/O objects in this session.
            asio::dispatch(acceptor.get_executor(),
                           boost::beast::bind_front_handler(&protocol_type::async_accept, this));

            this->logger.info(log_cat,
                              fmt::format("Starting beast server on {} with {} thread workers.",
                                          bound_uri().to_string(),
                                          thread_worker_count));

            auto get_thread = [this](stl::size_t index) noexcept {
                return [this, io_index = index, tries = 0UL]() mutable noexcept {
                    for (; !io.stopped(); ++tries) {
                        try {
                            // run executor in this thread
                            io.run();
                            this->logger.info(log_cat,
                                              fmt::format("Thread {} went down peacefully.", io_index));
                        } catch (stl::exception const& err) {
                            this->logger.error(
                              log_cat,
                              fmt::format("Error while starting io server; restarting io "
                                          "runner; io runner id: {}; tries: {}",
                                          io_index,
                                          tries),
                              err);
                        } catch (...) {
                            // todo: possible data race
                            this->logger.error(
                              log_cat,
                              fmt::format(
                                "Unknown server error; restarting io runner; io runner id: {}; tries: {}",
                                io_index,
                                tries));
                        }
                    }
                };
            };


            // start accepting in all workers
            for (stl::size_t i = 1UL; i != thread_worker_count - 1; ++i) {
                asio::post(pool, get_thread(i));
            }

            get_thread(0)();

            pool.attach();
            this->logger.info(log_cat, "Server is down.");
            return 0;
        }
    };

} // namespace webpp

#endif // WEBPP_BEAST_HPP
