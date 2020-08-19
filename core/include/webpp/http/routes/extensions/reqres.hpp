// Created by moisrex on 5/12/20.

#ifndef WEBPP_EXTENSIONS_REQRES_H
#define WEBPP_EXTENSIONS_REQRES_H

#include "../../../traits/traits_concepts.hpp"
#include "../../interfaces/interface_concepts.hpp"
#include "../../request_concepts.hpp"
#include "../../response_concepts.hpp"

namespace webpp::inline extensions {

    /**
     * Context Extension Request Response
     * This context extension will add both request and response public
     * members to access them in the routes and other extensions.
     */
    template <Request ReqType, Response ResType>
    struct ce_reqres {


        using traits_type    = typename ReqType::traits_type;
        using interface_type = typename ReqType::interface_type;
        using request_type   = ReqType;
        using response_type  = ResType;

        static_assert(std::is_same_v<typename request_type::traits_type,
                                     typename response_type::traits_type>,
                      "The traits type in the request and the response are "
                      "not the same; the framework currently doesn't "
                      "support traits conversion algorithms.");

        ce_reqres(request_type const& req, response_type& res) noexcept
          : request{req},
            response{res} {
        }


        // public fields:
        request_type const& request;
        response_type&      response;
    };

    /**
     * Router Extension Request Response
     */
    template <Request ReqType, Response ResType>
    struct re_reqres {

        using traits_type    = typename ReqType::traits_type;
        using interface_type = typename ReqType::interface_type;
        using request_type   = ReqType;
        using response_type  = ResType;

        static_assert(std::is_same_v<typename request_type::traits_type,
                                     typename response_type::traits_type>,
                      "The traits type in the request and the response are "
                      "not the same; the framework currently doesn't "
                      "support traits conversion algorithms.");

        using required_context_extensions =
          ce_reqres<request_type, response_type>;
    };

    /**
     * I'm sure it's gonna give me an error
     */
    template <Traits TraitsType, Interface InterfaceType>
    struct re_reqres : public re_reqres<request_t<TraitsType, InterfaceType>,
                                        response_t<TraitsType>> {};

} // namespace webpp::inline extensions

#endif // WEBPP_EXTENSIONS_REQRES_H
