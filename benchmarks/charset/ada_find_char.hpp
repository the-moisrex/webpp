// Created by moisrex on 10/29/23.

#ifndef WEBPP_ADA_FIND_CHAR_HPP
#define WEBPP_ADA_FIND_CHAR_HPP

#include <cstdint>
#include <cstring>
#include <string_view>

/// source: https://github.com/ada-url/ada/blob/a9dd0d427e965ade856306c0e4b0afca04292db1/src/helpers.cpp#L268
namespace ada_v1 {

#if defined(_MSC_VER)
#    define ada_really_inline __forceinline
#else // ADA_REGULAR_VISUAL_STUDIO
#    define ada_really_inline inline __attribute__((always_inline))
#endif




#if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__)
#    define ADA_IS_BIG_ENDIAN (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#elif defined(_WIN32)
#    define ADA_IS_BIG_ENDIAN 0
#else
#    if defined(__APPLE__) || defined(__FreeBSD__) // defined __BYTE_ORDER__ && defined
                                                   // __ORDER_BIG_ENDIAN__
#        include <machine/endian.h>
#    elif defined(sun) || defined(__sun)           // defined(__APPLE__) || defined(__FreeBSD__)
#        include <sys/byteorder.h>
#    else                                          // defined(__APPLE__) || defined(__FreeBSD__)

#        ifdef __has_include
#            if __has_include(<endian.h>)
#                include <endian.h>
#            endif //__has_include(<endian.h>)
#        endif     //__has_include

#    endif         // defined(__APPLE__) || defined(__FreeBSD__)

#    ifndef !defined(__BYTE_ORDER__) || !defined(__ORDER_LITTLE_ENDIAN__)
#        define ADA_IS_BIG_ENDIAN 0
#    endif

#    if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#        define ADA_IS_BIG_ENDIAN 0
#    else  // __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#        define ADA_IS_BIG_ENDIAN 1
#    endif // __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

#endif     // defined __BYTE_ORDER__ && defined __ORDER_BIG_ENDIAN__



    // Reverse the byte order.
    ada_really_inline uint64_t swap_bytes(uint64_t val) noexcept {
        // performance: this often compiles to a single instruction (e.g., bswap)
        return ((((val) & 0xff00'0000'0000'0000ull) >> 56) | (((val) & 0x00ff'0000'0000'0000ull) >> 40) |
                (((val) & 0x0000'ff00'0000'0000ull) >> 24) | (((val) & 0x0000'00ff'0000'0000ull) >> 8) |
                (((val) & 0x0000'0000'ff00'0000ull) << 8) | (((val) & 0x0000'0000'00ff'0000ull) << 24) |
                (((val) & 0x0000'0000'0000'ff00ull) << 40) | (((val) & 0x0000'0000'0000'00ffull) << 56));
    }

    ada_really_inline uint64_t swap_bytes_if_big_endian(uint64_t val) noexcept {
// performance: under little-endian systems (most systems), this function
// is free (just returns the input).
#if ADA_IS_BIG_ENDIAN
        return swap_bytes(val);
#else
        return val; // unchanged (trivial)
#endif
    }

    // starting at index location, this finds the next location of a character
    // :, /, ? or [. If none is found, view.size() is returned.
    // For use within get_host_delimiter_location.
    ada_really_inline size_t find_next_host_delimiter(std::string_view view, size_t location) noexcept {
        // performance: if you plan to call find_next_host_delimiter more than once,
        // you *really* want find_next_host_delimiter to be inlined, because
        // otherwise, the constants may get reloaded each time (bad).
        auto has_zero_byte = [](uint64_t v) {
            return ((v - 0x0101'0101'0101'0101) & ~(v) & 0x8080'8080'8080'8080);
        };
        auto index_of_first_set_byte = [](uint64_t v) {
            return ((((v - 1) & 0x101'0101'0101'0101) * 0x101'0101'0101'0101) >> 56) - 1;
        };
        auto broadcast = [](uint8_t v) -> uint64_t {
            return 0x101'0101'0101'0101ull * v;
        };
        size_t   i     = location;
        uint64_t mask1 = broadcast(':');
        uint64_t mask2 = broadcast('/');
        uint64_t mask4 = broadcast('?');
        uint64_t mask5 = broadcast('[');
        // This loop will get autovectorized under many optimizing compilers,
        // so you get actually SIMD!
        for (; i + 7 < view.size(); i += 8) {
            uint64_t word{};
            // performance: the next memcpy translates into a single CPU instruction.
            memcpy(&word, view.data() + i, sizeof(word));
            // performance: on little-endian systems (most systems), this next line is
            // free.
            word          = swap_bytes_if_big_endian(word);
            uint64_t xor1 = word ^ mask1;
            uint64_t xor2 = word ^ mask2;
            uint64_t xor4 = word ^ mask4;
            uint64_t xor5 = word ^ mask5;
            uint64_t is_match =
              has_zero_byte(xor1) | has_zero_byte(xor2) | has_zero_byte(xor4) | has_zero_byte(xor5);
            if (is_match) {
                return size_t(i + index_of_first_set_byte(is_match));
            }
        }
        if (i < view.size()) {
            uint64_t word{};
            // performance: the next memcpy translates into a function call, but
            // that is difficult to avoid. Might be a bit expensive.
            memcpy(&word, view.data() + i, view.size() - i);
            // performance: on little-endian systems (most systems), this next line is
            // free.
            word          = swap_bytes_if_big_endian(word);
            uint64_t xor1 = word ^ mask1;
            uint64_t xor2 = word ^ mask2;
            uint64_t xor4 = word ^ mask4;
            uint64_t xor5 = word ^ mask5;
            uint64_t is_match =
              has_zero_byte(xor1) | has_zero_byte(xor2) | has_zero_byte(xor4) | has_zero_byte(xor5);
            if (is_match) {
                return size_t(i + index_of_first_set_byte(is_match));
            }
        }
        return view.size();
    }


} // namespace ada_v1

#endif // WEBPP_ADA_FIND_CHAR_HPP
