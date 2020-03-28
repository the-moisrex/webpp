#include "../../../include/webpp/interfaces/fcgi.h"
#include "communication/server.hpp"

using namespace webpp;
using server = sserver::server;

class fcgi::fcgi_impl {
  private:
    server _server;
    fcgi* _fcgi;

  public:
    fcgi_impl(fcgi* __fcgi) noexcept : _fcgi(__fcgi) {}

    void operator()() noexcept {
        if (_fcgi->endpoints().empty()) {
        } else {
        }
    }
};

fcgi::fcgi() noexcept : impl(new fcgi_impl{this}) {}

void fcgi::operator()() noexcept { impl->operator()(); }
