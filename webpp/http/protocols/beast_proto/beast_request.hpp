// Created by moisrex on 10/24/20.

#ifndef WEBPP_BEAST_REQUEST_HPP
#define WEBPP_BEAST_REQUEST_HPP

#include "../../../std/string.hpp"
#include "../../../std/string_view.hpp"
#include "../../../traits/traits.hpp"
#include "../../http_concepts.hpp"
#include "../../request_view.hpp"
#include "../../version.hpp"
#include "beast_string_body.hpp"

#include <boost/beast/http/fields.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/parser.hpp>

namespace webpp::http::beast_proto {


    template <typename CommonHTTPRequest>
    struct beast_request final
      : public CommonHTTPRequest,
        protected details::request_view_interface<typename CommonHTTPRequest::traits_type> {
        using common_http_request_type = CommonHTTPRequest;
        using traits_type              = typename common_http_request_type::traits_type;
        using string_type              = traits::general_string<traits_type>;
        using string_view_type         = traits::string_view<traits_type>;
        using allocator_pack_type      = typename common_http_request_type::allocator_pack_type;
        using headers_type             = typename common_http_request_type::headers_type;
        using field_type               = typename headers_type::field_type;

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

        using beast_request_parser_type =
          boost::beast::http::request_parser<beast_body_type, char_allocator_type>;
        using beast_parser_ref = stl::add_lvalue_reference_t<beast_request_parser_type>;

        using super = common_http_request_type;

        beast_request_ptr breq;

        template <typename StrT>
        constexpr string_view_type string_viewify(StrT&& str) const noexcept {
            return istl::string_viewify_of<string_view_type>(stl::forward<StrT>(str));
        }

        template <typename StrT>
        constexpr string_type stringify(StrT&& str) const noexcept {
            return istl::stringify_of<string_type>(stl::forward<StrT>(str),
                                                   alloc::general_alloc_for<string_type>(*this));
        }

      protected:
        using pstring_type = typename request_view::string_type;

        template <typename T>
        [[nodiscard]] inline pstring_type pstringify(T&& str) const {
            return istl::stringify_of<pstring_type>(stl::forward<T>(str),
                                                    alloc::general_alloc_for<pstring_type>(*this));
        }

        // get the dynamic request object
        [[nodiscard]] inline request_view const& dreq() const noexcept {
            return static_cast<request_view const&>(*this);
        }

        [[nodiscard]] pstring_type get_method() const override {
            return pstringify(method());
        }

        [[nodiscard]] pstring_type get_uri() const override {
            return pstringify(uri());
        }

        [[nodiscard]] http::version get_version() const noexcept override {
            return version();
        }

      public:
        template <typename... Args>
        beast_request(Args&&... args) noexcept : super(stl::forward<Args>(args)...) {}

        beast_request(beast_request const&)                = delete; // no copying for now
        beast_request(beast_request&&) noexcept            = default;
        beast_request& operator=(beast_request&&) noexcept = delete;
        beast_request& operator=(beast_request const&)     = delete; // no copying for now

        ~beast_request() final = default;

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

        void set_beast_parser(beast_parser_ref parser) noexcept {
            breq = &parser.get();
            // todo: not very efficient, is it?
            for (const auto& field : *breq) {
                this->headers.emplace(string_viewify(field.name_string()), string_viewify(field.value()));
            }
            this->body.set_beast_parser(parser);
        }
    };

} // namespace webpp::http::beast_proto

#endif // WEBPP_BEAST_REQUEST_HPP
