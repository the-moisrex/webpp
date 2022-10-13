
#include "../core/include/webpp/http/request.hpp"

#include "../core/include/webpp/traits/default_traits.hpp"
#include "common_pch.hpp"


using namespace webpp;
using namespace webpp::http;



template <Allocator AllocType>
struct fake_request_headers_descriptor {
    template <typename ExtensionType>
    using extractor_type = typename ExtensionType::request_headers_extensions;

    template <typename RootExtensions, typename TraitsType, typename EList>
    using mid_level_extensie_type = int;
};


template <Traits TraitsType, typename RootExtensions, Allocator AllocType>
using fake_simple_request_headers =
  typename RootExtensions::template extensie_type<TraitsType, fake_request_headers_descriptor<AllocType>>;


TEST(HTTPRequestTest, HeaderType) {
    using fields_t =
      typename empty_extension_pack::template extensie_type<default_traits, request_header_field_descriptor>;
    using res_t = simple_request_headers<default_traits, empty_extension_pack, std::allocator<char>>;
    EXPECT_FALSE(static_cast<bool>(stl::same_as<fields_t, istl::nothing_type>))
      << "Fields should not be empty";
    EXPECT_FALSE(static_cast<bool>(stl::same_as<res_t, istl::nothing_type>))
      << "Request Headers should not be empty";


    using fake_int_t =
      fake_simple_request_headers<default_traits, empty_extension_pack, std::allocator<char>>;
    EXPECT_TRUE(static_cast<bool>(stl::same_as<fake_int_t, int>)) << "Fake Request Headers should not be int";

    using mid_type = request_headers<
      istl::nothing_type,
      typename empty_extension_pack::template extensie_type<std_traits, request_header_field_descriptor>,
      stl::allocator<char>>;
    EXPECT_FALSE(static_cast<bool>(stl::same_as<mid_type, istl::nothing_type>))
      << "Request Headers should not be empty; mid-type";
}
