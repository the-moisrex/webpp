// Created by moisrex on 5/5/20.

#ifndef WEBPP_EXAMPLE_BEAST_APP_H
#define WEBPP_EXAMPLE_BEAST_APP_H

#include <webpp/http/http.hpp>
#include <webpp/traits/default_traits.hpp>
#include <webpp/traits/enable_traits.hpp>
#include <webpp/views/view_manager.hpp>


namespace website {

    using namespace webpp::http;
    using namespace webpp;

    struct app : public enable_owner_traits<default_traits> {
        using etraits     = enable_owner_traits<default_traits>;
        using traits_type = typename etraits::traits_type;

        views::view_manager<traits_type> view_man{*this};

        app() {
            view_man.view_roots.push_back("./public");
        }

        auto operator()(auto&& req) {
            router _router{(get and root) >>=
                           [this] {
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
