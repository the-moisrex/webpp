#ifndef WEBPP_BODY_H
#define WEBPP_BODY_H

#include "../extensions/extension.hpp"


namespace webpp {

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
    template <Traits TraitsType, typename EList = empty_extension_pack>
    struct response_body : public EList {
        using traits_type      = TraitsType;
        using string_type      = typename traits_type::string_type;
        using string_view_type = typename traits_type::string_view_type;
        using elist_type       = EList;
        /*
              private:
                template <typename ExtensionType>
                using variant_extractor = typename ExtensionType::response_body_extensions;

                template <typename ExtensionType>
                struct has_variant {
                    static constexpr bool value = requires {
                        typename ExtensionType::response_body_extensions;
                    };
                };

                using extensions_that_has_variants =
                  typename EList::template filter<stl::variant, has_variant, EList>;

              public:
                using variant_type   = typename EList::template epack_miner<stl::variant, variant_extractor,
                                                                          extensions_that_has_variants>::type;
        */
        template <typename... Args>
        constexpr response_body(Args&&... args) noexcept : elist_type{stl::forward<Args>(args)...} {}
    };


    struct response_body_descriptor {
        template <typename ExtensionType>
        struct has_related_extension_pack {
            static constexpr bool value = requires {
                typename ExtensionType::response_body_extensions;
            };
        };

        template <typename ExtensionType>
        using related_extension_pack_type = typename ExtensionType::response_body_extensions;

        template <typename ExtensionListType, typename TraitsType, typename EList>
        using mid_level_extensie_type = response_body<TraitsType, EList>;

        // empty final extensie
        template <typename ExtensionListType, typename TraitsType, typename EList>
        using final_extensie_type = EList;
    };

    template <Traits TraitsType, Extension... E>
    using simple_response_body =
      typename extension_pack<E...>::template extensie_type<TraitsType, response_body_descriptor>;

}; // namespace webpp

#endif // WEBPP_BODY_H
