#ifndef WEBPP_WEBPP_H
#define WEBPP_WEBPP_H

#include <vector>
#include <functional>
#include <string>

namespace webpp {

class route {
  public:
    using method = std::string;
    using path = std::string;

  private:
    method m_method;
    path m_path;
};

class router {
  ::std::vector<route> m_routes;
public:
  using callback = ::std::function<void()>;
  using middleware = ::std::function<void()>;
  router& use(route new_route, callback _callback, ::std::vector<middleware> _middlewares={});
};

};


#endif // WEBPP_WEBPP_H
