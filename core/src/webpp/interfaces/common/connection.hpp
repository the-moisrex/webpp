#ifndef WEBPP_INTERFACE_COMMON_CONNECTION_H
#define WEBPP_INTERFACE_COMMON_CONNECTION_H

#include "../../../include/webpp/interfaces/common/endpoints.h"
#include "constants.hpp"
#include <memory>
#include "../../../include/webpp/common/meta.h"

#ifdef CXX20
#include <experimental/net>
#include <experimental/socket>
#include <experimental/io_context>
#include <experimental/internet>
namespace net = std::experimental::net;
#else
#error "We don't have access to networking TS."
#endif


/**
 * The reason that this file is here and not in the include directory is because
 * we want to hide every boost related library from the final users of this
 * framework. Hiding implementations are good ways to do this.
 */

namespace webpp::common {

    class connection {
    public:
        using socket_t = net::ip::tcp::socket;

    private:
        socket_t socket;
        std::array<char, buffer_size> buffer;

        void read() noexcept;

        void write() noexcept;

    public:
        connection(socket_t socket) noexcept : socket(std::move(socket)) {}
        connection(connection const&) = delete;
        connection& operator=(connection const&) = delete;

        /**
         * Start async operations
         */
        void start() noexcept;

        /**
         * We're shutting down everything, kepp up!
         */
        void stop() noexcept;
    };

    void connection::read() noexcept {
        // we share ourselves, so the connection keeps itself alive.
        auto self{shared_from_this()};
        socket.async_read_some(
            net::buffer(buffer),
            [this, self](error_code_t const& err,
                         std::size_t bytes_transferred) noexcept {
                if (!err) {
                    // we need to parse, store, read more, or write something
                }
            });
    }

    void connection::write() noexcept {}

} // namespace webpp::common

#endif // WEBPP_INTERFACE_COMMON_CONNECTION_H
