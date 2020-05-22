#ifndef WEBPP_SERVER_H
#define WEBPP_SERVER_H

#include "./http/interfaces/basic_interface_concepts.h"
#include "./http/routes/router.h"
#include "./traits/traits_concepts.h"

namespace webpp {

    template <Traits TraitsType, Interface InterfaceType>
    class server : public InterfaceType {

      public:
        using traits_type    = TraitsType;
        using interface_type = InterfaceType;

        using InterfaceType::Interface;

        int run() noexcept {
            InterfaceType::operator()();
            return 0; // success
        }

      private:
    };

}; // namespace webpp

#endif // WEBPP_SERVER_H
