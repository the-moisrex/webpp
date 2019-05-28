#ifndef SESSION_H
#define SESSION_H

/**
 *
 * TODO:
 *    - [ ] Flash data
 *    - [ ] Long live data
 *    - [ ] Timed data
 *
 *
 * Possible Client Adapters to implement:
 *    - [ ] In Cookies
 *    - [ ] In HTTP-Headers
 *    - [ ] In GET Method
 *    - [ ] In All Methods
 *    - [ ] Combination of all above ways
 *
 *
 * Possible Server Adapters to implement:
 *    - [ ] In predefined file
 *    - [ ] In cutsom file
 *    - [ ] In memory (for long lived versions of builds like FastCGI)
 *    - [ ] In cookies
 *    - [ ] In predefined database
 *    - [ ] In custom database
 */

namespace webpp {

    template <class Adapter>
    class session {
      public:
        session();
    };

} // namespace webpp

#endif // SESSION_H
