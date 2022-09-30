// Created by moisrex on 9/30/22.

#include "app.hpp"

#include "counter_model.hpp"

#include <webpp/http/http.hpp>
#include <webpp/views/view_manager.hpp>

using namespace website;
using namespace webpp;
using namespace webpp::http;

using traits_type = app::traits_type;

// This is the controller
struct app_impl : public enable_traits<traits_type> {
    using etraits = enable_traits<traits_type>;
    // using traits_type = typename etraits::traits_type;

    views::view_manager<traits_type> view_man;
    counter_model                    counter;
    dynamic_router                   router;


    app_impl(etraits const& et) : etraits{et}, view_man{*this} {
        view_man.view_roots.push_back("./public");
        view_man.view_roots.push_back("./examples/007-beast-view/public");
        view_man.view_roots.push_back("../examples/007-beast-view/public");
        view_man.view_roots.push_back("../../examples/007-beast-view/public");
        view_man.view_roots.push_back("../../../examples/007-beast-view/public");

        router.synced(true);
        setup_routes();
    }

    void increment(context const& ctx) {
        stl::string_view ip = ctx.client_ip();
        counter.ip          = ip;
        if (!counter.increment()) {
            this->logger.error("We're not able to increment for some reason.");
        }
    }

    void setup_routes() {
        router.objects.emplace_back(*this);

        // setup migrations
        router += &app::increment; // migration

        // setup routes
        router[get and root]           = &app::home;
        router[get and root / "about"] = &app::about;
    }

    // home page
    response home(context const& ctx) noexcept {
        return view_man.mustache("home",
                                 {
                                   {"count", counter.current()},   // visited times
                                   {"ip_address", ctx.client_ip()} // user ip
                                 });
    }

    // about page
    response about(context const& ctx) noexcept {
        return view_man.mustache("about");
    }

    // all pages
    auto operator()(auto&& req) {
        return router(req);
    }
};




// app impl
app::app(typename app::etraits const& et) : the_app{stl::make_unique<app_impl>(et)} {}

dynamic_response<traits_type> operator()(dynamic_request<traits_type> const& req) {
    the_app->operator()(req);
}
