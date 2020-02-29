#include <webpp/http/request>
#include <webpp/http/response>
#include <webpp/interfaces/cgi>
#include <webpp/router>
#include <webpp/server>
#include <webpp/valves/methods>
#include <webpp/valves/uri>

using namespace webpp;
using namespace webpp::valves;

using request = request_t<cgi>;

response about_page(request req) { return file("about.html"); }

int main() {

    server<cgi> app;
    app.router.on("/"_path, [] { return "main page"; });
    app.router.on(get and "/cgi-bin/webpp_helloworld"_path,
                  [](request const& req, response& res) noexcept {
                      res << "Hello world";
                  });
    app.router.on("/about"_path, about_page);
    // app.router.on("/user/{user_id}"_tpath, [](auto const& req, auto& res) {
    // auto headers = res.headers();
    // headers["User-Id"] = req.url().segments()["user_id"];
    //
    // app.router.on(ends_with("/profile"_tpath),
    // []() { return "welcome to your profile"; });
    //
    // app.router.on(post and ends_with("/contact"_path),
    // [](auto& req, auto& res) {
    // auto jdata = req.body().json();
    // ...
    // });
    // });
    //
    return app.run();
}
