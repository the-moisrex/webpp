#ifndef WEBPP_COOKIES_H
#define WEBPP_COOKIES_H

#include <string>

namespace webpp {

    class cookie {
      private:
        std::string name = "";
        std::string value = "";
        std::string comment = "";
        std::string domain = "";
        std::string path = "/";
        unsigned long max_age;
        bool secure = false;

      public:
        /**
         * empty cookie
         */
        cookie() = default;
    };

} // namespace webpp

#endif // WEBPP_COOKIES_H
