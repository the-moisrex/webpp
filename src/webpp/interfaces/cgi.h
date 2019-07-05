#ifndef WEBPP_CGI_H
#define WEBPP_CGI_H

#include "../router.hpp"

namespace webpp {

    class cgi {
      private:
      public:
        cgi();
        void run(::webpp::router const& _router) noexcept;
        char const* env(char const* const& name = nullptr) const noexcept;
    };

} // namespace webpp

#endif // WEBPP_CGI_H
