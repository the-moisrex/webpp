#ifndef WEBPP_BODY_H
#define WEBPP_BODY_H

#include "../std/string_view.h"
#include <memory>
#include <string>

/**
 * This class will not load, cache, or parse the body until the user needs it.
 * This means that we need access to the Interface so we can use it to read the
 * data of the body, then we parse it to the thing that the user needs.
 *
 */
namespace webpp {

    template <class Interface>
    class body {
      private:
        std::shared_ptr<Interface> interface;

      public:
        body(Interface const& __interface) noexcept : interface(__interface) {}
        body(Interface&& __interface) noexcept
            : interface(std::move(__interface)) {}

        void* const& json() const;

        /**
         * @brief get the whole body as a string
         * @return string_view
         */
        std::string_view string() const;
        std::istream const& stream() const;

        std::ostream& operator<<(std::ostream& __stream) noexcept {
            // TODO: fill here
            return __stream;
        }

        /**
         * Perfect forwarding the read method.
         */
        template <typename... T>
        auto read(T&&... data) const {
            return interface.read(std::forward<T>(data)...);
        }

        // TODO: add more methods for the images and stuff
    };

}; // namespace webpp

#endif // WEBPP_BODY_H
