// Created by moisrex on 5/5/20.

#ifndef WEBPP_EXAMPLE_ADMIN_H
#define WEBPP_EXAMPLE_ADMIN_H

#include <functional>
#include <webpp/http/bodies/json.hpp>
#include <webpp/http/http.hpp>

namespace website {
    using namespace webpp;
    using namespace webpp::http;

    struct admin {

        using my_extensions = extension_pack<string_body, json_body>;
        router<my_extensions> _router{
          // home
          relative / "home" >>= static_file("admin/page.html"),
          relative / "help" >>= mem_call(help)
          // done
        };


        auto help(Context auto& ctx) noexcept {
            return ctx.file("help.html");
        }

        auto operator()(HTTPRequest auto&& req) {
            return _router(req);
        }
    };

} // namespace website


#endif // WEBPP_EXAMPLE_ADMIN_H
