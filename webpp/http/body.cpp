#include "body.h"
#include <fstream>

std::string_view webpp::body::variants::str(std::string_view default_val) const
    noexcept {
    if (type == types::string)
        return *static_cast<std::string*>(data);

    // FIXME: check if there's an optimization issue here or not
    if (type == types::stream) {
        auto ndata = new std::string{
            std::istreambuf_iterator<char>(*static_cast<std::istream*>(data)),
            std::istreambuf_iterator<char>()};
        this->~variants();
        data = ndata;
        type = types::string;
        return *static_cast<std::string*>(data);
    }
    return default_val;
}

void webpp::body::variants::replace_stream(std::ostream& stream) noexcept {
    replace(&stream, types::stream);
}

void webpp::body::variants::replace_string_view(std::string_view str) noexcept {
    replace(new std::string(str), types::string);
}

void webpp::body::variants::replace_string(std::string&& str) noexcept {
    replace(new std::string(std::move(str)), types::string);
}

void webpp::body::variants::replace_string(std::string const& str) noexcept {
    replace(new std::string(str), types::string);
}

void webpp::body::variants::replace_string(std::string* str) noexcept {
    replace(str, types::string);
}

void webpp::body::variants::replace(
    void* _data, webpp::body::variants::types _type) noexcept {
    this->~variants();
    data = _data;
    type = _type;
}

void webpp::body::variants::clear() noexcept {
    this->~variants();
    data = nullptr;
    type = types::empty;
}

webpp::body::variants::~variants() noexcept {
    switch (type) {
    case types::string:
        delete static_cast<std::string*>(data);
        break;
    case types::stream:
        delete static_Cast<std::istream*>(data);
    default:
        break;
    }
}
