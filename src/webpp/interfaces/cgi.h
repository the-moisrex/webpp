#ifndef WEBPP_CGI_H
#define WEBPP_CGI_H

namespace webpp {

    class cgi {
      private:
      public:
        cgi();
        void run() noexcept;
        char const* env(char const* const& name = nullptr) const noexcept;
    };

} // namespace webpp

#endif // WEBPP_CGI_H
