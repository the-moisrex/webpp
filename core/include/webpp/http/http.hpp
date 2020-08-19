#ifndef WEBPP_HTTP_HTTP_H
#define WEBPP_HTTP_HTTP_H

#include "../traits/traits_concepts.hpp"
#include "./application_concepts.hpp"
#include "./interfaces/interface_concepts.hpp"
#include "./routes/router.hpp"

namespace webpp {

    template <Interface InterfaceType>
    class http : public InterfaceType {

      public:
        using interface_type   = InterfaceType;

        using interface_type::interface_type; // ctors of mommy

        int run() noexcept {
            InterfaceType::operator()();
            return 0; // success
        }
    };


}; // namespace webpp

#endif // WEBPP_HTTP_HTTP_H
