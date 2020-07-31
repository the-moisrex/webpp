// Created by moisrex on 7/30/20.

#ifndef WEBPP_ROUTER_STATS_HPP
#define WEBPP_ROUTER_STATS_HPP

namespace webpp::inline extensions {
    struct router_stats {
        enum class route_level : stl::uint8_t {
            none              = 0x0u,
            entryroute        = 0x1u,
            subroute          = 0x2u,
            internal_subroute = 0x3u,
        };

        enum class skip_next : stl::uint8_t {
            none               = 0x0u,
            entryroutes        = 0x1u,
            subroutes          = 0x2u,
            internal_subroutes = 0x3u,
        };

        // we specify the bit fields to reduce the padding
        routes_size entryroute_index           = 0;
        routes_size subroute_index             = 0;
        routes_size internal_subroute_index    = 0;
        bool        is_last_entryroute : 1        = false;
        bool        is_last_subroute : 1          = false;
        bool        is_last_internal_subroute : 1 = false;
        skip_next   skip : 2                   = skip_next::none;
        route_level level : 2                  = route_level::none;

    };
}

#endif // WEBPP_ROUTER_STATS_HPP
