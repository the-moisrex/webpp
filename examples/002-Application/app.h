// Created by moisrex on 5/5/20.

#ifndef WEBPP_APP_H
#define WEBPP_APP_H

#include "./common.h"

#include <webpp/application>
#include <webpp/routes/router>


struct my_router_extension {

    using context_extensions   = pack<>;
    using initial_context_type = context
};


struct app {
    using namespace webpp::routes;

    using additional_router_extensions = pack<rx_prioritized>;

    static constexpr const_router<additional_router_extensions> router = {
      get and (opath() / "home") >> app::home,
      get and (opath() / "about") >> app::about};

    app() noexcept {
    }


    auto home() noexcept {
        return response{"Home page"};
    }

    auto about() {
        return "About page";
    }

    template <typename Context>
    auto operator()(Context& req) {
        return router(req);
    }
};

#endif // WEBPP_APP_H
