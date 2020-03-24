
#include <boost/asio.hpp>

namespace net = boost::asio;
using socket_t = net::ip::tcp::socket;
using error_code_t = boost::system::error_code;
constexpr std::size_t buffer_size = 1024 * 1024;

