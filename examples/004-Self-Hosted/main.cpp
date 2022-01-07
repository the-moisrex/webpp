#include "website.hpp"

#include <webpp/http/protocols/self_hosted.hpp>

int main() {
    webpp::http::self_hosted<website::blog> my_app;
    return my_app();
}
