#ifndef WEBPP_REQUEST_H
#define WEBPP_REQUEST_H

#include "../http/headers.hpp"
#include "../http/protocols/protocol_concepts.hpp"
#include "../http/request_concepts.hpp"
#include "../http/response_body.hpp"
#include "../traits/traits.hpp"
#include "../version.hpp"


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
    template <Traits TraitsType, typename ReqType>
    struct final_request final : public ReqType {
        using traits_type      = TraitsType;
        using string_view_type = traits::string_view<traits_type>;

        using ReqType::ReqType;

        /**
         * Get the Web++ Library version
         */
        [[nodiscard]] string_view_type version() const noexcept {
            return webpp_version;
        }
    };

    template <template <typename, typename, typename...> typename MidLevelRequestType,
              typename... AdditionalReqArgs>
    struct request_descriptor {
        template <typename ExtensionType>
        using extractor_type = typename ExtensionType::request_extensions;

        template <RootExtensionList RootExtensions, Traits TraitsType, typename RequestEList>
        using mid_level_extensie_type = MidLevelRequestType<TraitsType, RequestEList, AdditionalReqArgs...>;

        // empty final extensie
        template <RootExtensionList RootExtensions, Traits TraitsType, typename MidLevelRequestWithExtensions>
        using final_extensie_type = final_request<TraitsType, MidLevelRequestWithExtensions>;
    };


    template <Traits        TraitsType,
              ExtensionList RootExtensions,
              template <typename, typename, typename...>
              typename MidLevelRequestType,
              typename... MidLevelRequestTemplateArgs>
    using simple_request = typename RootExtensions::template extensie_type<
      TraitsType,
      request_descriptor<MidLevelRequestType, MidLevelRequestTemplateArgs...>>;


} // namespace webpp

#endif // WEBPP_REQUEST_H
