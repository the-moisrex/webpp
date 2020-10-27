#ifndef WEBPP_REQUEST_H
#define WEBPP_REQUEST_H


/**
 *
 * What request class needs:
 *
 * - [ ] The INTERFACE:
 *       Because otherwise, the interface class itself has to instantiate this
 *       class and copy/move all the data over to here. So in that situation
 *       the interface has to process all the functionalities that this class
 *       can provide. But that's not what we want because there will be a lot of
 *       things that the user may not need in order to create and pass a
 *       response.
 * - [ ] The headers
 * - [ ] The body
 *
 *
 * Only the interface should be instantiating this class. There should be no
 * need for anybody else to do it.
 *
 * The user has access to this class. This class should be accessed as a const
 * variable since the user should not be able to make any changes here. Every
 * action that the user has to make should be in the "response" class or other
 * relative classes that finally end up in that class. This class is only for
 * giving informations that the user or other modules need.
 *
 */

#include "../traits/traits_concepts.hpp"
#include "./headers.hpp"
#include "./request_concepts.hpp"
#include "./response_body.hpp"
#include "./protocols/protocol_concepts.hpp"

namespace webpp {

    template <typename ReqType>
    using final_request = ReqType;

    template <template <typename, typename, typename...> typename MidLevelRequestType,
              typename... AdditionalReqArgs>
    struct request_descriptor {
        template <typename ExtensionType>
        struct has_related_extension_pack {
            static constexpr bool value = requires {
                typename ExtensionType::request_extensions;
            };
        };

        template <typename ExtensionType>
        using related_extension_pack_type = typename ExtensionType::request_extensions;

        template <typename ExtensionListType, typename TraitsType, typename EList>
        using mid_level_extensie_type = MidLevelRequestType<TraitsType, EList, AdditionalReqArgs...>;

        // empty final extensie
        template <typename ExtensionListType, typename TraitsType, typename EList>
        using final_extensie_type = final_request<EList>;
    };


    template <Traits    TraitsType, template <typename, typename, typename...> typename MidLevelRequestType,
              Protocol InterfaceType, typename EList>
    using simple_request =
      typename EList::template extensie_type<TraitsType,
                                             request_descriptor<MidLevelRequestType, InterfaceType>>;

} // namespace webpp

#endif // WEBPP_REQUEST_H
