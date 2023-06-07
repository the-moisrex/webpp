// Created by moisrex on 6/6/23.

#ifndef WEBPP_SOCKET_HPP
#define WEBPP_SOCKET_HPP

#include <cstdint>

#if defined(_WIN32)
#    include <winsock2.h>
// #include <ws2tcpip.h>

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
#    include <arpa/inet.h>
#    include <sys/socket.h>
#    include <unistd.h> // dup
#    ifdef __FreeBSD__
#        include <netinet/in.h>
#    endif
#    include <cerrno>  // io result
#    include <csignal> // for ignoring the signals
#    include <netdb.h>
#endif



namespace webpp {


    /**
     * Result of socket I/O syscalls.
     *   - Returned result
     *   - Read/Wrote counts
     *
     * Most I/O operations in the OS will return >=0 on sccess and -1 on error.
     * In the case of an error, the calling thread must read an `errno` variable
     * immediately, before any other system calls, to get the cause of an error
     * as an integer value defined by the constants ENOENT, EINTR, EBUSY, etc.
     */
    class io_result {
        // Byte count, or 0 on error or EOF
        std::size_t cnt_ = 0;

        // errno value (0 if no error or EOF)
        int err_ = 0;

      public:
        /**
         * OS-specific means to retrieve the last error from an operation.
         * This should be called after a failed system call to get the caue of
         * the error.
         */
        constexpr static int get_last_error() noexcept {
            if !consteval {
#if defined(_WIN32)
                return ::WSAGetLastError();
#else
                return errno;
#endif
            } else {
                return 0;
            }
        }

        // Creates an empty result
        constexpr io_result() = default;


        /**
         * Creates a result from the return value of a low-level I/O function.
         * @param n The number of bytes read or written. If <0, then an error is
         *  		assumed and obtained from socket::get_last_error().
         */
        constexpr explicit io_result(ssize_t n) noexcept {
            if (n < 0) {
                err_ = get_last_error();
            } else {
                cnt_ = std::size_t(n);
            }
        }

        // Set the error value
        constexpr void set_error(int e) noexcept {
            err_ = e;
        }

        // Increments the count
        constexpr void incr(std::size_t n) noexcept {
            cnt_ += n;
        }

        // Determines if the result is OK (not an error)
        [[nodiscard]] constexpr bool is_ok() const noexcept {
            return err_ == 0;
        }

        // Check if it's an error
        [[nodiscard]] constexpr bool is_error() const noexcept {
            return err_ != 0;
        }

        // is_ok
        [[nodiscard]] constexpr operator bool() const noexcept {
            return is_ok();
        }

        [[nodiscard]] constexpr std::size_t count() const noexcept {
            return cnt_;
        }

        [[nodiscard]] constexpr int error() const noexcept {
            return err_;
        }
    };


    /**
     * RAII and singleton class to initialize and then shut down the socket.
     *
     * This is only required on Windows, but is harmless on other platforms.
     * On POSIX, the initializer sets optional parameters for the library, and the destructor does nothing.
     */
    class socket_initializer {

        constexpr socket_initializer() noexcept {
            if !consteval {
#if defined(_WIN32)
                WSADATA wsadata;
                ::WSAStartup(MAKEWORD(2, 0), &wsadata);
#else
                // ignore signals on socket write errors.
                std::signal(SIGPIPE, SIG_IGN);
#endif
            }
        }

      public:
        constexpr socket_initializer(const socket_initializer&)            = delete;
        constexpr socket_initializer(socket_initializer&&)                 = delete;
        constexpr socket_initializer& operator=(const socket_initializer&) = delete;
        constexpr socket_initializer& operator=(socket_initializer&&)      = delete;


        /**
         * Creates the initializer singleton on the first call as a static.
         * It'll get destructed on program termination with the other static objects in
         * reverse order as they were created
         */
        static void initialize() noexcept {
            [[maybe_unused]] static socket_initializer sock_init;
        }

        constexpr ~socket_initializer() noexcept
#if defined(_WIN32)
        {
            ::WSACleanup();
        }
#else
          = default;
#endif
    };


    /**
     * basic_socket is just a wrapper around either a file descriptor or
     * a SOCKET based on the platform.
     *
     * All of the classes can be used at compile-time as well, not that it's useful though!
     */
    struct basic_socket {
#ifdef _WIN32
        using native_handle_type                                 = SOCKET;
        static constexpr native_handle_type invalid_handle_value = INVALID_SOCKET;
#else
        using native_handle_type = int;
        static constexpr native_handle_type invalid_handle_value = -1;
#endif

        basic_socket(int domain, int type, int protocol = 0) noexcept {
            socket_initializer::initialize();
            fd = ::socket(domain, type, protocol);
        }
        constexpr basic_socket() noexcept = default;
        constexpr basic_socket(native_handle_type d) : fd(d) {}

        bool close() noexcept {
            if (!is_valid()) {
                return true;
            }
#if defined(_WIN32)
            bool const val = ::closesocket(fd) >= 0;
#else
            bool const val = ::close(fd) >= 0;
#endif
            if (val) {
                fd = invalid_handle_value;
            }
            return val;
        }

        /**
         * Creates a new datagram socket that refers to this one.
         * This creates a new object with an independent lifetime, but refers back to this same socket.
         * A typical use of this is to have separate threads for using the socket.
         */
        [[nodiscard]] constexpr basic_socket clone() const {
            if consteval {
                return {invalid_handle_value};
            } else {
                native_handle_type h = invalid_handle_value;
#if defined(_WIN32)
                WSAPROTOCOL_INFOW protInfo;
                if (::WSADuplicateSocketW(handle_, ::GetCurrentProcessId(), &protInfo) == 0) {
                    h =
                      check_socket(::WSASocketW(AF_INET, SOCK_STREAM, 0, &protInfo, 0, WSA_FLAG_OVERLAPPED));
                }
#else
                h = ::dup(fd);
#endif
                return {h};
            }
        }


        [[nodiscard]] constexpr native_handle_type native_handle() const noexcept {
            return fd;
        }

        [[nodiscard]] constexpr bool operator==(const basic_socket& other) const noexcept {
            return fd == other.fd;
        }

        [[nodiscard]] constexpr bool operator!=(const basic_socket& other) const noexcept {
            return fd != other.fd;
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return fd != invalid_handle_value;
        }

        [[nodiscard]] constexpr operator bool() const noexcept {
            return is_valid();
        }

      private:
        native_handle_type fd = invalid_handle_value;
    };

} // namespace webpp

#include <functional>
namespace std {

    template <>
    struct hash<webpp::basic_socket> {
        std::size_t operator()(const webpp::basic_socket& s) const noexcept {
            return std::hash<webpp::basic_socket::native_handle_type>()(s.native_handle());
        }
    };

} // namespace std

#endif // WEBPP_SOCKET_HPP
