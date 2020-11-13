#include "../core/include/webpp/http/http.hpp"

#include "./common_pch.hpp"
#include <string>

using namespace webpp;

TEST(Server, Creation) {

}


//
// namespace webpp {
//
//    class fake_interface;
//
//    template <>
//    class basic_request<fake_interface> : public basic_request_t {
//      public:
//        stl::string_view request_uri() const noexcept {
//            return __path;
//        }
//
//        void set_path(stl::string_view ___path) noexcept {
//            __path = ___path;
//        }
//
//      protected:
//        stl::string __path;
//    };
//
//    class fake_interface : public basic_interface<fake_interface> {
//      public:
//        webpp::router_t<fake_interface> router;
//        stl::string                     body_result;
//        stl::string                     header_result;
//        basic_request<fake_interface>   req;
//
//        fake_interface() noexcept = default;
//
//        void operator()() noexcept {
//            auto res      = this->router(req);
//            body_result   = res.body.str();
//            header_result = res.header.str();
//        }
//    };
//
//} // namespace webpp
//
// TEST(Server, Init) {
//    http<fake_interface> app;
//    app.req.set_path("/");
//    app.router.on("/"_path, [] {
//        return "hello world";
//    });
//
//    app.run();
//    EXPECT_EQ(app.body_result, "hello world");
//
//    app.router.on("/home"_path, [](basic_request<fake_interface> const& req,
//                                   response_t&                          res) {
//        res << "Coding";
//    });
//    app.req.set_path("/home");
//    app.run();
//    EXPECT_EQ(app.body_result, "Coding");
//
//    app.router.on("/about"_path, [](basic_request<fake_interface> const& req,
//                                    response_t&                          res)
//                                    {
//        res << "Something";
//        // stop reformating this into one line clang-format!
//    });
//    app.req.set_path("/about/");
//    app.run();
//    EXPECT_EQ(app.body_result, "Something");
//}
