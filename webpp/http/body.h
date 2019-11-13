#ifndef WEBPP_BODY_H
#define WEBPP_BODY_H

#include <memory>
#include <string>
#include <string_view>

/**
 * There are two types of bodies:
 *   - The request body
 *   - The response body
 *
 * Common things between the two:
 *
 * Request body:
 *   This class will not load, cache, or parse the body until the user needs it.
 *   This means that we need access to the Interface so we can use it to read
 *   the data of the body, then we parse it to the thing that the user needs.
 *
 *   - Parsing formats:
 *     - [ ] JSON that returns:
 *       - [ ] JSONObject or some sort of custom class
 *       - [ ] Array
 *       - [ ] std::vector
 *       - [ ] std::multimap
 *     - [ ] blob
 *     - [ ] GraphQL Object
 *     - [ ] Form inputs
 *       - [ ] format 1 (I don't remember the type now)
 *       - [ ] format 2 TODO
 *
 * Response Body:
 *   - Features of the response body:
 *     - Should not be a template or we have to implement free functions
 *     - Owns it's data
 *
 *   - Caching system
 *     - In memory and on hard cache:
 *       - [ ] In memory cache with use counts
 *         - so it's possible to migrate the in memory cache into hard disk when
 *           memory is about to finish
 *       - [ ] In hard disk cache (completely remove-able)
 *       - [ ] Permanent cache
 *         - We shouldn't need this. Think about it. TODO
 *     - Static responses:
 *       - [ ] Static files -> html, css, js, images, ...
 *     - Half dynamic responses:
 *       - [ ] The HTML templates
 *     - Full dynamic responses:
 *       - [ ] Forward caching
 *         - Means that the user will predict the user's future request based
 *           on the current and the past requests he/she made and then generates
 *           a response a head of time if the system is not busy.
 *
 *   - Minifying/Compressing the responses:
 *     - [ ] GZip
 *     - [ ] Data themselves:
 *       - [ ] HTML
 *       - [ ] CSS
 *       - [ ] JS
 *       - [ ] Images
 *         - [ ] JPEG
 *         - [ ] GIF
 *         - [ ] PNG
 *
 *   - Writing the data in different formats
 *     - [ ] String
 *     - [ ] JSON from:
 *       - [ ] Strings
 *       - [ ] Iterate-ables: arrays, vectors, maps, ...
 *     - Media
 *       - Images
 *         - [ ] JPEG
 *         - [ ] PNG
 *         - [ ] GIF
 *         - [ ] research more for standards
 *       - Videos
 *         - [ ] mp4
 *         - [ ] flv
 *         - [ ] research more for standards
 *       - Audios
 *         - [ ] mp3
 *         - [ ] research more for standards
 *     - [ ] GraphQL
 *     - [ ] Downloadable files of any type (blobs)
 *
 */
namespace webpp {

    class body {
      public:
        struct variants {
            using string_type = std::string;
            using stream_type = std::ostream;

          private:
            mutable void* data = nullptr;
            enum class types : uint8_t {
                empty,
                string,
                stream
            } mutable type = types::empty;

          public:
            variants() noexcept = default;
            explicit variants(std::string* str) noexcept
                : data(str), type(types::string) {}
            explicit variants(std::string&& str) noexcept
                : data(new std::string(std::move(str))), type(types::string) {}
            explicit variants(std::string const& str) noexcept
                : data(new std::string(str)), type(types::string) {}
            explicit variants(std::ostream& stream) noexcept
                : data(&stream), type(types::stream) {}

            /**
             * Correctly destroy the data
             */
            ~variants() noexcept {
                switch (type) {
                case types::string:
                    delete static_cast<std::string*>(data);
                    break;
                default:
                    break;
                }
            }

            /**
             * Empty the data
             */
            void empty() noexcept {
                this->~variants();
                data = nullptr;
                type = types::empty;
            }

            void replace(void* _data, types _type) noexcept {
                this->~variants();
                data = _data;
                type = _type;
            }

            void replace_string(std::string* str) noexcept {
                replace(str, types::string);
            }

            void replace_string(std::string const& str) noexcept {
                replace(new std::string(str), types::string);
            }

            void replace_string(std::string&& str) noexcept {
                replace(new std::string(std::move(str)), types::string);
            }

            void replace_string_view(std::string_view str) noexcept {
                replace(new std::string(str), types::string);
            }

            void replace_stream(std::ostream& stream) noexcept {
                replace(&stream, types::stream);
            }

            [[nodiscard]] std::string_view
            str(std::string_view default_val = "") const noexcept {
                if (type == types::string)
                    return *static_cast<std::string*>(data);

                // FIXME: check if there's an optimization issue here or not
                if (type == types::stream) {
                    auto ndata =
                        new std::string{std::istreambuf_iterator<char>(
                                            *static_cast<std::istream*>(data)),
                                        std::istreambuf_iterator<char>()};
                    this->~variants();
                    data = ndata;
                    type = types::string;
                    return *static_cast<std::string*>(data);
                }
                return default_val;
            }
        };

      private:
        variants data;

      public:
        [[nodiscard]] std::string_view str() const noexcept {
            return data.str();
        }

        [[nodiscard]] auto json() const;
        auto json(std::string_view const& data);

        auto file(std::string_view const& filepath);

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
