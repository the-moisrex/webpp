#ifndef WEBPP_SERVER_H
#define WEBPP_SERVER_H

#include "router.h"

namespace webpp {

    template <class Interface>
    class server {
      protected:
        std::shared_ptr<Interface> _interface;
        webpp::router<Interface> _router;

      public:
        server(Interface const& inerface) noexcept;
        server() noexcept;

        /**
         * Set router
         * @param router
         */
        server& router(webpp::router<Interface> const& r) noexcept {
            _router = r;
            return *this;
        }

        /**
         * Set router
         * @param router
         */
        server& router(webpp::router<Interface>&& r) noexcept {
            _router = std::make_shared<Interface>(std::move(r));
            return *this;
        }

        /**
         * Get the router
         * @return router
         */
        const auto router() const noexcept { return _router; }

        /**
         * Get the router
         * @return router
         */
        auto router() noexcept { return _router; }

        /**
         * Set the interface
         * @param interface
         */
        server& interface(Interface const& i) noexcept {
            _interface = i;
            return *this;
        }

        /**
         * Set the interface
         * @param interface
         */
        server& interface(Interface&& i) noexcept {
            _interface = std::move(i);
            return *this;
        }

        /**
         * Get interface
         * @return interface
         */
        const auto interface() const noexcept { return _interface; }

        /**
         * Get interface
         * @return interface
         */
        auto interface() noexcept { return _interface; }

        int run() noexcept {
            // we leave the action to the interface itself because it may need
            // or may not need multi-threading.
            try {
                _interface.run(_router);
            } catch (...) { // TODO: add more handling stuff here
                return EXIT_FAILURE;
            }
            return EXIT_SUCCESS;
        }
    };

}; // namespace webpp

#endif // WEBPP_SERVER_H
