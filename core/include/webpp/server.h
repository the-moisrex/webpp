#ifndef WEBPP_SERVER_H
#define WEBPP_SERVER_H

#include "interfaces/basic_interface.h"
#include "router.h"

namespace webpp {

    template <typename Traits, typename Interface>
    class server : public Interface {
        static_assert(
          is_traits_v<Traits>,
          "The specified template parameter is not a valid traits type.");

        static_assert(
          std::is_convertible_v<Interface, basic_interface<Traits, Interface>>,
          "It's not an interface");

      public:
        using traits    = Traits;
        using interface = Interface;

        using Interface::Interface;

        int run() noexcept {
            Interface::operator()();
            return 0; // success
        }

      private:
    };

}; // namespace webpp

#endif // WEBPP_SERVER_H
