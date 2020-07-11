#ifndef WEBPP_HTTP_HTTP_H
#define WEBPP_HTTP_HTTP_H

#include "../traits/traits_concepts.hpp"
#include "./application_concepts.hpp"
#include "./interfaces/basic_interface_concepts.hpp"
#include "./routes/router.hpp"

namespace webpp {

    template <Interface InterfaceType, Application APP>
    class http : public InterfaceType {

      public:
        using interface_type   = InterfaceType;
        using application_type = APP;

        application_type app;

        using interface_type::interface_type; // ctors of mommy

        int run() noexcept {
            InterfaceType::operator()();
            return 0; // success
        }
    };

    // todo: we can provide more tools for traits here so the user can get the allocators from the interface (for app)

}; // namespace webpp

#endif // WEBPP_HTTP_HTTP_H
