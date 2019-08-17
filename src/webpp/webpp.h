#ifndef WEBPP_WEBPP_H
#define WEBPP_WEBPP_H

#include "router.hpp"

namespace webpp {

    template <class Interface>
    class server {
      protected:
        Interface _interface;
        ::webpp::router _router;

      public:
        server(Interface const& inerface) noexcept;
        server() noexcept;
        void router(::webpp::router const& r) noexcept;
        void interface(Interface const& i) noexcept;
        Interface& interface() const noexcept;
        int run() noexcept {
            // we leave the action to the interface itself because it may need
            // or may not need multithreading.
            _interface.run(_router);
        }
    };

}; // namespace webpp

#endif // WEBPP_WEBPP_H
