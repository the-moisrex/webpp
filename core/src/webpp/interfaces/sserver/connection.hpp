#include "constants.hpp"
#include <memory>

/**
 * The reason that this file is here and not in the include directory is because
 * we want to hide every boost related library from the final users of this
 * framework. Hiding implementaions are good ways to do this.
 */

namespace webpp::sserver {

    class connection : public std::enable_shared_from_this<connection> {
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

    void connection::write() noexcept {
      
    }

} // namespace webpp::sserver
