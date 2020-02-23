#ifndef WEBPP_SERVER_H
#define WEBPP_SERVER_H

#include "router.h"

namespace webpp {

    template <typename Interface>
    class server : Interface {

        static_assert(std::is_convertible_v<Interface, basic_interface>,
                      "It's not an interface");

      public:
        using Interface::Interface;
        using Interface::router;

        int run() noexcept {
            Interface::operator()();
            return 0; // success
        }

      private:
    };

}; // namespace webpp

#endif // WEBPP_SERVER_H
