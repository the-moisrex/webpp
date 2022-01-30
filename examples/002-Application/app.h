// Created by moisrex on 5/5/20.

#ifndef WEBPP_EXAMPLE_APP_H
#define WEBPP_EXAMPLE_APP_H

#include <iostream>
#include <webpp/http/http.hpp>

namespace website {

    using namespace webpp::http;
    using namespace webpp;

    struct app {

        auto home(Context auto&& ctx) noexcept {
            return ctx.string("Home page");
        }

        auto about(Context auto&& ctx) {
            return ctx.string("About page");
        }

        auto operator()(auto&& req) {
            using extensions = webpp::extension_pack<string_response>;
            const auto admin = []() {
                return "Nice page.";
            };
            router _router{extensions{},
                           (get and root) >>=
                           [] {
                               return "main page";
                           },
                           (get and (root / "home")) >>= mem_call(home),
                           get & (root / "about") >>= mem_call(about),
                           root / "admin" >>= admin};

            // for debugging purposes
            // std::cerr << _router.to_string(req) << std::endl;

            return _router(req);
        }
    };

} // namespace website


#endif // WEBPP_EXAMPLE_APP_H
