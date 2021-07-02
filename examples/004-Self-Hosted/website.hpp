// Created by moisrex on 5/5/20.

#ifndef WEBPP_EXAMPLE_BLOG_H
#define WEBPP_EXAMPLE_BLOG_H

#include "../../core/include/webpp/http/http.hpp"
#include "admin.hpp"


namespace website {

    using namespace webpp::http;

    struct blog {
        admin admin_panel{};

        auto home(auto const& ctx) noexcept {
            return ctx.string("Home page");
        }

        auto about(auto const& ctx) {
            return ctx.string("About page");
        }

        auto operator()(auto&& req) {
            using extensions = webpp::extension_pack<string_response>;
            router _router{extensions{},
                           (get and (root / "home")) >>=
                           [this](Context auto& ctx) {
                               return this->home(ctx);
                           },
                           get & (root / "about") >>=
                           [this](Context auto& ctx) {
                               return this->about(ctx);
                           },
                           root / "admin" >>= admin_panel};

            // for debugging purposes
            std::cerr << _router.to_string(req) << std::endl;

            return _router(req);
        }
    };

} // namespace website


#endif // WEBPP_EXAMPLE_BLOG_H
