// Created by moisrex on 10/7/20.

#ifndef WEBPP_BROTLI_HPP
#define WEBPP_BROTLI_HPP

#include "../libs/brotli.hpp"

#ifndef WEBPP_BROTLI
#    include <cstdlib>
#    include <stdexcept>
#    include <string>
#endif

namespace webpp {


    /**
     * RFC:                       https://tools.ietf.org/html/rfc7932
     * Google's Implementation:   https://github.com/google/brotli
     * Wikipedia:                 https://en.wikipedia.org/wiki/Brotli
     *
     * Possible wrapper for google's implementation:
     * https://github.com/CHN-beta/brotli-cpp/blob/master/brotli-cpp.hpp
     */
    struct brotli {
#ifdef WEBPP_BROTLI

        static std::string compress(char const* data, const size_t ndata) {
            std::string ret;
            if (ndata == 0) {
                return ret;
            }
            ret.resize(BrotliEncoderMaxCompressedSize(ndata));
            size_t encodedSize{ret.size()};
            auto   r = BrotliEncoderCompress(
              5,
              BROTLI_DEFAULT_WINDOW,
              BROTLI_DEFAULT_MODE,
              ndata,
              (uint8_t const*) (data),
              &encodedSize,
              (uint8_t*) (ret.data()));
            if (r == BROTLI_FALSE) {
                ret.resize(0);
            } else {
                ret.resize(encodedSize);
            }
            return ret;
        }

        static std::string decompress(char const* data, const size_t ndata) {
            if (ndata == 0) {
                return std::string(data, ndata);
            }

            size_t availableIn  = ndata;
            auto   nextIn       = (uint8_t const*) (data);
            auto   decompressed = std::string(availableIn * 3, 0);
            size_t availableOut = decompressed.size();
            auto   nextOut      = (uint8_t*) (decompressed.data());
            size_t totalOut{0};
            bool   done = false;
            auto   s    = BrotliDecoderCreateInstance(nullptr, nullptr, nullptr);
            while (!done) {
                auto result =
                  BrotliDecoderDecompressStream(s, &availableIn, &nextIn, &availableOut, &nextOut, &totalOut);
                if (result == BROTLI_DECODER_RESULT_SUCCESS) {
                    decompressed.resize(totalOut);
                    done = true;
                } else if (result == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) {
                    assert(totalOut == decompressed.size());
                    decompressed.resize(totalOut * 2);
                    nextOut      = (uint8_t*) (decompressed.data() + totalOut);
                    availableOut = totalOut;
                } else {
                    decompressed.resize(0);
                    done = true;
                }
            }
            BrotliDecoderDestroyInstance(s);
            return decompressed;
        }
#else
        static std::string compress(char const* /*data*/, const std::size_t /*ndata*/) {
            throw std::runtime_error(
              "If you do not have the brotli package installed, you cannot use brotliCompress()");
            abort();
        }

        static std::string decompress(char const* /*data*/, const std::size_t /*ndata*/) {
            throw std::runtime_error(
              "If you do not have the brotli package installed, you cannot use brotliDecompress()");
            abort();
        }
#endif
    };

} // namespace webpp

#endif // WEBPP_BROTLI_HPP
