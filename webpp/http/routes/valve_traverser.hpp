// Created by moisrex on 11/6/23.

#ifndef WEBPP_VALVE_TRAVERSER_HPP
#define WEBPP_VALVE_TRAVERSER_HPP

#include "../../std/string.hpp"
#include "../../std/vector.hpp"

namespace webpp::http {

    template <istl::String StrT = stl::string>
    struct valve_descriptor {
        using string_type = StrT;

      private:
        string_type name;
        string_type separator;
    };

    /**
     * Valve Traverser
     *
     * Valves are every segment of each route in a router.
     * Traversing valves means going through the valves one by one.
     *
     * If you pass the context type, the result of that context type will be filled, and it'll be like if you
     * run the route normally with that context; if the context type is void, then it'll go through all of the
     * valves.
     */
    template <typename ContextType = void>
    struct valve_traverser {
        using context_type = ContextType;
        using context_ptr  = context_type*;
        using context_ref  = context_type&;

        constexpr valve_traverser(context_ref inp_ctx) noexcept : ctx{&inp_ctx} {}

        constexpr valve_traverser(valve_traverser const&)            = delete; // stopping accidental copying
        constexpr valve_traverser(valve_traverser&&) noexcept        = default;
        constexpr valve_traverser& operator=(valve_traverser const&) = delete;
        constexpr valve_traverser& operator=(valve_traverser&&) noexcept = default;
        constexpr ~valve_traverser()                                     = default;

        template <typename RouteType>
        constexpr valve_traverser& operator()(RouteType& route) {
            route.traverse(*this);
            return *this;
        }

        template <typename RouteType>
        constexpr valve_traverser& operator()(RouteType const& route) {
            auto tmp_route = route;
            operator()(route);
            return *this;
        }

        template <istl::String StrT = stl::string>
        constexpr void to_string(StrT& out) const {
            // todo
        }

        template <istl::String StrT = stl::string, typename... Args>
            requires((!istl::String<Args> && ...))
        constexpr StrT to_string(Args&&... args) const {
            StrT out{stl::forward<Args>(args)...};
            to_string(out);
            return out;
        }

        template <istl::String StrT = stl::string>
        [[nodiscard]] explicit constexpr operator StrT() const {
            return to_string();
        }

        // Member functions for valves while traversing:

        constexpr void append() {
            // todo
        }

      private:
        context_ptr ctx;
    };

} // namespace webpp::http

#endif // WEBPP_VALVE_TRAVERSER_HPP
