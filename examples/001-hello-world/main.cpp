#include <webpp/http/request>
#include <webpp/http/response>
#include <webpp/interfaces/cgi>
#include <webpp/router>
#include <webpp/server>
#include <webpp/utils/traits>
#include <webpp/valves/methods>
#include <webpp/valves/uri>

using namespace webpp;
using namespace webpp::valves;
using iface   = cgi<std_traits>;
using request = request_t<iface>;

response about_page(request req) {
    return response::file("about.html");
}

int main() {
    server<iface> app;
    app.router.on("/"_path, [] {
        return "main page";
    });
    app.router.on(get and "/cgi-bin/webpp_helloworld"_path,
                  [](request const& req, response& res) noexcept {
                      res << "Hello world";
                  });
    app.router.on("/about"_path, &about_page);
    app.router.on(get and "/cgi-bin/webpp_helloworld/about"_path, &about_page);
    return app.run();
}
