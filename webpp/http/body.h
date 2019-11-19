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
        body() noexcept = default;
        explicit body(std::string* str) noexcept
            : data(str), type(types::string) {}
        explicit body(std::string&& str) noexcept
            : data(new std::string(std::move(str))), type(types::string) {}
        explicit body(std::string const& str) noexcept
            : data(new std::string(str)), type(types::string) {}
        explicit body(std::ostream& stream) noexcept
            : data(&stream), type(types::stream) {}

        /**
         * Correctly destroy the data
         */
        ~body() noexcept;

        /**
         * Empty the data
         */
        void clear() noexcept;

        /**
         * Replace the value
         * @param _data
         * @param _type
         */
        void replace(void* _data, types _type) noexcept;

        /**
         * Replace the data with a string
         * @param str
         */
        void replace_string(std::string* str) noexcept;

        /**
         * Replace the data with a string
         * @param str
         */
        void replace_string(std::string const& str) noexcept;

        /**
         * Replace the data with a string
         * @param str
         */
        void replace_string(std::string&& str) noexcept;

        /**
         * Replace the data with a string
         * @param str
         */
        void replace_string_view(std::string_view str) noexcept;

        /**
         * Replace the data with a stream
         * @param stream
         */
        void replace_stream(std::ostream& stream) noexcept;

      public:
        /**
         * Get the value as a string (converts the other types to string
         * too)
         * @param default_val the default value if we cannot convert the
         * data to string
         * @return the string representation of the data
         */
        [[nodiscard]] std::string_view
        str(std::string_view default_val = "") const noexcept;

        [[nodiscard]] auto json() const;
        auto json(std::string_view const& data);

        auto file(std::string_view const& filepath);

        std::istream& stream() const;

        std::ostream& operator<<(std::ostream& __stream) noexcept {
            // TODO: fill here
            return __stream;
        }

        // TODO: add more methods for the images and stuff
    };

}; // namespace webpp

#endif // WEBPP_BODY_H
