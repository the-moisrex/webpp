#ifndef WEBPP_WEBPP_H
#define WEBPP_WEBPP_H

#include <vector>
#include <functional>
#include <string>

namespace webpp {

class route {

};

class router {
  ::std::vector<route> m_routes;
public:
  using callback = ::std::function<void()>;
  using middleware = ::std::function<void()>;
  using method = ::std::string;
  router& use(method _method, route new_route, callback _callback, ::std::vector<middleware> _middlewares={});
  router& get(route new_route, callback _callback, ::std::vector<middleware> _middlewares={});
  router& post(route new_route, callback _callback, ::std::vector<middleware> _middlewares={});
  router& del(route new_route, callback _callback, ::std::vector<middleware> _middlewares={});
};

};


#endif // WEBPP_WEBPP_H
