
#include "../core/include/webpp/http/request.hpp"
#include "../core/include/webpp/traits/default_traits.hpp"
#include "common_pch.hpp"


using namespace webpp;
using namespace webpp::http;


TEST(HTTPRequestTest, HeaderType) {
    using res_t = simple_request_headers<default_traits, empty_extension_pack, std::allocator<char>>;
    EXPECT_FALSE(static_cast<bool>(stl::same_as<res_t, istl::nothing_type>));
}
