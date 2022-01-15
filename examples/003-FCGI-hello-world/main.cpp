#include "app.h"

#include <webpp/http/protocols/fcgi.hpp>

int main() {
    webpp::http::fastcgi::fcgi<app> my_app;
    return my_app();
}
