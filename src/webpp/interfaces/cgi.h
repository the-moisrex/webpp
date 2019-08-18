#ifndef WEBPP_CGI_H
#define WEBPP_CGI_H

#include "../http/body.h"
#include "../http/header.h"
#include "../router.h"
#include "../std/string_view.h"
#include <string>

namespace webpp {

    class cgi {
      public:
        using header_type = std::shared_ptr<webpp::header<cgi>>;
        using body_type = std::shared_ptr<webpp::body<cgi>>;

      private:
      public:
        cgi();
        char const* header(std::string h) const noexcept;
        header_type headers() noexcept;
        body_type body() noexcept;
        char const* server_addr() const noexcept;
        int server_port() const noexcept;
        char const* remote_addr() const noexcept;
        int remote_port() const noexcept;
        char const* server_name() const noexcept;
        char const* scheme() const noexcept;
        char const* server_protcol() const noexcept;
        char const* method() const noexcept;
        char const* request_uri() const noexcept;

        void run(::webpp::router& _router) noexcept;
        char const* env(char const* const name) const noexcept;
        std::streamsize read(char* data, std::streamsize length) const;
    };

} // namespace webpp

#endif // WEBPP_CGI_H
