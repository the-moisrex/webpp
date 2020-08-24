// Created by moisrex on 5/5/20.

#ifndef WEBPP_APP_H
#define WEBPP_APP_H

#include "../../core/include/webpp.hpp"

using namespace webpp;

struct app {

    auto home(Context auto& ctx) noexcept {
        return ctx.string("Home page");
    }

    auto about(Context auto& ctx) noexcept {
        return ctx.string("About page");
    }

    Response auto operator()(Request auto& req) noexcept {
        using extensions = extension_pack<string_response>;
        const auto admin = []() {
            return "Nice page.";
        };
        constexpr auto home_root = root / stl::string_view{"home"};
        router _router{extensions{},
                       home_root && get >>=
                       [this](Context auto& ctx) {
                           return this->home(ctx);
                       },
                                 get & (root / "about" >>=
                                        [this](Context auto& ctx) {
                                            return this->about(ctx);
                                        }),
                                 root / "admin" >>= admin};

        return _router(req);
    }
};

#endif // WEBPP_APP_H
