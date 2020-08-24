#ifndef WEBPP_INTERFACE_COMMON_CONNECTION_H
#define WEBPP_INTERFACE_COMMON_CONNECTION_H

#include "../../namespace.hpp"
// clang-format off
#include asio(ip/tcp)
// clang-format on

#include "./asio_constants.hpp"

#include <memory>
#include <system_error>
#include <array>

namespace webpp {

    template <typename SessionType>
    struct connection {
        using session_type = SessionType;
        using socket_type  = asio::ip::tcp::socket;

      private:
        [[no_unique_address]] session_type session;
        socket_type                        socket;
        stl::array<char, buffer_size>      buffer{};

        void read() noexcept {
            // we share ourselves, so the connection keeps itself alive.
            socket.async_read_some(asio::buffer(buffer), [this](stl::error_code const& err,
                                                                stl::size_t bytes_transferred) noexcept {
                // we need to parse, store, read more, or write something
                session.read(err, bytes_transferred);
            });
        }


      public:
        explicit connection(socket_type&& socket) noexcept : socket(stl::move(socket)) {}
        connection(connection const&) = delete;
        connection(connection&&)  noexcept    = default;
        connection& operator=(connection const&) = delete;
        connection& operator=(connection&&) = default;

        /**
         * Start async operations
         */
        void start() noexcept;

        /**
         * We're shutting down everything, keep up!
         */
        void stop() noexcept;
    };

} // namespace webpp

#endif // WEBPP_INTERFACE_COMMON_CONNECTION_H
