#include "../../core/include/webpp/http/request.hpp"
#include "../../core/include/webpp/http/response.hpp"
#include "../../core/include/webpp/http/protocols/cgi.hpp"
#include "../../core/include/webpp/http/routes/router.hpp"
//#include "../../core/include/webpp/server"
#include "../../core/include/webpp/traits/std_traits.hpp"
#include "../../core/include/webpp/http/routes/path.hpp"
#include "../../core/include/webpp/http/routes/methods.hpp"

using namespace webpp;

auto about_page(auto ctx) {
    return ctx.file("about.html");
}

int main() {
    router _router {
        root >>= [] {
            return "main page";
        },
        get and root / "cgi-bin" / "webpp_helloworld" >>= [](auto ctx) noexcept {
            return "Hello world";
        },
        root / "about" >>= &about_page,
        get and root / "cgi-bin" / "webpp_helloworld" / "about" >>= &about_page
    };
    cgi<decltype(_router)> app(_router);
    app();
    return 0;
}
