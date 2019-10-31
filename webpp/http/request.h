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

    template <typename Interface>
    class request_t : public basic_request_t {};
} // namespace webpp

#endif // WEBPP_REQUEST_H
