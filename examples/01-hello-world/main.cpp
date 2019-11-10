#include <webpp/webpp.h>

using namespace webpp;

using request = request_t<cgi>;

response about_page(request req) { return file("about.html"); }

int main() {

  webpp<cgi> app;
    app.router.on(get & "/home"_path, [](auto& req, auto& res) {
        res << "hello world";
        res << '\n';
    });
  app.router.on(get and "/about"_path, about_page);
  app.router.on("/user/{user_id}"_tpath, [](auto const& req, auto& res) {
      auto headers = res.headers();
      headers["User-Id"] = req.url().segments()["user_id"];

      app.router.on(ends_with("/profile"_tpath),
                    []() { return "welcome to your profile"; });

      app.router.on(post and ends_with("/contact"_path),
                    [](auto& req, auto& res) {
                        auto jdata = req.body().json();
                        // ...
                    });
  });

  return app.run();
}
