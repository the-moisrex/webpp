#ifndef WEBPP_ROUTER_H
#define WEBPP_ROUTER_H

#include <vector>
#include <functional>
#include <string>

namespace webpp {

class route {
  public:
    using method = std::string;
    using path = std::string;

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


#endif // WEBPP_ROUTER_H
