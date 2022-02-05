#ifndef WEBPP_HTTP_PROTO_BEAST_SERVER_HPP
#define WEBPP_HTTP_PROTO_BEAST_SERVER_HPP

#include "../../../libs/asio.hpp"
#include "../../../std/string_view.hpp"
#include "../../../traits/enable_traits.hpp"
#include "beast_session_manager.hpp"

// clang-format off
#include asio_include(steady_timer)
#include asio_include(ip/address)
// clang-format on

namespace webpp::http::beast_proto {

    template <Traits TraitsType>
    struct beast_server : public enable_traits<TraitsType> {
        using traits_type      = TraitsType;
        using steady_timer     = asio::steady_timer;
        using duration         = typename steady_timer::duration;
        using address_type     = asio::ip::address;
        using string_view_type = traits::string_view<traits_type>;

      private:
        address_type bind_address;

      public:
        // each request should finish before this
        duration timeout{stl::chrono::seconds(3)};

        beast_server& address(string_view_type addr) noexcept {
            asio::error_code ec;
            bind_address = asio::ip::make_address(to_std_string_view(addr), ec);
            if (ec) {
                this->logger.fatal("Cannot set address", ec);
            }
            return *this;
        }



        // run the server
        [[nodiscard]] int operator()() noexcept {
            try {
                //
                return 0;
            } catch (...) {
                this->logger.fatal("Unknown error");
                return -1;
            }
        }
    };

} // namespace webpp::http::beast_proto

#endif // WEBPP_HTTP_PROTO_BEAST_SERVER_HPP
