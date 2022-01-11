#include "app.hpp"

#include <webpp/http/protocols/cgi.hpp>

int main() {
    webpp::http::cgi<website::app> my_app;
    return my_app();
}
