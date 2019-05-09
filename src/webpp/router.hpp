#ifndef WEBPP_ROUTER_H
#define WEBPP_ROUTER_H

#include <functional>
#include <string>
#include <vector>

namespace webpp {

    class route {
      public:
        bool active = false;
        ::std::string method;
        ::std::string slug;
        ::std::vector<route> subroutes;

        route(::std::string method, ::std::string slug)
            : method(method), slug(slug) {}
    };

    class router {
        ::std::vector<route> m_routes;

      public:
        using callback = ::std::function<void()>;
        using middleware = ::std::function<void()>;
        router& use(route new_route, callback _callback,
                    ::std::vector<middleware> _middlewares = {});
    };

}; // namespace webpp

#endif // WEBPP_ROUTER_H
