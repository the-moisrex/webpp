#ifndef WEBPP_BODY_H
#define WEBPP_BODY_H

#include "../extensions/extension.h"

#include <memory>
#include <string>
#include <string_view>
#include <variant>

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

    template <Traits TraitsType, ExtensionList EList = empty_extension_pack>
    class response_body : public EList {
      private:
        template <typename ExtensionType>
        using variant_extractor =
          typename ExtensionType::response_body_extensions;

        template <typename ExtensionType>
        struct has_variant {
            static constexpr bool value = requires {
                typename ExtensionType::response_body_extensions;
            };
        };

        using extensions_that_has_variants =
          typename EList::template filter<stl::variant, has_variant, EList>;

      public:
        using extension_list = EList;
        using variant_type   = typename EList::template epack_miner<
          stl::variant, variant_extractor, extensions_that_has_variants>::type;

      public:
        using EList::EList;

        response_body() noexcept = default;

        /**
         * Get the value as a string (converts the other types to string
         * too)
         * @param default_val the default value if we cannot convert the
         * data to string
         * @return the string representation of the data
         */
        [[nodiscard]] stl::string
        str(stl::string_view const& default_val = "") const noexcept;
    };


    struct response_body_descriptor {
        template <typename ExtensionType>
        struct has_related_extension_pack {
            static constexpr bool value = requires {
                typename ExtensionType::response_body_extensions;
            };
        };

        template <typename ExtensionType>
        using related_extension_pack_type =
          typename ExtensionType::response_body_extensions;

        template <typename ExtensionListType, typename TraitsType,
                  typename EList>
        using mid_level_extensie_type = response_body<TraitsType, EList>;

        // empty final extensie
        template <typename ExtensionListType, typename TraitsType,
                  typename EList>
        struct final_extensie_type final : public EList {
            using EList::EList;
        };
    };

}; // namespace webpp

#endif // WEBPP_BODY_H
