#ifndef WEBPP_INTERFACES_COMMON_SERVER_H
#define WEBPP_INTERFACES_COMMON_SERVER_H
#include "connection.hpp"
#include <memory>
#include <set>

namespace webpp::sserver {

    /**
     * This class is the server and the conection manager.
     */
    class server : public std::enable_shared_from_this<server> {
      private:
        std::set<connection> connections;
        net::io_context io;
        net::ip::tcp::acceptor acceptor;
        boost::asio::signal_set signals;

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
        server() noexcept
            : acceptor(io, endpoints), signals(io, SIGINT, SIGTERM){};
        void run() noexcept { io.run(); }
    };

} // namespace webpp::sserver

#endif // WEBPP_INTERFACES_COMMON_SERVER_H