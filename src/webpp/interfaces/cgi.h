#ifndef WEBPP_CGI_H
#define WEBPP_CGI_H

#include "../http/body.h"
#include "../http/headers.hpp"
#include "../router.hpp"
#include "../std/string_view.h"
#include <string>

namespace webpp {

    class cgi {
      public:
        using header_type = webpp::headers<cgi>;
        using body_type = webpp::body<cgi>;

      private:
      public:
        cgi();
        std::string_view header(std::string h) const noexcept;
        header_type headers() noexcept;
        body_type body() noexcept;
        std::string_view server_addr() const noexcept;
        int server_port() const noexcept;
        std::string_view remote_addr() const noexcept;
        int remote_port() const noexcept;
        std::string_view server_name() const noexcept;
        std::string_view scheme() const noexcept;
        std::string_view server_protcol() const noexcept;
        std::string_view method() const noexcept;
        std::string_view request_uri() const noexcept;

        void run(::webpp::router const& _router) noexcept;
        std::string_view env(std::string_view name) const noexcept;
        size_t read(char* data, size_t length) const;
    };

} // namespace webpp

#endif // WEBPP_CGI_H
