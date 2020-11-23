// Created by moisrex on 5/9/20.

#ifndef WEBPP_PROTOCOL_CONCEPTS_H
#define WEBPP_PROTOCOL_CONCEPTS_H

#include "../../traits/enable_traits.hpp"
#include "../application_concepts.hpp"

namespace webpp {

    /**
     * Protocol is a "Protocol Type" based on the information that I said in the "server/server_concepts"
     * file.
     */
    template <typename T>
    concept Protocol = requires (T proto) {
        requires EnabledTraits<T>;
        requires Request<typename T::request_type>;
        requires Application<typename T::application_type>;
        requires ApplicationWrapper<typename T::app_wrapper_type>;
        { proto.app() } -> ApplicationWrapper; // get the app
        // should be able to pass an app to it as well

        {proto.is_ssl_available()} -> stl::same_as<bool>;
        {proto()} -> stl::same_as<int>;
    };

} // namespace webpp

#endif // WEBPP_PROTOCOL_CONCEPTS_H
