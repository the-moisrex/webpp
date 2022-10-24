// Created by moisrex on 5/5/20.

#ifndef WEBPP_EXAMPLE_APP_H
#define WEBPP_EXAMPLE_APP_H

#include <iostream>
#include <webpp/http/http.hpp>

namespace website {

    using namespace webpp::http;
    using namespace webpp;

    struct app {

        auto home(Context auto&& ctx) const noexcept {
            return ctx.string("Home page");
        }

        auto about(Context auto&& ctx) const {
            return ctx.string("About page");
        }


        // how to run this function:
        //   REQUEST_URI=/content-length HTTP_CONTENT_LENGTH=5 REQUEST_METHOD=POST ./cgi-application
        auto get_len(Context auto&& ctx) const {
            stl::size_t content_length = ctx.request.headers.content_length();
            auto res = ctx.format("Content-Length: {}\n", content_length);
            for (auto const& hdr : ctx.request.headers) {
                res += hdr.name;
                res += ": ";
                res += hdr.value;
                res += "\n";
            }
            return res;
        }

        auto operator()(auto&& req) {
            using extensions = webpp::extension_pack<string_response>;
            const auto admin = []() {
                return "Nice page.";
            };
            router _router{extensions{},
                           (get and root) >>=
                           [] {
                               return "main page";
                           },
                           (post and root / "content-length") >>= mem_call(get_len),
                           (get and (root / "home")) >>= mem_call(home),
                           get & (root / "about") >>= mem_call(about),
                           root / "admin" >>= admin};

            // for debugging purposes
            // std::cerr << _router.to_string(req) << std::endl;

            return _router(req);
        }
    };

} // namespace website


#endif // WEBPP_EXAMPLE_APP_H
