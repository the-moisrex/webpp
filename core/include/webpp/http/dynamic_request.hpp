// Created by moisrex on 10/10/22.

#ifndef WEBPP_DYNAMIC_REQUEST_HPP
#define WEBPP_DYNAMIC_REQUEST_HPP

#include "../traits/default_traits.hpp"
#include "./protocols/beast_proto/beast_request.hpp"
#include "./protocols/cgi_proto/cgi_request.hpp"
#include "./version.hpp"

#include <variant>

namespace webpp::http {

    /**
     * This request type can hold other HTTP request types.
     */
    template <typename CommonRequestType, template <typename> typename... RequestTypes>
    struct basic_dynamic_request : public CommonRequestType {
        using common_request_type = CommonRequestType;
        using traits_type         = typename common_request_type::traits_type;
        using string_type         = traits::general_string<traits_type>;

      private:
        using super = common_request_type;
        stl::variant<RequestTypes<CommonRequestType>*...> reqvar;

#define call_req(mem, ...)                                      \
    stl::visit(                                                 \
      [](auto* req) noexcept(noexcept(req->mem(__VA_ARGS__))) { \
          return req->mem(__VA_ARGS__);                         \
      },                                                        \
      reqvar)


      public:
        // Get the raw requested URI
        // This value is not checked for security; this is raw
        [[nodiscard]] string_type uri() const {
            return call_req(uri);
        }

        // Get the request METHOD (GET/PUT/POST/...)
        // This is unfiltered user input; don't store this value anywhere if you haven't checked the
        // correctness of its value
        [[nodiscard]] string_type method() const {
            return call_req(method);
        }

        // Get the HTTP version of the request
        [[nodiscard]] http::version version() const noexcept {
            return call_req(version);
        }

#undef call_req
    };


    template <typename CommonRequestParent>
    struct dynamic_request
      : basic_dynamic_request<CommonRequestParent, cgi_request, beast_proto::beast_request> {
        using basic_dynamic_request<CommonRequestParent, cgi_request, beast_proto::beast_request>::
          beast_dynamic_request;
    };
} // namespace webpp::http

#endif // WEBPP_DYNAMIC_REQUEST_HPP
