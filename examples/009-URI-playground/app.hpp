#ifndef WEBPP_EXAMPLE_URI_PLAYGROUND_APP_HPP
#define WEBPP_EXAMPLE_URI_PLAYGROUND_APP_HPP

#include <optional>
#include <string_view>
#include <webpp/http/bodies/json.hpp>
#include <webpp/http/bodies/string.hpp>
#include <webpp/http/mime_types.hpp>
#include <webpp/http/response.hpp>
#include <webpp/http/routes/context.hpp>
#include <webpp/http/routes/dynamic_router.hpp>
#include <webpp/json/defaultjson.hpp>
#include <webpp/views/view_manager.hpp>


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
            if (!ctx.path_traverser().next()) {
                return stl::nullopt;
            }
            auto const slug = ctx.path_traverser().segment();
            if (ctx.path_traverser().next()) {
                return stl::nullopt;
            }
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

            document<default_traits> doc;
            doc["valid"] = false;
            doc["error"] = "No URI specified.";
            return doc;
        }

        [[nodiscard]] response operator()(auto&& req) {
            etraits.logger.info(stl::format("{} {}", req.method(), req.uri()));
            return router(req);
        }

      private:
        void setup_routes() {
            router.objects.emplace_back(this);

            router += router / "assets" / &app::assets;
            router += router / "parse-uri" >> &app::parse_uri;
            router += router % &app::home;

            etraits.logger.info(router.to_string());
        }

        void setup_view_manager() {
            resources.view_roots.emplace_back(".");
            resources.view_roots.emplace_back("./public");
            resources.view_roots.emplace_back("./009-URI-playground/public");
            resources.view_roots.emplace_back("./examples/009-URI-playground/public");
            resources.view_roots.emplace_back("./webpp/examples/009-URI-playground/public");
        }
    };


} // namespace website


#endif // WEBPP_EXAMPLE_URI_PLAYGROUND_APP_HPP
