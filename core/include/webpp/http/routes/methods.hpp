#ifndef WEBPP_VALVES_METHODS_H
#define WEBPP_VALVES_METHODS_H

#include "route.hpp"

namespace webpp::http {

    struct method_route_condition : public stl::string_view {
        using stl::string_view::basic_string_view; // ctors

        [[nodiscard]] constexpr bool operator()(HTTPRequest auto const& req) const noexcept {
            return req.request_method() == *this;
        }

        template <istl::String StrT = stl::string>
        void append_name_to(StrT& out) const {
            append_to(out, *this);
        }
    };

    using method = route<method_route_condition>;

    constexpr auto get  = method("GET");
    constexpr auto post = method("POST");
    constexpr auto head = method("HEAD");
    constexpr auto put  = method("PUT");
    // TODO: add more methods

} // namespace webpp::http


#endif // WEBPP_VALVES_METHODS_H
