// Created by moisrex on 10/10/20.

#ifndef WEBPP_CRYPTOPP_HPP
#define WEBPP_CRYPTOPP_HPP

#if __has_include(<cryptopp/cryptlib.h>)
#    define WEBPP_CRYPTOPP
#endif

#ifdef WEBPP_CRYPTOPP
#    define cryptopp(file) <cryptopp/file.h>
#else
#    define cryptopp(file) // nothing
#endif

#endif // WEBPP_CRYPTOPP_HPP
