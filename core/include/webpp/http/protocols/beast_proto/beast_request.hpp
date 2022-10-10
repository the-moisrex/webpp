// Created by moisrex on 10/24/20.

#ifndef WEBPP_BEAST_REQUEST_HPP
#define WEBPP_BEAST_REQUEST_HPP

#include "../../../std/string_view.hpp"
#include "../../../traits/traits.hpp"
#include "../common/common_http_request.hpp"
#include "../protocol_concepts.hpp"

#include <boost/beast/http/fields.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http/string_body.hpp>

namespace webpp::http::beast_proto {


    // template <Traits TraitsType, typename REL, typename BodyType, typename FieldType = beast::http::fields>
    template <Traits TraitsType, typename CommonHTTPRequest>
    struct beast_request : public CommonHTTPRequest {
        using common_http_request_type = CommonHTTPRequest;
        using traits_type              = stl::remove_cvref_t<TraitsType>;
        using request_extension_list   = REL;
        using string_type              = traits::general_string<traits_type>;
        using string_view_type         = traits::string_view<traits_type>;
        using allocator_pack_type      = typename common_http_request_type::allocator_pack_type;
        using allocator_type =
          typename allocator_pack_type::template best_allocator<alloc::sync_pool_features, char>;
        using beast_body_type           = boost::beast::http::string_body;
        using beast_field_type          = boost::beast::http::basic_fields<allocator_type>;
        using beast_request_type        = boost::beast::http::request<beast_body_type, beast_field_type>;
        using beast_request_parser_type = boost::beast::http::request_parser<beast_body_type, allocator_type>;

      private:
        using super = common_http_request<TraitsType, REL, RootExtensions>;

        beast_request_parser_type parser;

        template <typename StrT>
        constexpr string_view_type string_viewify(StrT&& str) const noexcept {
            return istl::string_viewify_of<string_view_type>(stl::forward<StrT>(str));
        }

      public:
        template <typename... Args>
        beast_request(Args&&... args) noexcept
          : super(stl::forward<Args>(args)...),
            parser{
              stl::piecewise_construct,
              stl::make_tuple(), // body args
              stl::make_tuple(
                alloc::featured_alloc_for<alloc::sync_pool_features, char>(*this)) // fields args
            } {}

        beast_request(beast_request const&)     = delete; // no copying for now
        beast_request(beast_request&&) noexcept = default;

        beast_request_parser_type& beast_parser() noexcept {
            return parser;
        }

        [[nodiscard]] string_view_type uri() const {
            return string_viewify(parser.get().target());
        }

        [[nodiscard]] string_view_type method() const {
            return string_viewify(parser.get().method_string());
        }
    };

} // namespace webpp::http::beast_proto

#endif // WEBPP_BEAST_REQUEST_HPP
