// Created by moisrex on 10/8/20.

#ifndef WEBPP_CASSERT_HPP
#define WEBPP_CASSERT_HPP

#include <cassert>

// see this tweet: https://twitter.com/incomputable/status/1247235812271497216
#ifdef NDEBUG
#    undef assert
#    define assert(c)                \
        if (c) {                     \
        } else {                     \
            __builtin_unreachable(); \
        }
#endif

#endif // WEBPP_CASSERT_HPP
