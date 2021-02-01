#include "../../core/include/webpp/http/protocols/cgi.hpp"
#include "../../core/include/webpp/http/routes/methods.hpp"
#include "../../core/include/webpp/http/routes/path.hpp"
#include "../../core/include/webpp/http/routes/router.hpp"

using namespace webpp;

auto page_one() {
    return "Page 1";
}

int main() {
    router _router{empty_extension_pack{},
                   root >>=
                   []() {
                       return "main page";
                   },
                   get and root / "cgi-bin" / "cgi-hello-world" >>=
                   []() noexcept {
                       return "Hello world";
                   },
                   (get and root / "page" / "one") = page_one,
                   (root / "about") || (get and root / "cgi-bin" / "cgi-hello-world" / "about") >>=
                   [](auto ctx) {
                       return ctx.file("about.html");
                   }};

    // the app:
    return cgi(_router)();
}
