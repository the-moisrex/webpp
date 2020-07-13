#include "./app.h"
#include "../../core/include/webpp/http/interfaces/cgi.hpp"

int main() {
    using namespace webpp;
    http<cgi<std_traits, app>> my_app;
    return my_app();
}
