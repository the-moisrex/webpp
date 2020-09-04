#ifndef WEBPP_INTERFACE_COMMON_CONNECTION_H
#define WEBPP_INTERFACE_COMMON_CONNECTION_H

#include "../../namespace.hpp"
// clang-format off
#include asio(ip/tcp)
// clang-format on

#include "./asio_constants.hpp"

#include <memory>
#include <system_error>
#include <array>

namespace webpp {

    template <typename TraitsType, typename SessionType>
    struct connection {
        using traits_type    = TraitsType;
        using char_type      = typename traits_type::char_type;
        using logger_type    = typename traits_type::logger_type;
        using logger_ref     = typename logger_type::logger_ref;
        using session_type   = SessionType;
        using socket_type    = asio::ip::tcp::socket;
        using allocator_type = typename traits_type::allocator<char_type>;

      private:
        [[no_unique_address]] session_type session;
        socket_type                        socket;
        stl::array<char_type, buffer_size> buffer{}; // todo: should we use char_type here?

        void read() noexcept {
            // we share ourselves, so the connection keeps itself alive.
            socket.async_read_some(asio::buffer(buffer), [this](stl::error_code const& err,
                                                                stl::size_t bytes_transferred) noexcept {
                // we need to parse, store, read more, or write something
                session.read(err, bytes_transferred);
            });
        }

        void start_reading() noexcept {
            // todo: fill this
        }


      public:
        explicit connection(socket_type&& socket, logger_ref logger_obj = logger_type{},
                            auto const& alloc = allocator_type{}) noexcept
          : socket(stl::move(socket)),
            session{logger_obj, alloc} {

            start_reading();
        }
        connection(connection const&)     = delete;
        connection(connection&&) noexcept = default;
        connection& operator=(connection const&) = delete;
        connection& operator=(connection&&) = delete;
    };

} // namespace webpp

#endif // WEBPP_INTERFACE_COMMON_CONNECTION_H
