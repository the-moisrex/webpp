#include "../../../include/webpp/interfaces/fcgi.h"

// TODO: split this, I just don't feel like look it up in the documentations
// right now :)
#include <boost/asio.hpp>

using namespace webpp;

// we will be using std::net (or something like that) in a few years, so let's
// not go crazy here
using namespace boost;
using namespace boost::asio;


class fcgi::fcgi_impl {
  private:
    fcgi* _fcgi;
    io_context io;
    ip::tcp::acceptor _acceptor;

  public:
    fcgi_impl(fcgi* __fcgi) noexcept
        : _fcgi(__fcgi),
          io(), // I could suggest the number of concurrent threads here
          _acceptor(io) {}

    void operator()() noexcept {
        ip::tcp::resolver resolver(io);
        if (_fcgi->endpoints().empty()) {

        } else {
        }
    }
};

fcgi::fcgi() noexcept : impl(new fcgi_impl{this}) {}

void fcgi::operator()() noexcept { impl->operator()(); }
