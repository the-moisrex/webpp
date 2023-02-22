#include "../core/include/webpp/http/request.hpp"

#include "../core/include/webpp/http/request_view.hpp"
#include "../core/include/webpp/traits/default_traits.hpp"
#include "common_pch.hpp"
#include "fake_protocol.hpp"


using namespace webpp;
using namespace webpp::http;



using fake_protocol = fake_proto<default_traits, fake_app>;
using req_t         = typename fake_protocol::request_type;

TEST(HTTPRequestTest, ConceptTests) {
    EXPECT_TRUE(bool(HTTPRequest<request>)) << "Request is not a valid request!!";
    EXPECT_TRUE(bool(HTTPRequest<req_t>));
    EXPECT_TRUE(bool(HTTPRequestHeaders<typename req_t::headers_type>));
    EXPECT_TRUE(bool(HTTPRequestHeaders<typename req_t::headers_type>));
    EXPECT_TRUE(bool(HTTPRequestBody<typename req_t::body_type>));
}

TEST(HTTPRequestTest, Constructors) {
    fake_protocol pt;
    req_t         req1{pt};

    // content-length
    req1.data.emplace("Content-Length", "23");
    req1.reload();
    EXPECT_EQ(23, req1.headers.content_length());
}


TEST(HTTPRequestTest, DynamicRequest) {
    fake_protocol pt;
    req_t         req{pt};
    request       dreq{req};
    EXPECT_EQ(dreq.version(), req.version());
    EXPECT_EQ(dreq.method(), req.method());
    EXPECT_EQ(dreq.uri(), req.uri());
    // EXPECT_EQ(dreq.headers, req.headers);
    EXPECT_EQ(as<std::string>(dreq.body), as<std::string>(req.body));

    dreq.body = "this is a nice code";
    req.body  = "this is a nice code";
    EXPECT_EQ(as<std::string>(dreq.body), as<std::string>(req.body));
}

TEST(HTTPRequestTest, RequestViewTest) {
    fake_protocol pt;
    req_t         req1{pt};

    req1.data.emplace("Content-Length", "23");
    req1.data.emplace("SERVER_PROTOCOL", "HTTP/1.1");
    req1.reload();

    request_view const view{req1};

    EXPECT_FALSE(view.headers.iter("content-length") == view.headers.end());
    EXPECT_EQ("23", view.headers.get("content-length"));
    EXPECT_EQ(23, view.headers.content_length());
    EXPECT_EQ(http::http_1_1, view.version());
}
