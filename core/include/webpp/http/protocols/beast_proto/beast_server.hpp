#ifndef WEBPP_HTTP_PROTO_BEAST_SERVER_HPP
#define WEBPP_HTTP_PROTO_BEAST_SERVER_HPP

#include "../../../libs/asio.hpp"
#include "../../../std/string_view.hpp"
#include "../../../traits/enable_traits.hpp"
#include "beast_session_manager.hpp"

// clang-format off
#include asio_include(steady_timer)
#include asio_include(ip/address)
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

      private:
        address_type        bind_address;
        port_type           bind_port;
        asio::io_context    io;
        thread_manager_type th_man;
        bool                stop_server_flag = false;

        int start_io() noexcept {
            if (stop_server_flag)
                return 0;
            try {
                io.run();
                return 0;
            } catch (...) {
                // todo: possible data race
                this->logger.fatal("Unknown server error");

                // todo: try running the server again
                return -1;
            }
        }

      public:
        beast_server(int concurrency_hint) : io{concurrency_hint} {}
        beast_server() : io{stl::thread::hardware_concurrency()} {}


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
            for (;;)
                start_io();
            th_man.run([this] noexcept {
                return start_io();
            });
        }
    };

} // namespace webpp::http::beast_proto

#endif // WEBPP_HTTP_PROTO_BEAST_SERVER_HPP
