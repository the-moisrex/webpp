// Created by moisrex on 10/24/20.

#ifndef WEBPP_BEAST_REQUEST_HPP
#define WEBPP_BEAST_REQUEST_HPP

#include "../../../std/string.hpp"
#include "../../../std/string_view.hpp"
#include "../../../traits/traits.hpp"
#include "../../http_concepts.hpp"
#include "../../version.hpp"
#include "beast_string_body.hpp"

#include <boost/beast/http/fields.hpp>
#include <boost/beast/http/message.hpp>

namespace webpp::http::beast_proto {


    template <typename CommonHTTPRequest>
    struct beast_request : public CommonHTTPRequest {
        using common_http_request_type = CommonHTTPRequest;
        using traits_type              = typename common_http_request_type::traits_type;
        using string_type              = traits::general_string<traits_type>;
        using string_view_type         = traits::string_view<traits_type>;
        using allocator_pack_type      = typename common_http_request_type::allocator_pack_type;

      private:
        using request_header_type = typename common_http_request_type::headers_type;
        using request_body_type   = typename common_http_request_type::body_type;
        using char_allocator_type =
          typename allocator_pack_type::template best_allocator<alloc::sync_pool_features, char>;
        using fields_allocator_type =
          typename allocator_pack_type::template best_allocator<alloc::sync_pool_features, char>;
        using beast_fields_type = boost::beast::http::basic_fields<fields_allocator_type>;
        using beast_body_type   = string_body_of<string_type>;

        using beast_request_type = boost::beast::http::request<beast_body_type, beast_fields_type>;
        using beast_request_ref  = beast_request_type&;
        using beast_request_ptr  = beast_request_type*;

        using super = common_http_request_type;

        beast_request_ptr breq;

        template <typename StrT>
        constexpr string_view_type string_viewify(StrT&& str) const noexcept {
            return istl::string_viewify_of<string_view_type>(stl::forward<StrT>(str));
        }

        template <typename StrT>
        constexpr string_view_type stringify(StrT&& str) const noexcept {
            return istl::stringify_of<string_type>(stl::forward<StrT>(str),
                                                   alloc::general_alloc_for<string_type>(*this));
        }

      public:
        template <typename... Args>
        beast_request(Args&&... args) noexcept : super(stl::forward<Args>(args)...) {}

        beast_request(beast_request const&)     = delete; // no copying for now
        beast_request(beast_request&&) noexcept = default;

        [[nodiscard]] auto uri() const {
            return stringify(breq->target());
        }

        [[nodiscard]] auto method() const {
            return stringify(breq->method_string());
        }

        [[nodiscard]] http::version version() const noexcept {
            const auto major = static_cast<stl::uint16_t>(breq->version() / 10);
            const auto minor = static_cast<stl::uint16_t>(breq->version() % 10);
            return http::version{major, minor};
        }

        //////////////////////////////////////////

        void set_beast_request(beast_request_ref req) noexcept {
            breq = &req;
        }
    };

} // namespace webpp::http::beast_proto

#endif // WEBPP_BEAST_REQUEST_HPP
