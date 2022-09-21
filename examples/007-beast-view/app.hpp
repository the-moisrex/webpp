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

        app() : etraits{} {
            view_man.view_roots.push_back("./public");
            view_man.view_roots.push_back("./examples/007-beast-view/public");
            view_man.view_roots.push_back("../examples/007-beast-view/public");
            view_man.view_roots.push_back("../../examples/007-beast-view/public");
            view_man.view_roots.push_back("../../../examples/007-beast-view/public");
        }

        auto operator()(auto&& req) {
            using extensions = extension_pack<string_response>;
            router _router{extensions{},
                           *this,
                           (get and root) >>=
                           [this] {
                               return view_man.view("home.html");
                           },
                           (get and root / "about") >>=
                           [this]() {
                               return view_man.view("about.html");
                           }};

            return _router(req);
        }
    };

} // namespace website


#endif // WEBPP_EXAMPLE_BEAST_APP_H
