// Created by moisrex on 5/5/20.

#ifndef WEBPP_EXAMPLE_BEAST_APP_H
#define WEBPP_EXAMPLE_BEAST_APP_H

#include <webpp/http/bodies/json.hpp>
#include <webpp/http/http.hpp>
#include <webpp/json/defaultjson.hpp>

namespace website {

    using namespace webpp::http;
    using namespace webpp::json;

    struct app {

        auto about([[maybe_unused]] auto const& ctx) {
            auto doc    = ctx.json_doc();
            doc["page"] = "about";
            return doc;
        }

        auto operator()(auto&& req) {
            using extensions = webpp::extension_pack<json_response>;

            router _router{extensions{},
                           (get and root) >>=
                           [] {
                               document doc;
                               doc["page"] = "about";
                               return doc;
                           },
                           (get and root / "about") >>=
                           [this](auto&& ctx) {
                               return this->about(ctx);
                           },
                           [] {
                               document doc;
                               doc["welcome"] = "Hello World";
                               return doc;
                           }};

            return _router(req);
        }
    };

} // namespace website


#endif // WEBPP_EXAMPLE_BEAST_APP_H