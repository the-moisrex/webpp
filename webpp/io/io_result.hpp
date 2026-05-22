// Created by moisrex on 9/16/23.

#ifndef WEBPP_IO_RESULT_HPP
#define WEBPP_IO_RESULT_HPP

#include "../std/string.hpp"

#include <cerrno>
#include <cstring> // strerror
#include <system_error>

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
    struct [[nodiscard]] io_result {
        struct raw_error_tag {};

        // Creates an empty result
        constexpr io_result() noexcept = default;

        /**
         * Creates a result from the return val of a low-level I/O function.
         *  - If n == -1, then an error is obtained,
         *  - If n <  -1, then it considers n as the invalid val
         * @param n The number of bytes read or written.
         */
        // NOLINTNEXTLINE(*-explicit-*)
        explicit(false) constexpr io_result(int const n) noexcept : val{n == -1 ? -errno : n} {}

        explicit(false) constexpr io_result(int const n, raw_error_tag) noexcept : val{n} {}

        static io_result invalid(int const inp_val = errno) noexcept {
            return io_result{inp_val > 0 ? -inp_val : inp_val, raw_error_tag{}};
        }

        void set_error(int const inp_val = errno) noexcept {
            val = -inp_val;
        }

        // Determines if the result is OK (not an error)
        [[nodiscard]] constexpr bool is_ok() const noexcept {
            return val >= 0;
        }

        // Check if it's an error
        [[nodiscard]] constexpr bool is_error() const noexcept {
            return val < 0;
        }

        [[nodiscard]] explicit constexpr operator bool() const noexcept {
            return is_ok();
        }

        [[nodiscard]] constexpr int value() const noexcept {
            return val;
        }

        [[nodiscard]] explicit constexpr operator int() const noexcept {
            return val;
        }

        [[nodiscard]] constexpr int error() const noexcept {
            return -val;
        }

        // Returns an stl::error_code representation of the error
        [[nodiscard]] stl::error_code error_code(
          stl::error_category const& cat = stl::generic_category()) const noexcept {
            if (is_ok()) {
                return {};
            }
            // Since we store POSIX errno values, generic_category is the correct category
            return {error(), cat};
        }

        // Implicit conversion to stl::error_code for seamless compatibility
        [[nodiscard]] operator stl::error_code() const noexcept { // NOLINT(*-explicit-*)
            return error_code();
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
            if (!is_error()) {
                return;
            }
            char buf[256]; // NOLINT
            buf[0] = '\0';

#ifdef _WIN32
            strerror_s(buf, sizeof(buf), this->error());
            out += buf;
#elif defined(_GNU_SOURCE) && defined(__GLIBC__)
            // GNU-specific strerror_r returns a char* which might not be `buf`
            out += strerror_r(this->error(), buf, sizeof(buf));
#elif (_POSIX_C_SOURCE >= 200'112L) || defined(__APPLE__) || defined(__FreeBSD__)
            // XSI-compliant POSIX strerror_r returns an int
            strerror_r(this->error(), buf, sizeof(buf));
            out += buf;
#else
            // Fallback (not thread-safe)
            out += stl::strerror(this->error());
#endif
        }

      private:
        int val = 0;
    };

} // namespace webpp::io

#endif // WEBPP_IO_RESULT_HPP
