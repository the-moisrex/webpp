// Created by moisrex on 5/5/20.

#ifndef WEBPP_APP_H
#define WEBPP_APP_H

#include "../../core/include/webpp.hpp"

using namespace webpp;
using namespace webpp::routes;

struct app {

    static constexpr auto admin = [] {
        return "profile page";
    };

    const router<> router{this, (get & root / "home") = &app::home,
                          get & (root / "about" = &app::about),
                          root / "admin" = admin};

    auto home(Context auto& ctx) noexcept {
        return ctx.string("Home page");
    }

    auto about(Context auto& ctx) {
        return ctx.string("About page");
    }

    Response auto operator()(Context auto& req) {
        return this->router(req);
    }
};

#endif // WEBPP_APP_H
