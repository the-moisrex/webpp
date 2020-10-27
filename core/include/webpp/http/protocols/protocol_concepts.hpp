// Created by moisrex on 5/9/20.

#ifndef WEBPP_PROTOCOL_CONCEPTS_H
#define WEBPP_PROTOCOL_CONCEPTS_H

#include "../../traits/enable_traits.hpp"

namespace webpp {

    /**
     * Protocol is a "Protocol Type" based on the information that I said in the "server/server_concepts"
     * file.
     */
    template <typename T>
    concept Protocol = requires (T proto) {
      EnabledTraits<T>;
      proto();
      proto.app; // get the app
      // should be able to pass an app to it as well
    };

} // namespace webpp

#endif // WEBPP_PROTOCOL_CONCEPTS_H
