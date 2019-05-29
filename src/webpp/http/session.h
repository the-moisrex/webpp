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
 *    - [ ] In a folder
 *    - [ ] In predefined file (your own thing)
 *    - [ ] In cutsom file (json, xml, Excel, ...)
 *    - [ ] In memory (for long lived versions of builds like FastCGI)
 *    - [ ] In cookies (encrypted or plain text)
 *    - [ ] In predefined database (sqlite)
 *    - [ ] In custom database (User configured database)
 */

namespace webpp {

    template <class Adapter>
    class session {
      public:
        session();
    };

} // namespace webpp

#endif // SESSION_H
