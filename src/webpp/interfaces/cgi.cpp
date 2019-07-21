#include "cgi.h"
#include <iostream>
#include "../http/request.h"
#include <cstdlib>
#include <cctype>
#include <algorithm>
#include <functional>

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

cgi::cgi() {}

char const* cgi::env(std::string_view name) const noexcept {
  auto a = std::getenv(name.c_str());
  if (!a)
    return "";
  return a;
}

char const* cgi::remote_addr() const noexcept {
  return env("REMOTE_ADDR");
}

int cgi::remote_port() const noexcept {
  return atoi(env("REMOTE_PORT")); // default value: 0
}

int cgi::server_port() const noexcept {
  return atoi(env("SERVER_PORT")); // default value: 0
}

char const* cgi::server_addr() const noexcept {
  return env("SERVER_ADDR");
}

char const* cgi::server_name() const noexcept {
  return env("SERVER_NAME");
}

char const* cgi::request_uri() const noexcept {
  return env("REQUEST_URI");
}

char const* cgi::header(std::string_view str) const noexcept {
  std::transform(str.begin(), str.end(), str.begin(), static_cast<int(*)(int)>(&std::toupper));
  str.insert(0, "HTTP_");
  return env(str.c_str());
}

void cgi::run(const router& _router) noexcept {
    webpp::request<webpp::cgi> req(this);
    auto res = _router.run(req);
    for (auto const &header : res.headers()) {
      std::cout << header.attr() << ": " << header.value() << "\r\n";
    }
    std::cout << "\r\n" << res.body();
}

::webpp::body body() const noexcept {
  ::webpp::body<webpp::cgi> data { this };
  return data;
}

size_t cgi::read(char* data, size_t length) const {
  std::cin.read(data, length);
  return std::cin.gcount();
}
