#include "app.hpp"

#include <webpp/http/protocols/beast.hpp>

int main() {
    webpp::http::beast<website::app> my_app;
    return my_app();
}
