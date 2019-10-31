#include <webpp/webpp.h>

using namespace webpp;

using request = request_t<cgi>;
using response = request_t<cgi>;

response about_page(request req) { return file("about.html"); }

int main() {

  webpp<cgi> app;
    app.router.on(GET & "/home"_path, [](auto& req, auto& res) {
        res << "hello world";
        res << '\n';
    });
    app.router.on(GET and "/about"_path, about_page);

    return app.run();
}
