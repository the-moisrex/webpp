#ifndef WEBPP_ASIO_SESSION_HPP
#define WEBPP_ASIO_SESSION_HPP

#include "../../namespace.hpp"
// clang-format off
#include asio(ip/tcp)
// clang-format on

#include <system_error>
#include <utility>

namespace webpp {
    struct session {
      public:
        using socket_type = asio::ip::tcp::socket;

      private:
        socket_type socket;

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
        session(socket_type&& _socket) noexcept : socket(stl::move(_socket)) {}
    };

} // namespace webpp


#endif

