#include "../../../include/webpp/interfaces/fcgi.h"
#include "../../../include/webpp/std/internet.h"
#include "common/constants.hpp"
#include "common/server.hpp"

using namespace webpp;
using server = common::server;

class fcgi::fcgi_impl {
  private:
    server _server;
    fcgi* _fcgi;

    auto get_endpoints() noexcept {
        std::net::ip::tcp::resolver resolver(_server.io);
        std::error_code ec;
        std::net::ip::tcp::resolver::results_type _endpoints;
        if (_fcgi->endpoints().empty()) {
            _endpoints = resolver.resolve(
                std::net::ip::address::from_string(default_fcgi_listen_addr),
                default_fcgi_listen_port);
        } else {
            _endpoints =
        }
    }

  public:
    fcgi_impl(fcgi* __fcgi) noexcept : _fcgi(__fcgi) {}

    void operator()() noexcept {}
};

fcgi::fcgi() noexcept : impl(std::make_unique<fcgi_impl>(this)) {}

void fcgi::operator()() noexcept { impl->operator()(); }
