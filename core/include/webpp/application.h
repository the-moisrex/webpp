// Created by moisrex on 5/5/20.

#ifndef WEBPP_APPLICATION_H
#define WEBPP_APPLICATION_H

#include "./http/request.h"
#include "./http/response.h"

#include <type_traits>

namespace webpp {

    template <typename... AppTypes>
    struct master_application {

        using traits = Traits;

        template <typename ContextType>
        auto operator()(ContextType& ctx) noexcept {
            return router(ctx);
        }
    };

} // namespace webpp

#endif // WEBPP_APPLICATION_H
