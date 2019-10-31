#include "cgi.h"
#include <cctype>
#include <cstdlib>
#include <functional>
#include <iostream>

using namespace webpp;

//    AUTH_PASSWORD
//    AUTH_TYPE
//    AUTH_USER
//    CERT_COOKIE
//    CERT_FLAGS
//    CERT_ISSUER
//    CERT_KEYSIZE
//    CERT_SECRETKEYSIZE
//    CERT_SERIALNUMBER
//    CERT_SERVER_ISSUER
//    CERT_SERVER_SUBJECT
//    CERT_SUBJECT
//    CF_TEMPLATE_PATH
//    CONTENT_LENGTH
//    CONTENT_TYPE
//    CONTEXT_PATH
//    GATEWAY_INTERFACE
//    HTTPS
//    HTTPS_KEYSIZE
//    HTTPS_SECRETKEYSIZE
//    HTTPS_SERVER_ISSUER
//    HTTPS_SERVER_SUBJECT
//    HTTP_ACCEPT
//    HTTP_ACCEPT_ENCODING
//    HTTP_ACCEPT_LANGUAGE
//    HTTP_CONNECTION
//    HTTP_COOKIE
//    HTTP_HOST
//    HTTP_REFERER
//    HTTP_USER_AGENT
//    QUERY_STRING
//    REMOTE_ADDR
//    REMOTE_HOST
//    REMOTE_USER
//    REQUEST_METHOD
//    SCRIPT_NAME
//    SERVER_NAME
//    SERVER_PORT
//    SERVER_PORT_SECURE
//    SERVER_PROTOCOL
//    SERVER_SOFTWARE
//    WEB_SERVER_API

std::string_view cgi::header(std::string name) noexcept {

    // fixme: check if this is all we have to do or we have to do more too:
    std::transform(name.begin(), name.end(), name.begin(), [](auto const& c) {
        if (c == '-')
            return '_';
        return std::toupper(c);
    });

    name.insert(0, "HTTP_");
    return env(name.c_str());
}

// void cgi::run(router<cgi>& _router) noexcept {
//    auto self = std::make_shared<cgi>(this);
//    webpp::request<webpp::cgi> req(self);
//    auto res = _router.run(req);
//    std::ios_base::sync_with_stdio(false); // TODO: write tests for this
//    part for (auto const& [attr, value] : res.headers()) {
//        std::cout << attr << ": " << value << "\r\n";
//    }
//    std::cout << "\r\n" << res.body();
//}

std::streamsize cgi::read(char* data, std::streamsize length) noexcept {
    std::cin.read(data, length);
    return std::cin.gcount();
}

void cgi::write(std::ostream& stream) noexcept {
    // I think o-stream is not readable so we cannot do this:
    // https://stackoverflow.com/questions/15629886/how-to-write-ostringstream-directly-to-cout
    std::cout << stream.rdbuf(); // TODO: test this, I don't trust myself :)
}

void cgi::write(char const* data, std::streamsize length) noexcept {
    std::cout.write(data, length);
}

std::string_view cgi::env(char const* const key) noexcept {
    if (auto value = getenv(key))
        return value;
    return {};
}
