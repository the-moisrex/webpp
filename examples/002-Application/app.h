// Created by moisrex on 5/5/20.

#ifndef WEBPP_APP_H
#define WEBPP_APP_H

#include "../../core/include/webpp.hpp"

using namespace webpp;
using namespace webpp::routes;

// admin sub application
using admin_app = route{get and "profile" >> [] {
    return "profile page";
}};


struct app {

    // webpp can pick up on this
    using traits_type = std_traits;

    admin_app admin;

    static constexpr webpp::router router {
      get and (opath() / "home") >> app::home,
      get and (opath() / "about") >> app::about, opath() / "admin" >> admin};

    app() noexcept {
    }


    auto home(Context auto & ctx) noexcept {
        return ctx.string("Home page");
    }

    auto about(Context auto & ctx) {
        return ctx.string("About page");
    }

    Response auto operator()(Context auto& req) {
        return this->router(req);
    }
};

#endif // WEBPP_APP_H
