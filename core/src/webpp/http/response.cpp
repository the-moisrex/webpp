#include "../../../include/webpp/http/response.h"

#include <fstream>

using namespace webpp;

response& response::operator<<(std::string_view str) noexcept {
    body << str;
    return *this;
}

void response::calculate_default_headers() noexcept {
    // todo: use C++20 header.contains instead when possible
    if (header.find("Content-Type") == header.cend())
        header.emplace("Content-Type", "text/html; charset=utf-8");

    if (header.find("Content-Length") == header.cend())
        header.emplace("Content-Length",
                       std::to_string(body.str().size() * sizeof(char)));
}

response::response(response const& res) noexcept {
    body   = res.body;
    header = res.header;
}

response::response(response&& res) noexcept {
    if (this != &res) {
        body   = std::move(res.body);
        header = std::move(res.header);
    }
}

response& response::operator=(response&& res) noexcept {
    if (&res != this) {
        body   = std::move(res.body);
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

response::response(std::string const& b) noexcept : body{b} {
}
response::response(std::string&& b) noexcept : body{std::move(b)} {
}

[[nodiscard]] bool response::operator==(response const& res) const noexcept {
    return body == res.body && header == res.header;
}

[[nodiscard]] bool response::operator!=(response const& res) const noexcept {
    return !operator==(res);
}

response::operator std::string_view() const noexcept {
    return body.str();
}

response::operator std::string() const noexcept {
    return std::string(body.str());
}

#ifdef CONFIG_FILE
#    if CONFIG_FILE != ""
#        include CONFIG_FILE
#    else
extern std::string_view get_static_file(std::string_view const&) noexcept;
#    endif
#endif

response response::file(std::filesystem::path const& _file) noexcept {
    response res;
#ifdef CONFIG_FILE
    if (auto content = ::get_static_file(filepath); !content.empty()) {
        res.body.replace_string(content);
        return res;
    }
#endif

    // TODO: performance tests
    // TODO: change the replace_string with replace_string_view if the file is
    // cached

    if (std::ifstream in{_file.c_str(), std::ios::binary | std::ios::ate}) {
        // details on this matter:
        // https://stackoverflow.com/questions/11563963/writing-a-binary-file-in-c-very-fast/39097696#39097696
        // std::unique_ptr<char[]> buffer{new char[buffer_size]};
        // in.rdbuf()->pubsetbuf(buffer.get(), buffer_size); // speed boost, I
        // think
        auto                    size = in.tellg();
        std::unique_ptr<char[]> result(new char[size]);
        in.seekg(0);
        in.read(result.get(), size);
        res.body.replace_string(std::string{
          result.get(), static_cast<std::string_view::size_type>(size)});
    }

    return res;
}
