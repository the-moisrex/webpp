// Created by moisrex on 5/5/20.

#ifndef WEBPP_FCGI_EXAMPLE_APP_H
#define WEBPP_FCGI_EXAMPLE_APP_H

#include <webpp/http/http.hpp>
#include <webpp/webpp.hpp>


struct app {
    using namespace webpp;
    using namespace webpp::http;

    auto home(Context auto& ctx) noexcept {
        return ctx.string("Home page");
    }

    auto about(Context auto& ctx) noexcept {
        return ctx.string("About page");
    }

    auto operator()(HTTPRequest auto& req) noexcept {
        using extensions = extension_pack<string_response>;
        const auto admin = []() {
            return "Nice page.";
        };


        static_router router{get and root / "home" >>
                                       [this](context& ctx) {
                                           return this->home(ctx);
                                       },
                             get & (root / "about" >>
                                    [this](context& ctx) {
                                        return this->about(ctx);
                                    }),
                             root / "admin" >> admin};

        return router(req);
    }
};

#endif // WEBPP_FCGI_EXAMPLE_APP_H
