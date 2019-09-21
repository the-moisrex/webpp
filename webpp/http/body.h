#ifndef WEBPP_BODY_H
#define WEBPP_BODY_H

#include <memory>
#include <string>
#include <string_view>

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
        std::shared_ptr<Interface> _interface;

      public:
        body() noexcept : _interface(std::make_shared<Interface>()) {}
        body(std::shared_ptr<Interface> const& __interface) noexcept
            : _interface(__interface) {}
        body(Interface const& __interface) noexcept
            : _interface(std::make_shared<Interface>(__interface)) {}
        body(Interface&& __interface) noexcept
            : _interface(std::make_shared<Interface>(std::move(__interface))) {}

        /**
         * Get the interface
         * @return interface
         */
        auto interface() noexcept { return _interface; }

        void* const& json() const;

        /**
         * @brief get the whole body as a string
         * @return string_view
         */
        std::string_view string() const;
        std::istream& stream() const;

        std::ostream& operator<<(std::ostream& __stream) noexcept {
            // TODO: fill here
            return __stream;
        }

        /**
         * Perfect forwarding the read method.
         */
        template <typename... T>
        auto read(T&&... data) const {
            return _interface.read(std::forward<T>(data)...);
        }

        // TODO: add more methods for the images and stuff
    };

    template <typename Interface>
    std::ostream& operator<<(std::ostream& out, const body<Interface>& _body) {
        out << _body.string();
        return out;
    }

}; // namespace webpp

#endif // WEBPP_BODY_H
