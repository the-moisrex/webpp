#include <webpp/http/request>
#include <webpp/http/response>
#include <webpp/interfaces/cgi>
#include <webpp/router>
#include <webpp/server>
#include <webpp/utils/traits>
#include <webpp/valves/methods>
#include <webpp/valves/uri>
#inclide "./app.h"

using namespace webpp;
using namespace webpp::valves;

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
