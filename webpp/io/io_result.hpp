// Created by moisrex on 9/16/23.

#ifndef WEBPP_IO_RESULT_HPP
#define WEBPP_IO_RESULT_HPP

#include "../std/string.hpp"

#include <cerrno>
#include <cstring> // strerror

namespace webpp::io {

    /**
     * Result of I/O syscalls
     *   - Returned result
     *   - Read/Wrote counts
     *   - ...
     *
     * Most I/O operations in the OS will return >=0 on success and -1 on error.
     * In the case of an error, the calling thread must read an `errno` variable immediately,
     * before any other system calls, to get the cause of an error as an integer val defined
     * by the constants ENOENT, EINTR, EBUSY, etc.
     */
    struct io_result {
        // Creates an empty result
        constexpr io_result() noexcept = default;

        /**
         * Creates a result from the return val of a low-level I/O function.
         *  - If n == -1, then an error is obtained,
         *  - If n <  -1, then it considers n as the invalid val
         * @param n The number of bytes read or written.
         */
        constexpr io_result(int n) noexcept : val{n == -1 ? -errno : n} {}

        static constexpr io_result invalid(int n = errno) noexcept {
            return {-n};
        }

        // Set the error val
        constexpr void error(int n = errno) noexcept {
            val = -n;
        }

        // Determines if the result is OK (not an error)
        [[nodiscard]] constexpr bool is_ok() const noexcept {
            return val >= 0;
        }

        // Check if it's an error
        [[nodiscard]] constexpr bool is_error() const noexcept {
            return val < 0;
        }

        [[nodiscard]] constexpr operator bool() const noexcept {
            return is_ok();
        }

        [[nodiscard]] constexpr int value() const noexcept {
            return val;
        }

        [[nodiscard]] constexpr operator int() const noexcept {
            return val;
        }

        [[nodiscard]] constexpr int error() const noexcept {
            return -val;
        }

        template <typename StrT = stl::string, typename... Args>
            requires(!istl::cvref_as<Args, StrT> && ...)
        [[nodiscard]] constexpr StrT to_string(Args&&... args) const {
            StrT out{stl::forward<Args>(args)...};
            to_string(out);
            return out;
        }

        template <typename StrT>
        constexpr void to_string(StrT& out) const {
            out += stl::strerror(this->error());
        }

      private:
        int val = 0;
    };

} // namespace webpp::io

#endif // WEBPP_IO_RESULT_HPP
