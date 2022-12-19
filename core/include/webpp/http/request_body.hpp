// Created by moisrex on 8/19/20.

#ifndef WEBPP_REQUEST_BODY_HPP
#define WEBPP_REQUEST_BODY_HPP

#include "../extensions/extension.hpp"
#include "http_concepts.hpp"


namespace webpp::http {

    /**
     * Request body
     *
     * All request bodies will come from here, you can add all the nice features for the request body here.
     */
    template <typename TraitsType, HTTPRequestBodyCommunicator Communicator, typename EList>
    struct request_body : public EList, public Communicator {
        using traits_type                 = TraitsType;
        using request_body_extension_list = EList;
        using request_body_communicator   = Communicator; // the way that the Protocol gives us the body
        using char_type                   = traits::char_type<traits_type>;
        using size_type                   = stl::size_t;

        template <typename... Args>
        constexpr request_body(Args&&... args) noexcept
          : request_body_extension_list{stl::forward<Args>(args)...} {}


        /**
         * Read the body; this member function should be used by the custom bodies to fill their own bellies
         * with the goodies that the user sends in the request body.
         * This function is intentionally designed like POSIX "read" function.
         */
        constexpr size_type read(char_type* data, size_type count) const noexcept {
            return request_body_communicator::read(data, count);
        }
    };

    template <typename ServerType>
    struct request_body_descriptor {
        static_assert(HTTPRequestBodyCommunicator<typename ServerType::request_body_communicator>,
                      "The specified ServerType doesn't seem to have a request body communicator; "
                      "the Protocol is not providing the correct types.");

        template <typename ExtensionType>
        using extractor_type = typename ExtensionType::request_body_extensions;

        template <typename RootExtensions, typename TraitsType, typename BEList>
        using mid_level_extensie_type =
          request_body<TraitsType, typename ServerType::request_body_communicator, BEList>;
    };

    template <Traits TraitsType, typename ServerType>
    using simple_request_body =
      typename ServerType::root_extensions::template extensie_type<TraitsType,
                                                                   request_body_descriptor<ServerType>>;


} // namespace webpp::http

#endif // WEBPP_REQUEST_BODY_HPP
