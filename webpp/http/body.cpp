#include "body.h"
#include <sstream>

using namespace webpp;

std::string_view body::str(std::string_view const &default_val) const noexcept {
    if (type == types::string)
        return *static_cast<std::string *>(data);

    // FIXME: check if there's an optimization issue here or not
    if (type == types::stream) {
        auto ndata = new std::string{
                std::istreambuf_iterator<char>(*static_cast<std::istream *>(data)),
                std::istreambuf_iterator<char>()};
        this->~body();
        data = ndata;
        type = types::string;
        return std::string_view{str_ref()};
    }
    return default_val;
}

std::ostream &body::operator<<(std::ostream &__stream) {
    switch (type) {
        case types::stream:
            __stream << static_cast<stream_type *>(data);
            break;
        case types::string:
            __stream << str_ref();
            break;
        default:
            // do nothing
            break;
    }

    return __stream;
}

void body::replace_stream(body::stream_type &stream) noexcept {
    replace(&stream, types::stream);
}

void body::replace_string_view(std::string_view const &str) noexcept {
    replace(new std::string(str), types::string);
}

void body::replace_string(std::string &&str) noexcept {
    replace(new std::string(std::move(str)), types::string);
}

void body::replace_string(std::string const &str) noexcept {
    replace(new std::string(str), types::string);
}

void body::replace_string(std::string *str) noexcept {
    replace(str, types::string);
}

void body::replace(void *_data, body::types _type) noexcept {
    this->~body();
    data = _data;
    type = _type;
}

void body::clear() noexcept {
    this->~body();
    data = nullptr;
    type = types::empty;
}

body::~body() noexcept {
    switch (type) {
        case types::string:
            std::string(std::move(*static_cast<string_type *>(data)));
            break;
        case types::stream:
            delete static_cast<stream_type *>(data);
            break;
        default:
            break;
    }
}

body &body::operator<<(std::string_view const &str) noexcept {
    switch (type) {
        case types::empty:
            replace_string_view(str);
            break;
        case types::string:
            append_string(str);
            break;
        case types::stream:
            std::stringstream sstr;
            sstr << str;
            append_stream(sstr);
            break;
    }
    return *this;
}

void body::append_string(std::string_view const &str) noexcept {
    switch (type) {
        case types::empty:
            replace_string_view(str);
            break;
        case types::string:
            str_ref().append(str);
            break;
        case types::stream:
            stream_ref() << str;
            break;
    }
}

body::string_type const &body::str_ref() const noexcept {
    return *static_cast<string_type *>(data);
}

body::string_type &body::str_ref() noexcept {
    return *static_cast<string_type *>(data);
}

body::stream_type &body::stream_ref() noexcept {
    return *static_cast<stream_type *>(data);
}

void body::append_stream(webpp::body::stream_type &stream) noexcept {
    switch (type) {
        case types::stream:
            stream_ref() << stream.rdbuf();
            break;
        case types::string:
            // todo: I have no idea what the heck is this!
            // https://stackoverflow.com/questions/3203452/how-to-read-entire-stream-into-a-stdstring
            str_ref().append(std::string(std::istreambuf_iterator<char>(
                    (std::istreambuf_iterator<char, std::char_traits<char>>::istream_type &) stream), {}));
            break;
        case types::empty:
            replace_stream(stream);
            break;
    }
}

