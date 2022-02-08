#ifndef WEBPP_HTTP_PROTO_BEAST_SERVER_HPP
#define WEBPP_HTTP_PROTO_BEAST_SERVER_HPP

#include "../../../configs/constants.hpp"
#include "../../../libs/asio.hpp"
#include "../../../std/string_view.hpp"
#include "../../../traits/enable_traits.hpp"
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

        template <Traits TraitsType, typename RootExtensionsT>
        struct beast_session : stl::enable_shared_from_this<beast_session<TraitsType, RootExtensionsT>> {
            using traits_type     = TraitsType;
            using root_extensions = RootExtensionsT;
            using acceptor_type   = asio::ip::tcp::acceptor;
            using socket_type     = asio::ip::tcp::socket;
            using endpoint_type   = asio::ip::tcp::endpoint;
            using steady_timer    = asio::steady_timer;
            using duration        = typename steady_timer::duration;
            using request_type = simple_request<traits_type, root_extensions, beast_request, root_extensions>;
            using buffer_type  = boost::beast::flat_buffer;

          private:
            socket_type   sock;
            acceptor_type acceptor;
            steady_timer  timer;
            request_type  req;
            buffer_type   buf{default_buffer_size}; // fixme: see if this is using our allocator


          public:
            beast_session(asio::io_context& io, endpoint_type const& ep, duration const& dur)
              : sock{io},
                acceptor{io, ep},
                timer{io, dur} {}



            // Asynchronously receive a complete request message.
            void async_read_request() {
                boost::beast::http::async_read(
                  sock,
                  buf,
                  req,
                  [self = this->shared_from_this()](boost::beast::error_code     ec,
                                                    [[maybe_unused]] std::size_t bytes_transferred) {
                      if (!ec)
                          self->process_request();
                  });
            }
        };

    } // namespace details

    template <Traits TraitsType, typename RootExtensionsT>
    struct beast_server : public enable_traits<TraitsType> {
        using traits_type      = TraitsType;
        using root_extensions  = RootExtensionsT;
        using steady_timer     = asio::steady_timer;
        using duration         = typename steady_timer::duration;
        using address_type     = asio::ip::address;
        using string_view_type = traits::string_view<traits_type>;
        using port_type        = unsigned short;
        using thread_pool_type = asio::thread_pool;
        using session_type     = details::beast_session<traits_type, root_extensions>;
        using endpoint_type    = asio::ip::tcp::endpoint;
        using etraits          = enable_traits<traits_type>;

      private:
        address_type     bind_address;
        port_type        bind_port;
        asio::io_context io;
        thread_pool_type pool;
        stl::size_t      pool_count;

        void accept() {
            stl::allocate_shared<session_type>(this->allocs.template general_allocator<session_type>(),
                                               io,
                                               endpoint_type{bind_address, bind_port},
                                               timeout);
        }

        int start_io() noexcept {
            try {
                io.run();
                return 0;
            } catch (stl::exception const& err) {
                this->logger.fatal("Error while starting io server.", err);
                return -1;
            } catch (...) {
                // todo: possible data race
                this->logger.fatal("Unknown server error");

                // todo: try running the server again
                return -1;
            }
        }

      public:
        // each request should finish before this
        duration timeout{stl::chrono::seconds(3)};


        template <typename ET>
        requires(EnabledTraits<stl::remove_cvref_t<ET>>)
          beast_server(ET&& et, stl::size_t concurrency_hint = stl::thread::hardware_concurrency())
          : etraits{stl::forward<ET>(et)},
            io{static_cast<int>(concurrency_hint)},
            pool{concurrency_hint - 1}, // the main thread is one thread itself
            pool_count{concurrency_hint} {}


        beast_server& address(string_view_type addr) noexcept {
            asio::error_code ec;
            bind_address = asio::ip::make_address(to_std_string_view(addr), ec);
            if (ec) {
                this->logger.fatal("Cannot set address", ec);
            }
            return *this;
        }


        beast_server& port(port_type p) noexcept {
            bind_port = p;
            return *this;
        }

        beast_server& post();
        beast_server& defer();

        // run the server
        [[nodiscard]] int operator()() noexcept {
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
