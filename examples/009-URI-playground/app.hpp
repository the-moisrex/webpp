#ifndef WEBPP_EXAMPLE_URI_PLAYGROUND_APP_HPP
#define WEBPP_EXAMPLE_URI_PLAYGROUND_APP_HPP

#include "pch.hpp"

namespace website {

    using namespace webpp::http;
    using namespace webpp;

    struct app {
      private:
        enable_owner_traits<default_traits> etraits{};
        dynamic_router                      router{etraits};
        views::view_manager<default_traits> resources{etraits};

      public:
        app() {
            setup_routes();
            setup_view_manager();
        }

        auto home() {
            return resources.view("index.html");
        }

        std::optional<response> assets(context& ctx) {
            auto const slug = ctx.path_traverser().segment();
            if (auto res_str = resources.view(slug); !res_str.empty()) {
                response res{etraits};
                res.body = res_str;
                res.headers.set("Content-Type", stl::format("{}; charset=utf-8", mime_type_for(slug)));
                return res;
            }
            return stl::nullopt;
        }

        auto parse_uri(context const& ctx) {
            using json::document;
            using std::chrono::duration_cast;
            using std::chrono::high_resolution_clock;
            using std::chrono::nanoseconds;

            document<default_traits> doc;

            auto const query = ctx.request.uri();
            if (query.empty()) {
                doc["valid"] = false;
                doc["error"] = "No URI specified.";
            } else {
                using iterator = typename stl::remove_cvref_t<decltype(query)>::const_iterator;
                uri::parsing_uri_context<iterator> context{.beg = query.begin(),
                                                           .pos = query.begin(),
                                                           .end = query.end()};
                auto const                         start = high_resolution_clock::now();
                {
                    // calculating the time that it takes to parse it
                    uri::parse_uri(context);
                }
                auto const fin = high_resolution_clock::now();

                auto const parsing_time = duration_cast<nanoseconds>(start - fin).count();
                doc["parsing-time"]     = parsing_time;
                doc["scheme"]           = context.out.scheme();
                doc["username"]         = context.out.username();
                doc["password"]         = context.out.password();
                doc["host"]             = context.out.host();
                doc["port"]             = context.out.port();
                doc["path"]             = context.out.path();
                doc["queries"]          = context.out.queries();
                doc["fragment"]         = context.out.fragment();
            }
            return doc;
        }

        [[nodiscard]] response operator()(auto&& req) {
            etraits.logger.info(stl::format("{} {}", req.method(), req.uri()));
            return router(req);
        }

      private:
        void setup_routes() {
            router.objects.emplace_back(this);

            router += http::get / "assets" % &app::assets;
            router += http::get % "parse-uri" >> &app::parse_uri;
            router += http::get % root >> &app::home;

            etraits.logger.info(router.to_string());
        }

        void setup_view_manager() {
            resources.view_roots.emplace_back(".");
            resources.view_roots.emplace_back("./assets");
            resources.view_roots.emplace_back("./009-URI-playground/assets");
            resources.view_roots.emplace_back("./examples/009-URI-playground/assets");
            resources.view_roots.emplace_back("./webpp/examples/009-URI-playground/assets");
            resources.view_roots.emplace_back("../examples/009-URI-playground/assets");
            resources.view_roots.emplace_back("./009-URI-playground");
            resources.view_roots.emplace_back("./examples/009-URI-playground");
            resources.view_roots.emplace_back("./webpp/examples/009-URI-playground");
            resources.view_roots.emplace_back("../examples/009-URI-playground");
        }
    };


} // namespace website


#endif // WEBPP_EXAMPLE_URI_PLAYGROUND_APP_HPP
