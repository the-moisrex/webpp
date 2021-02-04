#include "../../core/include/webpp/http/protocols/cgi.hpp"
#include "./app.h"

int main() {
    webpp::http::cgi<website::app> my_app;
    return my_app();
}
