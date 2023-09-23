// Created by moisrex on 9/22/23.

#ifndef WEBPP_URING_HPP
#define WEBPP_URING_HPP

#ifdef __linux__
#    if 1 || defined(USE_LIBURING) && __has_include(<liburing.h>)
#        include <liburing.h>
#    else
#        include "./liburing/liburing.hpp"
#    endif
#    define WEBPP_IO_URING_SUPPORT
#endif

#endif // WEBPP_URING_HPP
