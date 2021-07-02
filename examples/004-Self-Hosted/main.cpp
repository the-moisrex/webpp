#include "../../core/include/webpp/http/protocols/self_hosted.hpp"
#include "./website.hpp"

int main() {
    webpp::http::self_hosted<website::blog> my_app;
    return my_app();
}
