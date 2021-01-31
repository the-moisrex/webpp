#include "../../core/include/webpp/http/protocols/cgi.hpp"
#include "../../core/include/webpp/http/routes/methods.hpp"
#include "../../core/include/webpp/http/routes/path.hpp"
#include "../../core/include/webpp/http/routes/router.hpp"

using namespace webpp;

auto about_page(auto ctx) {
    return ctx.file("about.html");
}

int main() {
    router _router{
      empty_extension_pack{},
      root >>=
      []() {
          return "main page";
      },
      get and root / "cgi-bin" / "cgi-hello-world" >>=
      []() noexcept {
          return "Hello world";
      },
      // (root / "about") >>= about_page,
      // get and root / "cgi-bin" / "cgi-hello-world" / "about" >>= about_page
    };

    // the app:
    return cgi(_router)();
}
