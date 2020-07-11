#ifndef WEBPP_COMMON_DEBUG_H
#define WEBPP_COMMON_DEBUG_H

// see this tweet: https://twitter.com/incomputable/status/1247235812271497216
#ifdef NDEBUG
#    undef assert
#    define assert(c)                \
        if (c) {                     \
        } else {                     \
            __builtin_unreachable(); \
        }
#endif

#endif // WEBPP_COMMON_DEBUG_H
