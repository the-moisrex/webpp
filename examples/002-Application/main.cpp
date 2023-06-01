#include "app.h"

#include <webpp/cgi/cgi.hpp>

int main() {
    webpp::http::cgi<website::app> my_app;
    return my_app();
}
