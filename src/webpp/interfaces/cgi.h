#ifndef WEBPP_CGI_H
#define WEBPP_CGI_H

#include "../router.hpp"
#include "../http/headers.hpp"

namespace webpp {

    class cgi {
      private:
      public:
        cgi();
        char const* header(char const* const) const noexcept;
        ::webpp::headers headers() const noexcept;
        char const* server_addr() const noexcept;
        int server_port() const noexcept;
        char const* remote_addr() const noexcept;
        int remote_port() const noexcept;
        char const* server_name() const noexcept;
        char const* scheme() const noexcept;
        char const* server_protcol() const noexcept;
        char const* method() const noexcept;
        void run(::webpp::router const& _router) noexcept;
        char const* env(char const* const &name) const noexcept;
    };

} // namespace webpp

#endif // WEBPP_CGI_H
