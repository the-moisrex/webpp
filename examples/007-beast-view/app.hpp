// Created by moisrex on 5/5/20.

#ifndef WEBPP_EXAMPLE_BEAST_APP_H
#define WEBPP_EXAMPLE_BEAST_APP_H

#include <webpp/http/http.hpp>
#include <webpp/traits/default_traits.hpp>
#include <webpp/traits/enable_traits.hpp>
#include <webpp/views/view_manager.hpp>

namespace website {

    using namespace webpp::http;
    using namespace webpp;

    struct app : public enable_owner_traits<default_traits> {
        using etraits     = enable_owner_traits<default_traits>;
        using traits_type = typename etraits::traits_type;

      private:
        views::view_manager<traits_type> view_man;

        // curl -d @Dockerfile localhost:8080/content-length
        static stl::string get_len(context& ctx) {
            stl::string const body           = ctx.request.as();
            stl::size_t const content_length = ctx.request.headers.content_length();

            stl::string res  = webpp::fmt::format("Content-Length: {}\n", content_length);
            res             += webpp::fmt::format("Body Length: {}\n", body.size());

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

      public:
        app() : etraits{}, view_man{*this} {
            view_man.view_roots.emplace_back("./public");
            view_man.view_roots.emplace_back("./examples/007-beast-view/public");
            view_man.view_roots.emplace_back("../examples/007-beast-view/public");
            view_man.view_roots.emplace_back("../../examples/007-beast-view/public");
            view_man.view_roots.emplace_back("../../../examples/007-beast-view/public");
        }

        HTTPResponse auto operator()(HTTPRequest auto&& req) noexcept {
            static static_router router{
              (get and root) >>
                [this] {
                    return view_man.view("home.html");
                },
              (post and root) >>
                [this](context& ctx) {
                    stl::size_t const                  body_size = ctx.request.headers.content_length();
                    stl::map<stl::string, stl::string> values;
                    values["request_body_size"] = stl::to_string(body_size);
                    values["request_body"]      = as<stl::string>(ctx.request.body);
                    return view_man.mustache("home-post", values);
                },
              (get and root / "about") >>
                [this]() {
                    return view_man.file("about.html");
                },
              (post and root / "content-length") >> &app::get_len};

            return router(req);
        }
    };

} // namespace website


#endif // WEBPP_EXAMPLE_BEAST_APP_H
