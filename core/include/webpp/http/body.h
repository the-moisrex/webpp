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
        using string_view_type = std::string_view;

      protected:
        mutable void* data = nullptr;
        enum class types : int_fast8_t {
            empty,
            string,
            string_view,
            stream
        } mutable type = types::empty;

        [[nodiscard]] string_type const& str_ref() const noexcept;

        [[nodiscard]] string_type& str_ref() noexcept;

        [[nodiscard]] string_view_type const& str_view_ref() const noexcept;

        [[nodiscard]] stream_type& stream_ref() noexcept;

      public:
        body() noexcept = default;

        body(char const* const _str) noexcept
            : data(new std::string(_str)), type(types::string) {}

        body(std::string_view const& str) noexcept
            : data(new std::string(str)), type(types::string) {}

        body(std::ostream& stream) noexcept
            : data(&stream), type(types::stream) {}

        body& operator=(body const& b) noexcept {
            if (this != &b) {
                this->~body();
                type = b.type;
                switch (b.type) {
                case types::empty:
                    break;
                case types::stream:
                    // todo: fix this to be a stream instead of this:
                    replace_string(string_type(b.str("")));
                    break;
                case types::string:
                    replace_string(b.str_ref());
                    break;
                case types::string_view:
                    // we don't know if the string_view will exists that long
                    replace_string(std::string{b.str_view_ref()});
                    break;
                }
            }
            return *this;
        }

        body& operator=(body&& b) noexcept {
            if (this != &b) {
                type = std::move(b.type);
                data = std::move(b.data);
                b.data = nullptr;
            }
            return *this;
        }

        body& operator=(std::string_view const& _str) noexcept {
            replace_string(std::string{_str});
            return *this;
        }

        body& operator=(char const* const _str) noexcept {
            replace_string(_str);
            return *this;
        }

        [[nodiscard]] bool operator==(body const& b) const noexcept {
            return b.str("") == str("");
        }

        [[nodiscard]] bool operator!=(body const& b) const noexcept {
            return !operator==(b);
        }

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
        void replace_string_view(std::string_view const& str) noexcept;

        /**
         * Replace the data with a stream
         * @param stream
         */
        void replace_stream(stream_type& stream) noexcept;

        /**
         * Append a string to the body (weather it's a string ot stream)
         * @param str
         */
        void append_string(std::string_view const& str) noexcept;

        /**
         * Appending a stream
         * @param stream
         */
        void append_stream(stream_type& stream) noexcept;

        /**
         * Get the value as a string (converts the other types to string
         * too)
         * @param default_val the default value if we cannot convert the
         * data to string
         * @return the string representation of the data
         */
        [[nodiscard]] std::string
        str(std::string_view const& default_val = "") const noexcept;

        [[nodiscard]] auto json() const;

        auto json(std::string_view const& data);

        auto file(std::string_view const& filepath);

        std::istream& stream() const;

        std::ostream& operator<<(std::ostream& __stream);

        body& operator<<(std::string_view const& str) noexcept;

        // TODO: add more methods for the images and stuff

        // static auto file(std::string_view const& _file) noexcept;
    };

}; // namespace webpp

#endif // WEBPP_BODY_H
