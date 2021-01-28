#include "../../core/include/webpp/http/protocols/cgi.hpp"
#include "./app.h"

int main() {
    using namespace webpp;

    cgi<app> my_app;

    return my_app();
}
