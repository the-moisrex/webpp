// Created by moisrex on 9/21/20.

#ifndef WEBPP_EVE_HPP
#define WEBPP_EVE_HPP

/**
 * Eve is a project written by "Joel Falcou"
 * It's purpose is to ease the pain of using SIMD in projects like this
 * Github URL: https://github.com/jfalcou/eve
 */

#include "../std/std.hpp"

#if __has_include(<eve/eve.hpp>)
#    define WEBPP_EVE
#endif

#ifdef WEBPP_EVE
#    define eve(eve_file) <eve/eve_file.hpp>
#else
#    define eve(eve_file) // nothing
#endif

#ifdef WEBPP_EVE
namespace eve {}
namespace webpp::eve {
    using namespace ::eve;
} // namespace webpp::eve
#endif

#endif // WEBPP_EVE_HPP
