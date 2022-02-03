#ifndef WEBPP_HTTP_PROTO_BEAST_SERVER_HPP
#define WEBPP_HTTP_PROTO_BEAST_SERVER_HPP

#include "../../../libs/asio.hpp"
#include "beast_session_manager.hpp"

namespace webpp::http::beast_proto {

    template <Traits TraitsType>
    struct beast_server {
        using traits_type  = TraitsType;
        using steady_timer = asio::steady_timer;
        using duration     = typename steady_timer::duration;

      private:
      public:
        // each request should finish before this
        duration timeout{stl::chrono::seconds(3)};


        // run the server
        int operator()() noexcept {
            try {
                //
                return 0;
            } catch (...) {
                logger.error("Unknown error");
                return -1;
            }
        }
    };

} // namespace webpp::http::beast_proto

#endif // WEBPP_HTTP_PROTO_BEAST_SERVER_HPP
