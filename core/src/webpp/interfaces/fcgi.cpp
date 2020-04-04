#include "../../../include/webpp/interfaces/fcgi.h"
#include "common/constants.hpp"
#include "common/server.hpp"

using namespace webpp;
using server = common::server;

class fcgi::fcgi_impl {
  private:
    server _server;
    fcgi* _fcgi;

    auto get_endpoints() noexcept {
        std::net::tcp::resolver resolver(server.io);
        std::error_code ec;
        auto endpoints =
            _fcgi->endpoints().empty()
                ? resolver.reslove(std::net::ip::address::from_string(
                                       default_fcgi_listen_addr),
                                   default_fcgi_listen_port)
                : resolver.resolve(address, port);
    }

  public:
    fcgi_impl(fcgi* __fcgi) noexcept : _fcgi(__fcgi) {}

    void operator()() noexcept {}
};

fcgi::fcgi() noexcept : impl(new fcgi_impl{this}) {}

void fcgi::operator()() noexcept { impl->operator()(); }
