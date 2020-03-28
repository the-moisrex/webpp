#include "../../../include/webpp/interfaces/fcgi.h"
#include "communication/server.hpp"

using namespace webpp;
using server = common::server;

class fcgi::fcgi_impl {
  private:
    server _server;
    fcgi* _fcgi;

    auto get_endpoints() noexcept {
        if (_fcgi->endpoints().empty()) {
        } else {
            net::tcp::resolver resolver(io_context);
            net::ip::tcp::endpoint endpoint =
                *resolver.resolve(address, port).begin();
        }
    }

  public:
    fcgi_impl(fcgi* __fcgi) noexcept : _fcgi(__fcgi) {}

    void operator()() noexcept {}
};

fcgi::fcgi() noexcept : impl(new fcgi_impl{this}) {}

void fcgi::operator()() noexcept { impl->operator()(); }
