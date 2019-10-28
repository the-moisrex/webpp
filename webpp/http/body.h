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

    class body {
      private:

      public:


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

    std::ostream& operator<<(std::ostream& out, const body& _body) {
        out << _body.string();
        return out;
    }

}; // namespace webpp

#endif // WEBPP_BODY_H
