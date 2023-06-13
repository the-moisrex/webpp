// Created by moisrex on 6/8/23.

#ifndef WEBPP_SOCKET_OS_HPP
#define WEBPP_SOCKET_OS_HPP


#include <cstdint>

#if defined(_WIN32)
#    include <winsock2.h>
#    include <ws2tcpip.h>
#    pragma comment(lib, "Ws2_32.lib")

using socket_t = SOCKET;

using socklen_t = int;
using in_port_t = uint16_t;
using in_addr_t = uint32_t;

using sa_family_t = u_short;

#    ifndef _SSIZE_T_DEFINED
#        define _SSIZE_T_DEFINED
#        undef ssize_t
using ssize_t = SSIZE_T;
#    endif // _SSIZE_T_DEFINED

#    ifndef _SUSECONDS_T
#        define _SUSECONDS_T
typedef long suseconds_t; // signed # of microseconds in timeval
#    endif                // _SUSECONDS_T

#    define SHUT_RD   SD_RECEIVE
#    define SHUT_WR   SD_SEND
#    define SHUT_RDWR SD_BOTH

struct iovec {
    void*       iov_base;
    std::size_t iov_len;
};

#else
#    include <arpa/inet.h> // htonl, ...
#    include <fcntl.h>
#    include <sys/socket.h>
#    include <unistd.h> // dup
#    ifdef __FreeBSD__
#        include <netinet/in.h>
#    endif
#    include <cerrno>  // io result
#    include <csignal> // for ignoring the signals
#    include <cstdio>  // perror
#    include <cstdlib> // EXIT_FAILURE
#    include <netdb.h>
#endif


#endif // WEBPP_SOCKET_OS_HPP
