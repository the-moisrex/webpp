// Created by moisrex on 10/6/21.

#ifndef WEBPP_BROTLI_HPP
#define WEBPP_BROTLI_HPP

#if __has_include(<brotli/encode.h>)
#    include <brotli/decode.h>
#    include <brotli/encode.h>
#    define WEBPP_BROTLI
#endif

#endif // WEBPP_BROTLI_HPP
