// Created by moisrex on 9/8/23.

#ifndef WEBPP_OS_HPP
#define WEBPP_OS_HPP

#if defined(_WIN32) || defined(_WIN64)
#    define WINDOWS_SYSTEM
#else
#    define UNIX_SYSTEM
#endif


#ifdef _MSC_VER
#    define MSVC_COMPILER
#    define OS_VALUE(unix, windows) windows
#else
#    define OS_VALUE(unix, windows) unix
#endif


#endif // WEBPP_OS_HPP
