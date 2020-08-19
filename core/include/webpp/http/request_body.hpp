// Created by moisrex on 8/19/20.

#ifndef WEBPP_REQUEST_BODY_HPP
#define WEBPP_REQUEST_BODY_HPP

#include "../extensions/extension.hpp"


namespace webpp {

    /**
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
     */
    template <Traits TraitsType, typename EList = empty_extension_pack>
    struct request_body : public EList {
        using traits_type      = TraitsType;
        using string_type      = typename traits_type::string_type;
        using string_view_type = typename traits_type::string_view_type;
        using elist_type       = EList;

        template <typename... Args>
        constexpr request_body(Args&&... args) noexcept : elist_type{stl::forward<Args>(args)...} {}
    };

    struct request_body_descriptor {

        template <typename ExtensionType>
        struct has_related_extension_pack {
            static constexpr bool value = requires {
                typename ExtensionType::request_body_extensions;
            };
        };

        template <typename ExtensionType>
        using related_extension_pack_type = typename ExtensionType::request_body_extensions;

        template <typename ExtensionListType, typename TraitsType, typename EList>
        using mid_level_extensie_type = request_body<TraitsType, EList>;

        // empty final extensie
        template <typename ExtensionListType, typename TraitsType, typename EList>
        using final_extensie_type = EList;
    };

    template <Traits TraitsType, Extension... E>
    using simple_request_body =
      typename extension_pack<E...>::template extensie_type<TraitsType, request_body_descriptor>;


} // namespace webpp

#endif // WEBPP_REQUEST_BODY_HPP
