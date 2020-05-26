// Created by moisrex on 5/5/20.

#ifndef WEBPP_APP_H
#define WEBPP_APP_H

#include "./common.h"

#include <webpp/application>
#include <webpp/routes/router>

using namespace webpp::routes;

// admin sub application
using admin_app = const_route{get and "profile" >> [] {
    return "profile page";
}};


struct app {

    // webpp can pick up on this
    using traits_type = std_traits;

    admin_app admin;

    static constexpr const_router router = {
      get and (opath() / "home") >> app::home,
      get and (opath() / "about") >> app::about, opath() / "admin" >> admin};

    app() noexcept {
    }


    auto home() noexcept {
        return response{"Home page"};
    }

    auto about() {
        return "About page";
    }

    Response auto operator()(Context auto& req) {
        return router(req);
    }
};

#endif // WEBPP_APP_H
