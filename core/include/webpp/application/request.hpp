#ifndef WEBPP_REQUEST_H
#define WEBPP_REQUEST_H

#include "webpp/http/headers.hpp"
#include "webpp/http/protocols/protocol_concepts.hpp"
#include "webpp/http/request_concepts.hpp"
#include "webpp/http/response_body.hpp"
#include "webpp/traits/traits.hpp"


/**
 * Only the protocol should be instantiating this class. There should be no
 * need for anybody else to do it.
 *
 * The user has access to this class. This class should be accessed as a const
 * variable since the user should not be able to make any changes here. Every
 * action that the user has to make should be in the "response" class or other
 * relative classes that finally end up in that class. This class is only for
 * giving information that the user or other modules need.
 *
 */

namespace webpp {

    /**
     * If you want to add features to all of the request types, you can use this type
     */
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


    template <Traits TraitsType,
              typename EList,
              template <typename, typename, typename...>
              typename MidLevelRequestType,
              typename... MidLevelRequestTemplateArgs>
    using simple_request = typename EList::template extensie_type<
      TraitsType,
      request_descriptor<MidLevelRequestType, MidLevelRequestTemplateArgs...>>;


} // namespace webpp

#endif // WEBPP_REQUEST_H
