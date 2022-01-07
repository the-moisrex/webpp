#include "./app.h"

#include <webpp/http/interfaces/fcgi.hpp>

int main() {
    using namespace webpp;
    http<fcgi<default_traits, app>> my_app;
    my_app();
    return 0;
}
