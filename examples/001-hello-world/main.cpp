#include <webpp/http/http.hpp>
#include <webpp/http/protocols/cgi.hpp>

using namespace webpp;
using namespace webpp::http;

auto page_one() {
    return "Page 1";
}

int main() {
    static_router router{root >>
                           []() noexcept {
                               return "main page";
                           },
                         (get and root / "page" / "one") >> &page_one,
                         get and root / "cgi-bin" / "cgi-hello-world" >>
                                   []() noexcept {
                                       return "Hello world";
                                   },
                         (root / "about") || (get and root / "cgi-bin" / "cgi-hello-world" / "about") >>
                                               []() {
                                                   return "About Page";
                                               }};


    // run the app:
    return cgi{router}();
}
