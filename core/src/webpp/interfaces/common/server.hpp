#ifndef WEBPP_INTERFACES_COMMON_SERVER_H
#define WEBPP_INTERFACES_COMMON_SERVER_H
#include "../../../../include/webpp/common/endpoints.h"
#include "connection.hpp"
#include "constants.hpp"
#include <memory>
#include <set>

namespace webpp::common {

    /**
     * This class is the server and the conection manager.
     */
    class server : public std::enable_shared_from_this<server> {
      private:
        std::set<connection> connections;
        net::io_context io;
        net::ip::tcp::acceptor acceptor;
        net::signal_set signals{io, SIGINT, SIGTERM};
        std::error_code ec;

        void accept() noexcept {
            auto self{shared_from_this()};
            acceptor.async_accept(
                [this, self](error_code_t const& ec, socket_t socket) {
                    // Check whether the server was stopped by a signal before
                    // this completion handler had a chance to run
                    if (!acceptor.is_open()) {
                        return;
                    }

                    if (!ec) {
                        connections.emplace(std::move(socket));
                    } else {
                        // TODO: log
                    }

                    accept();
                });
        }

        void set_signals() noexcept {
            signals.async_wait([&](auto, auto) { io.stop(); });
        }

      public:
        server(net::ip::tcp::endpoints endpoints) noexcept
            : acceptor(io, endpoints){};
        void run() noexcept { io.run(ec); }
    };

} // namespace webpp::common

#endif // WEBPP_INTERFACES_COMMON_SERVER_H