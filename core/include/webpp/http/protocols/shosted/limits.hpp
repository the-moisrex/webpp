// Created by moisrex on 10/24/20.

#ifndef WEBPP_LIMITS_HPP
#define WEBPP_LIMITS_HPP

#include "../../../std/std.hpp"
#include <cstdint>

namespace webpp::shosted {

    struct limits_type {
        stl::uint16_t uri = 255;
        struct body_limits {
            stl::uint16_t get_method     = 8 * 1024;        // 8KiB
            stl::size_t   post_method    = 1 * 1024 * 1024; // 1MiB
        } body;
    };

}

#endif // WEBPP_LIMITS_HPP
