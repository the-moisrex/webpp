#include "../../../include/webpp/http/response.h"

using namespace webpp;

response& response::operator<<(std::string_view str) noexcept {
    body << str;
    return *this;
}

void response::calculate_default_headers() noexcept {
    // It's C++20
    if (!header.contains("Content-Type"))
        header.emplace("Content-Type", "text/html; charset=utf-8");

    if (!header.contains("Content-Length"))
        header.emplace("Content-Length",
                       std::to_string(body.str().size() * sizeof(char)));
}

response::response(response const& res) noexcept {
    body = res.body;
    header = res.header;
}

response::response(response&& res) noexcept {
    if (this != &res) {
        body = std::move(res.body);
        header = std::move(res.header);
    }
}

response& response::operator=(response&& res) noexcept {
    if (&res != this) {
        body = std::move(res.body);
        header = std::move(res.header);
    }
    return *this;
}

response& response::operator=(std::string const& str) noexcept {
    body.replace_string(str);
    return *this;
}

response& response::operator=(std::string& str) noexcept {
    body.replace_string(std::move(str));
    return *this;
}

response::response(std::string const& b) noexcept : body{b} {}

response::response(std::string&& b) noexcept : body{std::move(b)} {}

[[nodiscard]] bool response::operator==(response const& res) const noexcept {
    return body == res.body && header == res.header;
}

[[nodiscard]] bool response::operator!=(response const& res) const noexcept {
    return !operator==(res);
}

response::operator std::string_view() const noexcept { return body.str(); }

response::operator std::string() const noexcept {
    return std::string(body.str());
}
