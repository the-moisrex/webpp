#ifndef WEBPP_SERVER_H
#define WEBPP_SERVER_H

#include "router.h"

namespace webpp {

    template <typename Interface>
    class server : Interface, public router {

        static_assert(std::is_convertible_v<Interface, basic_interface>,
                      "It's not an interface");

      public:
        using Interface::Interface;
        using Interface::run;

      private:
    };

}; // namespace webpp

#endif // WEBPP_SERVER_H
