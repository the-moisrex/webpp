// Created by moisrex on 5/5/20.

#ifndef WEBPP_APPLICATION_H
#define WEBPP_APPLICATION_H

#include "./http/request.h"
#include "./http/response.h"

#include <type_traits>

namespace webpp {

    template <typename Traits, typename Interface, typename Router,
              typename... ExtensionTypes>
    struct application : public std::decay_t<ExtensionTypes>... {

        using traits    = Traits;
        using interface = Interface;
        using req_t     = request<traits, interface>;
        using res_t     = response<traits>;

        Rotuer router;

        res_t operator()(req_t const& req) noexcept {
            return router(req);
        }
    };

} // namespace webpp

#endif // WEBPP_APPLICATION_H
