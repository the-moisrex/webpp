#include "../core/include/webpp/http/request.hpp"

#include "../core/include/webpp/traits/default_traits.hpp"
#include "common_pch.hpp"
#include "fake_protocol.hpp"


using namespace webpp;
using namespace webpp::http;



using fake_protocol = fake_proto<default_traits, fake_app>;
using req_t         = simple_request<fake_protocol, fake_proto_request>;
TEST(HTTPRequestTest, ConceptTests) {
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
