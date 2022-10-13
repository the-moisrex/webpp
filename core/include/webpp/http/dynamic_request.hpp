// Created by moisrex on 10/10/22.

#ifndef WEBPP_DYNAMIC_REQUEST_HPP
#define WEBPP_DYNAMIC_REQUEST_HPP

#include "../traits/default_traits.hpp"
#include "./protocols/cgi_proto/cgi_request.hpp"
#include "./version.hpp"

#include <variant>

namespace webpp::http {

    /**
     * This request type can hold other HTTP request types.
     * @tparam TraitsType
     */
    template <Traits TraitsType, typename CommonRequestParent>
    struct dynamic_request : public CommonRequestParent {
        using traits_type           = TraitsType;
        using common_request_parent = CommonRequestParent;

      private:
        using super = common_request_parent;

      public:
        http::version version() const noexcept {
            // todo
        }
    };
} // namespace webpp::http

#endif // WEBPP_DYNAMIC_REQUEST_HPP
