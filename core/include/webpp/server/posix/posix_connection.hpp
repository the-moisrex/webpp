#ifndef WEBPP_INTERFACE_COMMON_CONNECTION_H
#define WEBPP_INTERFACE_COMMON_CONNECTION_H

#include "../../platform/posix.hpp"

#ifdef webpp_posix

#    include "../../std/format.hpp"
#    include "../../traits/enable_traits.hpp"
#    include "../server_concepts.hpp"

#    include <netinet/in.h>
#    include <netinet/tcp.h>
#    include <sys/socket.h>
#    include <sys/types.h>
#    include <unistd.h>

namespace webpp::posix {

    template <typename TraitsType>
    struct posix_connection : public enable_traits<TraitsType> {
        using traits_type = TraitsType;
        using socket_type = int;
        using etraits     = enable_traits<traits_type>;

        static constexpr auto logger_category = "Posix/Connection";

      private:
        socket_type sock;

        /**
         * This function will read some bytes.
         */
        void read() noexcept {}

        bool write(auto data, stl::size_t data_size) noexcept {}

        bool write_file(auto&& header_buffer, stl::size_t header_buffer_size, auto&& file) noexcept {

            // Enable TCP_CORK option on 'sock' - subsequent TCP output is corked
            // until this option is disabled or the socket is closed
            int optval = 1; // true
            if (setsockopt(sock, IPPROTO_TCP, TCP_CORK, &optval, sizeof(optval)) == -1) {
                this->logger.error(logger_category, "Problem setting socket option TCP_CORK.", errno);
                this->done();
                return false;
            }

            // first send the header
            if (ssize_t written = ::write(sock, header_buffer, header_buffer_size);
                written != header_buffer_size) {
                // todo: handle half writes first
                return false;
            }

            // then send the file
            sendfile(sock, );
            return true;
        }

      public:
        explicit posix_connection(socket_type _sock, auto&&... args) noexcept
          : sock(_sock),
            etraits{stl::forward<decltype(args)>(args)...} {}

        void done() {
            if (close(sock) == -1) {
                this->logger.error(logger_category, "Problem with closing connection.", errno);
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
            etraits::swap(con);
        }
    };

} // namespace webpp::posix

#endif // webpp_posix

#endif // WEBPP_INTERFACE_COMMON_CONNECTION_H
