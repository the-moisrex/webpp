// Created by moisrex on 5/5/20.

#ifndef WEBPP_CGI_EXAMPLE_APP_H
#define WEBPP_CGI_EXAMPLE_APP_H

#include <webpp/http/bodies/json.hpp>
#include <webpp/http/http.hpp>
#include <webpp/json/defaultjson.hpp>

namespace website {

    using namespace webpp::http;
    using namespace webpp::json;

    struct app {
        auto about() {
            document doc;
            doc["page"] = "about";
            return doc;
        }

        auto operator()(auto&& req) {
            static_router router{
              (get and root) >>
                [] {
                    document doc;
                    doc["page"] = "about";
                    return doc;
                },
              (get and root / "about") >>
                [this]() {
                    return this->about();
                }};

            return router(req);
        }
    };

} // namespace website


#endif // WEBPP_CGI_EXAMPLE_APP_H
