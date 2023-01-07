#ifndef WEBPP_RESPONSE_BODY_HPP
#define WEBPP_RESPONSE_BODY_HPP

#include "../extensions/extension.hpp"
#include "../std/functional.hpp"
#include "http_concepts.hpp"

namespace webpp::http {


    template <Traits TraitsType>
    struct default_response_body_communicator {
        using traits_type   = TraitsType;
        using char_type     = traits::char_type<traits_type>;
        using function_type = istl::function<void(char_type const* out_ptr)>;

      private:
      public:
    };


    /**
     * There are two types of bodies:
     *   - The request body
     *   - The response body
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
    template <Traits TraitsType, typename EList>
    struct response_body : public EList {
        using traits_type = TraitsType;
        using elist_type  = EList;
        using char_type   = traits::char_type<traits_type>;

        using EList::EList;

        constexpr response_body() requires(stl::is_default_constructible_v<elist_type>) = default;

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)

        template <EnabledTraits ET>
        requires(stl::is_constructible_v<elist_type, ET>) constexpr response_body(ET&& et) noexcept(
          stl::is_nothrow_constructible_v<elist_type, ET>)
          : elist_type{et} {}


        template <EnabledTraits ET>
        constexpr response_body([[maybe_unused]] ET&&) noexcept(
          stl::is_nothrow_default_constructible_v<elist_type>)
          : elist_type{} {}

        // NOLINTEND(bugprone-forwarding-reference-overload)


        // Get the data pointer if available, returns nullptr otherwise
        [[nodiscard]] constexpr char_type const*
        data() const noexcept requires TextBasedBodyCommunicator<elist_type> {
            return elist_type::data();
        }

        // Get the size of the response body if possible. returns 0 if it's not available
        [[nodiscard]] constexpr stl::size_t
        size() const noexcept requires TextBasedBodyCommunicator<elist_type> {
            return elist_type::size();
        }

        constexpr stl::streamsize write(char_type const* data, stl::streamsize count) {
            if constexpr (requires { elist_type::write(data, count); }) {
                return elist_type::write(data, count);
            } else {
                // todo
                return 0;
            }
        }

        [[nodiscard]] constexpr bool operator==(response_body const& body) const noexcept {
            if constexpr (requires { elist_type::operator==(body); }) {
                return elist_type::operator==(body);
            } else {
                const auto this_size = size();
                return this_size == body.size() && stl::equal(data(), data() + this_size, body.data());
            }
        }


        [[nodiscard]] constexpr bool operator!=(response_body const& body) const noexcept {
            return !operator==(body);
        }




        template <typename T>
        constexpr T as() const {
            if constexpr (requires {
                              { deserialize_response_body<T>(*this) } -> stl::same_as<T>;
                          }) {
                return deserialize_response_body<T>(*this);
            } else if constexpr (requires {
                                     { deserialize_body<T>(*this) } -> stl::same_as<T>;
                                 }) {
                return deserialize_body<T>(*this);
            } else {
                static_assert_false(T,
                                    "We don't know how to convert the request body to the specified type."
                                    " Did you import the right header?"
                                    " You can always write your own custom body (de)serializer functions.");
            }
        }

        constexpr auto as() const {
            return auto_converter<response_body>{.obj = *this};
        }


        template <typename T>
        constexpr response_body& set(T&& obj) {
            if constexpr (requires { elist_type::template set<T>(stl::forward<T>(obj)); }) {
                elist_type::template set<T>(stl::forward<T>(obj));
            } else if constexpr (requires { elist_type::template operator=<T>(stl::forward<T>(obj)); }) {
                elist_type::template operator=<T>(stl::forward<T>(obj));
            } else if constexpr (requires { serialize_response_body<T>(stl::forward<T>(obj), *this); }) {
                serialize_response_body<T>(stl::forward<T>(obj), *this);
            } else if constexpr (requires { serialize_body<T>(stl::forward<T>(obj), *this); }) {
                serialize_body<T>(stl::forward<T>(obj), *this);
            } else {
                static_assert_false(T,
                                    "We don't know how to convert the specified object to a response body."
                                    " Did you import the right header?"
                                    " You can always write your own custom body (de)serializer functions.");
            }
            return *this;
        }

        template <typename T>
        constexpr response_body& operator=(T&& obj) {
            set(stl::forward<T>(obj));
            return *this;
        }
    };


    struct response_body_descriptor {

        template <typename ExtensionType>
        using extractor_type = typename ExtensionType::response_body_extensions;

        template <typename RootExtensions, typename TraitsType, typename EList>
        using mid_level_extensie_type = response_body<TraitsType, EList>;
    };

    template <Traits TraitsType, Extension... E>
    using simple_response_body =
      typename extension_pack<E...>::template extensie_type<TraitsType, response_body_descriptor>;

} // namespace webpp::http

#endif // WEBPP_RESPONSE_BODY_HPP
