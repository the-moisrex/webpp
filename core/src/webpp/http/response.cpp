#include "webpp/http/response.h"

using namespace webpp;

response &response::operator<<(std::string_view str) noexcept {
    body << str;
    return *this;
}

void response::calculate_default_headers() noexcept {
  // It's C++20
  if (!header.contains("Content-Type"))
    header.emplace("Content-Type", "text/html");

  if (!header.contains("Content-Length"))
    header.emplace("Content-Length", std::to_string(body.str().size() * sizeof(char)));
}



