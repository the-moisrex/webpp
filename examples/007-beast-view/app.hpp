// Created by moisrex on 5/5/20.

#ifndef WEBPP_EXAMPLE_BEAST_APP_H
#define WEBPP_EXAMPLE_BEAST_APP_H

#include <webpp/http/http.hpp>
#include <webpp/traits/enable_traits.hpp>
#include <webpp/views/view_manager.hpp>


namespace website {

    using namespace webpp::http;

    struct app : enable_owner_traits<default_traits> {
        using etraits     = enable_owner_traits<default_traits>;
        using traits_type = typename etraits::traits_type;

        view_manager<traits_type> view_man;

        app() : view_man{*this} {
            view_man.view_roots.push_back("./public");
        }

        auto operator()(auto&& req) {
            using extensions = webpp::extension_pack<json_response>;

            router _router{extensions{},
                           (get and root) >>=
                           [] {
                               return view_man.view("home");
                           },
                           (get and root / "about") >>=
                           [this](auto&& ctx) {
                               return view_man.view("about");
                           }};

            return _router(req);
        }
    };

} // namespace website


#endif // WEBPP_EXAMPLE_BEAST_APP_H
