// Created by moisrex on 5/5/20.

#ifndef WEBPP_EXAMPLE_COUNTER_APP_HPP
#define WEBPP_EXAMPLE_COUNTER_APP_HPP

#include <webpp/db/sql_database.hpp>
#include <webpp/db/sqlite/sqlite.hpp>
#include <webpp/http/http.hpp>
#include <webpp/http/routes/dynamic_router.hpp>
#include <webpp/views/view_manager.hpp>


namespace website {

    using namespace webpp::http;
    using namespace webpp;

    struct counter_model {
        sql::sql_database<sql::sqlite> db;
        stl::string                    ip;

        counter_model() {
            auto counter = db.create.if_not_exists().table("counter");
            counter["id"].primary().number().not_null();
            counter["ip"].string().unique().not_null();
            counter["val"].number().not_null().default_value(0);
            counter.commit();
        };

        bool increment() {
            auto counter = db.tables["counter"].where("ip", ip);
            counter++;
            return counter.update_or_set();
        }

        stl::size_t current() {
            return db.tables["counter"].where("ip", ip).select("value").first();
        }
    };


    // This is the controller
    struct app : public enable_owner_traits<default_traits> {
        using etraits     = enable_owner_traits<default_traits>;
        using traits_type = typename etraits::traits_type;

        views::view_manager<traits_type> view_man;
        counter_model                    counter;
        dynamic_router                   router;

        app() : etraits{}, view_man{*this} {
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
            router.callables.push_back(*this);

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

} // namespace website


#endif
