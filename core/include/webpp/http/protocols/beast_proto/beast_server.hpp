#ifndef WEBPP_HTTP_PROTO_BEAST_SERVER_HPP
#define WEBPP_HTTP_PROTO_BEAST_SERVER_HPP

#include "../../../libs/asio.hpp"
#include "../../../std/string_view.hpp"
#include "../../../traits/enable_traits.hpp"
#include "beast_session_manager.hpp"

#include <thread>

// clang-format off
#include asio_include(steady_timer)
#include asio_include(ip/address)
#include asio_include(thread_pool)
#include asio_include(post)
// clang-format on

namespace webpp::http::beast_proto {

    template <Traits TraitsType>
    struct beast_server : public enable_traits<TraitsType> {
        using traits_type      = TraitsType;
        using steady_timer     = asio::steady_timer;
        using duration         = typename steady_timer::duration;
        using address_type     = asio::ip::address;
        using string_view_type = traits::string_view<traits_type>;
        using port_type        = unsigned short;
        using thread_pool_type = asio::thread_pool;

      private:
        address_type     bind_address;
        port_type        bind_port;
        asio::io_context io;
        thread_pool_type pool;
        stl::size_t      pool_count;

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
        beast_server(stl::size_t concurrency_hint = stl::thread::hardware_concurrency())
          : io{static_cast<int>(concurrency_hint)},
            pool{concurrency_hint - 1}, // the main thread is one thread itself
            pool_count{concurrency_hint} {}


        // each request should finish before this
        duration timeout{stl::chrono::seconds(3)};

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
                asio::post(pool, [this] noexcept {
                    start_io();
                });
            }
            for (;;)
                start_io();
        }
    };

} // namespace webpp::http::beast_proto

#endif // WEBPP_HTTP_PROTO_BEAST_SERVER_HPP
