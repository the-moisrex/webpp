#ifndef WEBPP_VALVES_METHODS_H
#define WEBPP_VALVES_METHODS_H

#include "route.hpp"

namespace webpp {

    struct method_route_condition {
      private:
        const stl::string_view method_string;

      public:
        constexpr method_route_condition(stl::string_view str) noexcept : method_string(stl::move(str)) {}

        constexpr method_route_condition() noexcept = default;

        [[nodiscard]] bool operator()(Context auto const& ctx) const noexcept {
            return ctx.request.request_method() == method_string;
        }
    };

    using method = route<method_route_condition>;

    constexpr auto get  = method("GET");
    constexpr auto post = method("POST");
    constexpr auto head = method("HEAD");
    constexpr auto put  = method("PUT");
    // TODO: add more methods

} // namespace webpp


#endif // WEBPP_VALVES_METHODS_H
