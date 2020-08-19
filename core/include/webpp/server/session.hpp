#ifndef WEBPP_SESSION_HPP
#define WEBPP_SESSION_HPP

#include "../std/internet.hpp"
#include "../std/socket.hpp"

#include <system_error>
#include <utility>

namespace webpp {
    class session {
      public:
        using socket_t = stl::net::ip::tcp::socket;

      private:
        socket_t _socket;

        /**
         * This function will read the data and pass it to read_callback to handle
         * those data
         */
        void read() {}

        void read_callback(stl::error_code const&, stl::size_t length) noexcept;

        /**
         * This method will write the data and calls the write_callback to handle
         * the next steps
         */
        void write();
        void write_callback(stl::error_code const&, stl::size_t length) noexcept;

      public:
        session(socket_t&& _socket) noexcept : _socket(stl::move(_socket)) {}
    };

} // namespace webpp


#endif
