#include "response.h"

using namespace webpp;

response &response::operator<<(std::string_view str) noexcept {
    body << str;
    return *this;
}
