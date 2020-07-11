// Created by moisrex on 5/5/20.

#ifndef WEBPP_APPLICATION_H
#define WEBPP_APPLICATION_H

#include "./application_concepts.hpp"
#include "./routes/context_concepts.hpp"

#include <cstdint>
#include <type_traits>

namespace webpp {

    enum class master_app_features : ::std::int_fast16_t {
        mutable_context,   // preserve changes to the context and pass along
        immutable_context, // make a copy of context for each application
    };

    /**
     * This is the application master which lets the user to combine multiple
     * application and use them as a single application and pass to the
     * protocols when they need to.
     */
    template <Application... AppTypes>
    struct master_application : private AppTypes... {
        Response auto operator()(Context auto& ctx) noexcept {
            (AppTypes(ctx), ...); // todo: this is not correct
        }
    };

} // namespace webpp

#endif // WEBPP_APPLICATION_H
