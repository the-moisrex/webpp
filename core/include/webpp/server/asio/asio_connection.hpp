#ifndef WEBPP_INTERFACE_COMMON_CONNECTION_H
#define WEBPP_INTERFACE_COMMON_CONNECTION_H

#include "../../namespace.hpp"
// clang-format off
#include asio(ip/tcp)
// clang-format on

#include "./asio_constants.hpp"


namespace webpp {

    template <typename TraitsType, typename SessionType>
    struct asio_connection {
        using traits_type    = TraitsType;
        using char_type      = typename traits_type::char_type;
        using logger_type    = typename traits_type::logger_type;
        using logger_ref     = typename logger_type::logger_ref;
        using session_type   = SessionType;
        using socket_type    = asio::ip::tcp::socket;
        using allocator_type = typename traits_type::template allocator<char_type>;

      private:
        socket_type                        socket;
        [[no_unique_address]] session_type session;

        /**
         * This function will read some bytes.
         */
        void read() noexcept {
            // we share ourselves, so the connection keeps itself alive.
            socket.async_read_some(
              asio::buffer(session.buffer()),
              [this](asio::error_code const& err, stl::size_t bytes_transferred) noexcept {
                  if (!err) {
                      // we need to parse, store, read more, or write something
                      session.read(bytes_transferred);
                      read();
                  } else {
                      if (err.value() != EOF) { // todo: check if this works
                          session.logger.error(session.logger_category, "Error receiving data.", err);
                      }
                      if (!session.keep_connection()) {
                          asio::error_code ec;
                          socket.close(ec);
                          if (!ec) {
                              session.logger.error(session.logger_category,
                                                   "Problem with closing connection.", ec);
                          }
                      }
                  }
              });
        }

        /**
         * This method will kick start the reading
         */
        void start_reading() noexcept {
            // todo: fill this
        }


      public:
        explicit asio_connection(socket_type&& socket, logger_ref logger_obj = logger_type{},
                                 auto const& alloc = allocator_type{}) noexcept
          : socket(stl::move(socket)),
            session{logger_obj, alloc} {
            start_reading();
        }

        // move-able, non-copy-able
        asio_connection(asio_connection&&) noexcept = default;
        asio_connection(asio_connection const&)     = delete;
        asio_connection& operator=(asio_connection const&) = delete;
        asio_connection& operator=(asio_connection&&) = delete;
    };

} // namespace webpp

#endif // WEBPP_INTERFACE_COMMON_CONNECTION_H
