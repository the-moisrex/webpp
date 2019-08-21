#ifndef SERVER_H
#define SERVER_H

#include "router.h"

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
            try {
                _interface.run(_router);
            } catch (...) { // TODO: add more handling stuff here
                return EXIT_FAILURE;
            }
            return EXIT_SUCCESS;
        }
    };

}; // namespace webpp

#endif // SERVER_H
