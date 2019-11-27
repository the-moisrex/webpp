#include "body.h"
#include <fstream>

std::string_view webpp::body::str(std::string_view default_val) const noexcept {
    if (type == types::string)
        return *static_cast<std::string*>(data);

    // FIXME: check if there's an optimization issue here or not
    if (type == types::stream) {
        auto ndata = new std::string{
            std::istreambuf_iterator<char>(*static_cast<std::istream*>(data)),
            std::istreambuf_iterator<char>()};
        this->~body();
        data = ndata;
        type = types::string;
        return *static_cast<std::string*>(data);
    }
    return default_val;
}

std::ostream& webpp::body::operator<<(std::ostream& __stream) {
    switch (type) {
    case types::stream:
        __stream << static_cast<stream_type*>(data);
        break;
    case types::string:
        __stream << *static_cast<string_type*>(data);
        break;
    default:
        // do nothing
        break;
    }

    return __stream;
}

void webpp::body::replace_stream(std::ostream& stream) noexcept {
    replace(&stream, types::stream);
}

void webpp::body::replace_string_view(std::string_view str) noexcept {
    replace(new std::string(str), types::string);
}

void webpp::body::replace_string(std::string&& str) noexcept {
    replace(new std::string(std::move(str)), types::string);
}

void webpp::body::replace_string(std::string const& str) noexcept {
    replace(new std::string(str), types::string);
}

void webpp::body::replace_string(std::string* str) noexcept {
    replace(str, types::string);
}

void webpp::body::replace(void* _data, webpp::body::types _type) noexcept {
    this->~body();
    data = _data;
    type = _type;
}

void webpp::body::clear() noexcept {
    this->~body();
    data = nullptr;
    type = types::empty;
}

webpp::body::~body() noexcept {
    switch (type) {
    case types::string:
        delete static_cast<std::string*>(data);
        break;
    case types::stream:
        delete static_cast<std::istream*>(data);
        break;
    default:
        break;
    }
}
