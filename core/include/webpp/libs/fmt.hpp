// Created by moisrex on 10/10/20.

#ifndef WEBPP_FMT_HPP
#define WEBPP_FMT_HPP

#if __has_include(<fmt/format.h>)
#    define WEBPP_FMT
#endif

#ifdef WEBPP_FMT
#    define fmt(file) <fmt/file.h>
#else
#    define fmt(file) // nothing, we actually wnat to make an error
#endif

#endif // WEBPP_FMT_HPP
