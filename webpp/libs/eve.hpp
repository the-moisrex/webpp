// Created by moisrex on 9/21/20.

#ifndef WEBPP_EVE_HPP
#define WEBPP_EVE_HPP

/**
 * Eve is a project written by "Joel Falcou"
 * It's purpose is to ease the pain of using SIMD in projects like this
 * Github URL: https://github.com/jfalcou/eve
 */

#include "../std/std.hpp"

#ifdef WEBPP_USE_EVE
#    if __has_include(<eve/eve.hpp>)
#        define WEBPP_EVE
#    endif

#    ifdef WEBPP_EVE
namespace eve {}
namespace webpp::eve {
    using namespace ::eve;
} // namespace webpp::eve
#    endif
#endif

#endif // WEBPP_EVE_HPP
