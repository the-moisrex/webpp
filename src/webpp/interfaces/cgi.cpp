#include "cgi.h"
#include <iostream>
#include "../http/request.h"
#include <cstdlib>

using namespace std;
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

char const* cgi::env(char const*const &name) const noexcept {
  auto a = std::getenv(name);
  if (!a)
    return "";
  return a;
}

char const* cgi::remote_addr() const noexcept {
  return env("REMOTE_ADDR");
}

int cgi::remote_port() const noexcept {
  return atoi(env("REMOTE_PORT"));
}

int cgi::server_port() const noexcept {
  return atoi(env("SERVER_PORT"));
}

char const* cgi::server_addr() const noexcept {
  return env("SERVER_ADDR");
}

char const* cgi::server_name() const noexcept {
  return env("SERVER_NAME");
}

void cgi::run(const router& _router) noexcept {
    webpp::request<webpp::cgi> req(this);
    auto res = _router.run(req);
    for (auto const &header : res.headers()) {
      std::cout << header.attr() << ": " << header.value() << "\r\n";
    }
    std::cout << "\r\n" << res.body();
}
