// Created by moisrex on 5/5/20.

#ifndef WEBPP_EXAMPLE_APP_H
#define WEBPP_EXAMPLE_APP_H

#include "../../core/include/webpp/http/http.hpp"

namespace website {

    using namespace webpp::http;

    struct app {

        auto home(auto const& ctx) noexcept {
            return ctx.string("Home page");
        }

        auto about(auto const& ctx) {
            return ctx.string("About page");
        }

        auto operator()(auto&& req) {
            using extensions = webpp::extension_pack<string_response>;
            const auto admin = []() {
                return "Nice page.";
            };
            router _router{extensions{},
                           get and (relative / "home") >>=
                           [this](Context auto& ctx) {
                               return this->home(ctx);
                           },
                           get & (relative / "about" >>=
                                  [this](Context auto& ctx) {
                                      return this->about(ctx);
                                  }),
                           relative / "admin" >>= admin};

            return _router(req);
        }
    };

} // namespace website


#endif // WEBPP_EXAMPLE_APP_H
