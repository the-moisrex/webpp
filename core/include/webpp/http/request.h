#ifndef WEBPP_REQUEST_H
#define WEBPP_REQUEST_H

#include "../interfaces/basic_interface.h"
#include "body.h"
#include "header.h"

#include <string_view>

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

namespace webpp {

    class basic_request_t {
      private:
      public:
    };

    /**
     * This class doesn't own its data (at least the ones that are important)
     * @tparam Interface
     */
    template <Traits TraitsT, Interface InterfaceT>
    struct request_t : public basic_request_t {
        using traits    = TraitsT;
        using interface = InterfaceT;
    };

    template <typename T>
    concept Request = requires(T x) {
        typename T::traits;
        Traits<typename T::traits>;

        typename T::interface;
        Interface<typename T::interface>;
    };
} // namespace webpp

#endif // WEBPP_REQUEST_H
