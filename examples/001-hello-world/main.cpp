#include <iostream>
#include <webpp/http/bodies/file.hpp>
#include <webpp/http/protocols/cgi.hpp>
#include <webpp/http/routes/methods.hpp>
#include <webpp/http/routes/path.hpp>
#include <webpp/http/routes/router.hpp>

using namespace webpp;
using namespace webpp::http;

auto page_one() {
    return "Page 1";
}

int main() {
    using extensions = extension_pack<file_response, string_response>;
    router _router{extensions{},
                   root >>=
                   []() noexcept {
                       return "main page";
                   },
                   (get and root / "page" / "one") = page_one,
                   get and root / "cgi-bin" / "cgi-hello-world" >>=
                   []() noexcept {
                       return "Hello world";
                   },
                   (root / "about") || (get and root / "cgi-bin" / "cgi-hello-world" / "about") >>=
                   [](auto ctx) {
                       return ctx.file("about.html");
                   }};

    // run the app:
    return cgi(_router)();
}
