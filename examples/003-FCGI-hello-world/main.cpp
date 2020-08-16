#include "./app.h"
#include "../../core/include/webpp/http/interfaces/fcgi.hpp"

int main() {
    using namespace webpp;
    http<fcgi<std_traits, app>> my_app;
    my_app();
    return 0;
}
