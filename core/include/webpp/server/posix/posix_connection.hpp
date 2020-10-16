#ifndef WEBPP_INTERFACE_COMMON_CONNECTION_H
#define WEBPP_INTERFACE_COMMON_CONNECTION_H

#include "../../std/format.hpp"
#include <sys/socket.h>

namespace webpp::posix {

    template <typename TraitsType, typename SessionType>
    struct posix_connection {
        using traits_type    = TraitsType;
        using char_type      = typename traits_type::char_type;
        using logger_type    = typename traits_type::logger_type;
        using logger_ref     = typename logger_type::logger_ref;
        using session_type   = SessionType;
        using socket_type    = int;
        using allocator_type = typename traits_type::template allocator<char_type>;

      private:
        socket_type  sock;
        session_type session;

        /**
         * This function will read some bytes.
         */
        void read() noexcept {
        }

        void write() noexcept {
            auto res = session.output();

            // todo: do something about this
            if (session.logger.enabled) {
                posix::socket_base::keep_alive keep_alive_option;
                socket.get_option(keep_alive_option);
                session.logger.info(
                  session.logger_category,
                  stl::format("Session keep alive option: {}", keep_alive_option.value() ? "true" : "false"));
            }

            if (::read(sock, session.buffer(), ) == -1) {
                      if (err.value() != EOF) { // todo: check if this works
                          session.logger.error(session.logger_category, "Error receiving data.", err);
                      }
                      if (!session.keep_connection()) {
                          posix::error_code ec;
                          socket.close(ec);
                          if (!ec) {
                              session.logger.error(session.logger_category,
                                                   "Problem with closing connection.", ec);
                          }
                      }
                  }
              }
        }

        /**
         * This method will kick start the reading
         */
        void start_reading() noexcept {
            // todo: fill this
            read();
        }

        void start_writing() noexcept {
            write();
        }

      public:
        explicit posix_connection(socket_type&& socket, logger_ref logger_obj = logger_type{},
                                 auto const& alloc = allocator_type{}) noexcept
          : socket(stl::move(socket)),
            session{logger_obj, alloc} {
            start_reading();
        }

        // move-able, non-copy-able
        posix_connection(posix_connection&&) noexcept = default;
        posix_connection(posix_connection const&)     = delete;
        posix_connection& operator=(posix_connection const&) = delete;
        posix_connection& operator=(posix_connection&&) = delete;
    };

} // namespace webpp

#endif // WEBPP_INTERFACE_COMMON_CONNECTION_H
