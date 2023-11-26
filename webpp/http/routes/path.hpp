// Created by moisrex on 4/7/23.

#ifndef WEBPP_HTTP_ROUTES_PATH_HPP
#define WEBPP_HTTP_ROUTES_PATH_HPP

#include "valves.hpp" // implementation

namespace webpp::http {

    static constexpr struct root_valve : valve<root_valve> {
        template <Traits TraitsType>
        [[nodiscard]] constexpr bool operator()(basic_context<TraitsType>& ctx) const noexcept {
            return ctx.path_traverser().at_beginning();
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" root");
        }
    } root;


} // namespace webpp::http

#endif // WEBPP_HTTP_ROUTES_PATH_HPP
