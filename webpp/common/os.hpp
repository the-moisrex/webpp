// Created by moisrex on 9/8/23.

#ifndef WEBPP_OS_HPP
#define WEBPP_OS_HPP

#ifdef _WIN32
#    define OS_VALUE(unix, windows) windows
#else
#    define OS_VALUE(unix, windows) unix
#endif


#endif // WEBPP_OS_HPP
