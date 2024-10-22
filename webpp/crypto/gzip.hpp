// Created by moisrex on 10/7/20.

#ifndef WEBPP_GZIP_HPP
#define WEBPP_GZIP_HPP

#include "../libs/zlib.hpp"
#include "../std/string.hpp"

#include <cassert>

namespace webpp {

    struct gzip {
        /**
         * Compress the input with gzip
         * Original source from: drogon project
         **/
        template <istl::String StrT = stl::string>
        static StrT compress(
#ifdef ZLIB_CONST
          typename StrT::const_pointer
#else
          typename StrT::pointer
#endif
                            data,
          stl::size_t const ndata) {
            using string_type = StrT;

            z_stream strm =
              {nullptr, 0, 0, nullptr, 0, 0, nullptr, nullptr, nullptr, nullptr, nullptr, 0, 0, 0};
            if (data && ndata > 0) {
                if (deflateInit2(&strm,
                                 Z_DEFAULT_COMPRESSION,
                                 Z_DEFLATED,
                                 MAX_WBITS + 16,
                                 8,
                                 Z_DEFAULT_STRATEGY) != Z_OK)
                {
                    // todo: add an error handling way here
                    return string_type{};
                }
                string_type outstr;
                outstr.resize(compressBound(static_cast<uLong>(ndata)));
                strm.next_in  = reinterpret_cast<Bytef z_const *>(data);
                strm.avail_in = static_cast<uInt>(ndata);
                for (;;) {
                    if (strm.total_out >= outstr.size()) {
                        outstr.resize(strm.total_out * 2);
                    }
                    assert(outstr.size() >= strm.total_out);
                    strm.avail_out = static_cast<uInt>(outstr.size() - strm.total_out);
                    strm.next_out  = reinterpret_cast<Bytef *>(outstr.data()) + strm.total_out;
                    int const ret  = deflate(&strm, Z_FINISH); // no bad return value
                    if (ret == Z_STREAM_ERROR) {
                        static_cast<void>(deflateEnd(&strm));
                        return string_type{};
                    }
                    if (strm.avail_out != 0) {
                        break;
                    }
                }
                assert(strm.avail_in == 0);
                // assert(ret == Z_STREAM_END);          // stream will be complete
                outstr.resize(strm.total_out);

                static_cast<void>(deflateEnd(&strm)); // clean up and return
                return outstr;
            }
            return string_type{};
        }

        /**
         * Decompress gzip data
         * Original source from: drogon project
         */
        template <istl::String StrT = stl::string>
        static StrT decompress(
#ifdef ZLIB_CONST
          typename StrT::const_pointer
#else
          typename StrT::pointer
#endif
                            data,
          stl::size_t const ndata) {
            using string_type = StrT;

            if (ndata == 0) {
                return string_type(data, ndata);
            }

            auto const full_length = ndata;

            auto decompressed = string_type(full_length * 2, 0);
            bool done         = false;

            z_stream strm =
              {nullptr, 0, 0, nullptr, 0, 0, nullptr, nullptr, nullptr, nullptr, nullptr, 0, 0, 0};
            strm.next_in   = reinterpret_cast<Bytef z_const *>(data);
            strm.avail_in  = static_cast<uInt>(ndata);
            strm.total_out = 0;
            strm.zalloc    = nullptr;
            strm.zfree     = nullptr;
            if (inflateInit2(&strm, (15 + 32)) != Z_OK) {
                // todo: add an error handling way here
                return string_type{};
            }
            while (!done) {
                // Make sure we have enough space and reset the lengths
                if (strm.total_out >= decompressed.length()) {
                    decompressed.resize(decompressed.length() * 2);
                }
                strm.next_out  = reinterpret_cast<Bytef *>(decompressed.data()) + strm.total_out;
                strm.avail_out = static_cast<uInt>(decompressed.length() - strm.total_out);
                // Inflate another chunk
                if (int const status = inflate(&strm, Z_SYNC_FLUSH); status == Z_STREAM_END) {
                    done = true;
                } else if (status != Z_OK) {
                    break;
                }
            }
            if (inflateEnd(&strm) != Z_OK) {
                return string_type{};
            }
            // Set the real length
            if (done) {
                decompressed.resize(strm.total_out);
                return decompressed;
            }
            return string_type{};
        }
    };

} // namespace webpp

#endif // WEBPP_GZIP_HPP
