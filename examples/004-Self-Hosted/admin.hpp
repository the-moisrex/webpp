// Created by moisrex on 5/5/20.

#ifndef WEBPP_EXAMPLE_ADMIN_H
#define WEBPP_EXAMPLE_ADMIN_H

#include "../../core/include/webpp/http/http.hpp"

#include <functional>

namespace website {

    using namespace webpp::http;

    struct admin {

        using extensions = webpp::extension_pack<string_response, json_response>;
        router<extensions> _router{
          // home
          relative / "home" >>= file("admin/page.html"),
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
