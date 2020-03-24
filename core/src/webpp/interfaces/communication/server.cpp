#include "connection.hpp"
#include <set>

namespace webpp::sserver {

  /**
   * This class is the server and the conection manager.
   */
  class server {
    private:
      std::set<connection> connections;
      net::io_context io;
      net::ip::tcp::acceptor acceptor;

      void accept() noexcept {
        acceptor.async_accept([] (error_code_t const& ec, socket_t socket) {
                // Check whether the server was stopped by a signal before this completion handler had a chance to run
                if (!acceptor.is_open()) {
                    return;
                }

                if (!ec) {
                    connections.emplace_back(std::move(socket));
                }
            });
      }

    public:


  };

}
