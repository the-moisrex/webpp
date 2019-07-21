#ifndef WEBPP_CGI_H
#define WEBPP_CGI_H

#include "../router.hpp"
#include "../http/headers.hpp"
#include "../http/body.h"
#include "../std/string_view.h"
#include <string>

namespace webpp {

    class cgi {
      private:
      public:
        cgi();
        char const* header(std::string h) const noexcept;
        ::webpp::headers headers() const noexcept;
        ::webpp::body body() const noexcept;
        char const* server_addr() const noexcept;
        int server_port() const noexcept;
        char const* remote_addr() const noexcept;
        int remote_port() const noexcept;
        char const* server_name() const noexcept;
        char const* scheme() const noexcept;
        char const* server_protcol() const noexcept;
        char const* method() const noexcept;
        char const* request_uri() const noexcept;
        
        void run(::webpp::router const& _router) noexcept;
        char const* env(std::string_view name) const noexcept;
        size_t read(char* data, size_t length) const;
    };

} // namespace webpp

#endif // WEBPP_CGI_H
