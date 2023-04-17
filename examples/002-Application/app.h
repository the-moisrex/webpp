// Created by moisrex on 5/5/20.

#ifndef WEBPP_CGI_EXAMPLE_APP_H
#define WEBPP_CGI_EXAMPLE_APP_H

#include <webpp/http/http.hpp>

namespace website {

    using namespace webpp::http;
    using namespace webpp;

    struct app {

        [[nodiscard]] auto home() const noexcept {
            return "Home page";
        }

        [[nodiscard]] auto about() const noexcept {
            return "About page";
        }


        // how to run this function:
        //   REQUEST_URI=/content-length HTTP_CONTENT_LENGTH=5 REQUEST_METHOD=POST ./cgi-application
        static stl::string get_len(context& ctx) {
            stl::string const body           = ctx.request.as();
            stl::size_t const content_length = ctx.request.headers.content_length();

            stl::string res = webpp::fmt::format("Content-Length: {}\n", content_length);
            res += webpp::fmt::format("Body Length: {}\n", body.size());

            for (auto const& hdr : ctx.request.headers) {
                res += hdr.name;
                res += ": ";
                res += hdr.value;
                res += "\n";
            }

            res += "\n";
            res += body;

            res += "\n";
            res += "Done";
            return res;
        }

        auto operator()(auto&& req) {
            const auto admin = []() {
                return "Nice page.";
            };
            static_router router{stl::tuple{*this},
                                 (get and root) >>
                                   [] {
                                       return "main page";
                                   },
                                 (post and root / "content-length") >> &app::get_len,
                                 (get and (root / "home")) >> &app::home,
                                 get && (root / "about") >> &app::about,
                                 root / "admin" >> admin};

            return router(req);
        }
    };

} // namespace website


#endif // WEBPP_CGI_EXAMPLE_APP_H
