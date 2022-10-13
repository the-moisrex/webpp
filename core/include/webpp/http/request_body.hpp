// Created by moisrex on 8/19/20.

#ifndef WEBPP_REQUEST_BODY_HPP
#define WEBPP_REQUEST_BODY_HPP

#include "../extensions/extension.hpp"
#include "http_concepts.hpp"


namespace webpp::http {

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
    template <typename TraitsType, typename EList>
    struct request_body : public EList {
        using traits_type                 = TraitsType;
        using request_body_extension_list = EList;

        template <typename... Args>
        constexpr request_body(Args&&... args) noexcept
          : request_body_extension_list{stl::forward<Args>(args)...} {}
    };

    struct request_body_descriptor {

        template <typename ExtensionType>
        using extractor_type = typename ExtensionType::request_body_extensions;

        // template <typename RootExtensions, typename TraitsType, typename BEList>
        // using mid_level_extensie_type = request_body<TraitsType, BEList>;
    };

    template <Traits TraitsType, RootExtensionList RootExtensions>
    using simple_request_body =
      typename RootExtensions::template extensie_type<TraitsType, request_body_descriptor>;


} // namespace webpp::http

#endif // WEBPP_REQUEST_BODY_HPP
