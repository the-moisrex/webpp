// Created by moisrex on 10/24/20.

#ifndef WEBPP_BEAST_REQUEST_HPP
#define WEBPP_BEAST_REQUEST_HPP

#include "../../std/string_view.hpp"
#include "../../traits/traits_concepts.hpp"
#include "./protocol_concepts.hpp"
#include "common/common_request.hpp"

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>

namespace webpp {

    namespace beast {
        using namespace boost::beast;
    }

    template <Traits TraitsType, typename REL, typename BodyType, typename FieldType = beast::http::fields>
    struct beast_request : public common_request<TraitsType, REL>,
                           public beast::http::request<BodyType, FieldType> {
        using traits_type            = stl::remove_cvref_t<TraitsType>;
        using request_extension_list = REL;
        using string_type            = typename traits_type::string_type;
        using string_view_type       = typename traits_type::string_view_type;
        using beast_request_type     = beast::http::request<BodyType, FieldType>;
        using body_type              = BodyType;
        using field_type             = FieldType;

      private:
        using super = common_request<TraitsType, REL>;

      public:
        template <typename... Args>
        beast_request(Args&&... args) noexcept : super(stl::forward<Args>(args)...),
                                                 beast_request_type{} {}

        beast_request(beast_request const&) = delete; // no copying for now
        beast_request(beast_request &&) noexcept = default;
    };

} // namespace webpp

#endif // WEBPP_BEAST_REQUEST_HPP
