#include "../../core/include/webpp/http/protocols/cgi.hpp"
#include "./app.h"

int main() {
    using namespace webpp;
    cgi<std_traits, app> my_app;
    my_app();
    return 0;
}
