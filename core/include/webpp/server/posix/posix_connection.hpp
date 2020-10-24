#ifndef WEBPP_INTERFACE_COMMON_CONNECTION_H
#define WEBPP_INTERFACE_COMMON_CONNECTION_H

#include "../../std/format.hpp"
#include "../../platform/posix.hpp"
#include "../server_concepts.hpp"
#ifdef webpp_posix

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <unistd.h>

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

            switch (session.output_source_type()) {
                case session_output_source::memory: {

                } break;

                    // we can send a file faster
                case session_output_source::file: {

                    // Enable TCP_CORK option on 'sock' - subsequent TCP output is corked
                    // until this option is disabled or the socket is closed
                    int optval = 1; // true
                    if (setsockopt(sock, IPPROTO_TCP, TCP_CORK, &optval, sizeof(optval)) == -1) {
                        session.logger.error(session.logger_category, "Problem setting socket option TCP_CORK.", errno);
                        this->done();
                        return;
                    }

                    // first send the header
                    if (::write(sock, session.buffer() ) == -1) {
                        if (!session.keep_connection()) {
                            this->done();
                        }
                    }

                    // then send the file
                    sendfile(sock, );
                } break;
            }

        }

      public:
        explicit posix_connection(socket_type _sock, auto&&...args) noexcept
          : sock(_sock),
            session{stl::forward<decltype(args)>(args)...} {
        }

        void done() {
            if (close(sock) == -1) {
                session.logger.error(session.logger_category, "Problem with closing connection.", errno);
            }
            // todo: remove the connection from the connection list
        }

        // move-able, non-copy-able
        posix_connection(posix_connection&&) noexcept = default;
        posix_connection(posix_connection const&)     = delete;
        posix_connection& operator=(posix_connection const&) = delete;
        posix_connection& operator=(posix_connection&&) = delete;

        void swap(posix_connection& con) noexcept {
            using stl::swap;
            swap(sock, con.sock);
            swap(session, con.session);
        }
    };

} // namespace webpp

#endif // webpp_posix

#endif // WEBPP_INTERFACE_COMMON_CONNECTION_H
