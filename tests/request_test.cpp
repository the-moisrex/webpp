#include "../core/include/webpp/http/request.hpp"

#include "../core/include/webpp/traits/default_traits.hpp"
#include "common_pch.hpp"
#include "fake_protocol.hpp"


using namespace webpp;
using namespace webpp::http;



TEST(HTTPRequestTest, HeaderType) {
    using fake_protocol = fake_proto<default_traits, fake_app>;
    using fake_req_type = simple_request<fake_protocol, fake_proto_request>;
    EXPECT_TRUE(bool(HTTPRequest<fake_req_type>));
    EXPECT_TRUE(bool(HTTPRequestHeaders<typename fake_req_type::headers_type>));
}
