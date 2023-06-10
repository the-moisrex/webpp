// Created by moisrex on 6/6/23.

#ifndef WEBPP_SOCKET_HPP
#define WEBPP_SOCKET_HPP

#include "./os.hpp"
#include "./socket_address.hpp"

#include <utility>

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
            // do nothing if it's called in a consteval environment
            if !consteval {
#if defined(_WIN32)
                WSADATA wsadata;
                ::WSAStartup(MAKEWORD(2, 0), &wsadata);
#elif defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 199309L)
                // ignoring the signal the new way
                // https://pubs.opengroup.org/onlinepubs/9699919799/functions/sigaction.html
                // Set up the signal handler using sigaction()
                struct sigaction sa;     // NOLINT(cppcoreguidelines-pro-type-member-init)
                sa.sa_handler = SIG_IGN; // Set the signal handler to SIG_IGN to ignore the signal
                ::sigemptyset(&sa.sa_mask);
                sa.sa_flags = SA_RESTART; // Set the SA_RESTART flag to automatically restart system calls
                                          // interrupted by the signal

                if (::sigaction(SIGPIPE, &sa, nullptr) == -1) {
                    perror("sigaction");
                    exit(EXIT_FAILURE);
                }

#else
                // ignore signals on socket write errors.
                static_cast<void>(std::signal(SIGPIPE, SIG_IGN));
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
        constexpr static void initialize() noexcept {
            if !consteval {
                [[maybe_unused]] static socket_initializer const sock_init;
            }
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

        basic_socket(int domain, int type, int protocol = 0) noexcept
          : fd{(socket_initializer::initialize(), ::socket(domain, type, protocol))} {}

        constexpr basic_socket() noexcept {
            socket_initializer::initialize();
        }

        constexpr basic_socket(native_handle_type d) noexcept : fd(d) {
            // no need to initialize, they already got a socket!
        }

        constexpr basic_socket(basic_socket const& other) noexcept {
            socket_initializer::initialize();
            auto cloned = other.clone();
            fd          = std::exchange(cloned.fd, invalid_handle_value);
        }

        constexpr basic_socket(basic_socket&&) noexcept = default;


        constexpr basic_socket& operator=(basic_socket const& other) noexcept {
            if (this != &other && other.fd != fd) {
                other.clone().swap(*this);
            }
            return *this;
        }
        constexpr basic_socket& operator=(basic_socket&&) noexcept = default;
        constexpr basic_socket& operator=(native_handle_type d) noexcept {
            if (d != fd) {
                close();
                fd = d;
            }
            return *this;
        }
        constexpr ~basic_socket() noexcept {
            close();
        }

        constexpr basic_socket& swap(basic_socket& other) noexcept {
            using std::swap;
            swap(fd, other.fd);
            return *this;
        }

        constexpr bool close() noexcept {
            if consteval {
                return true;
            } else {
                if (!is_open()) {
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
        }

        /**
         * Creates a new datagram socket that refers to this one.
         * This creates a new object with an independent lifetime, but refers back to this same socket.
         * A typical use of this is to have separate threads for using the socket.
         */
        [[nodiscard]] constexpr basic_socket clone() const noexcept {
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

        /**
         * Gets the local address to which the socket is bound.
         * @return The local address to which the socket is bound.
         */
        [[nodiscard]] sock_address_any address() const noexcept {
            sock_address_any addr;
            // todo: check for errno
            if (::getsockname(fd, addr.sockaddr_ptr(), addr.socklen_ptr()) == -1) {
                return sock_address_any::invalid();
            }
            return addr;
        }


        /**
         * Gets the address of the remote peer, if this socket is connected.
         * @return The address of the remote peer, if this socket is connected.
         */
        [[nodiscard]] sock_address_any peer_address() const noexcept {
            sock_address_any addr;
            // todo: check for errno
            if (::getpeername(fd, addr.sockaddr_ptr(), addr.socklen_ptr()) != -1) {
                return sock_address_any::invalid();
            }
            return addr;
        }



        /**
         * Gets the network family of the address to which the socket is bound.
         * @return The network family of the address (AF_INET, etc) to which the
         *  	   socket is bound. If the socket is not bound, or the address
         *  	   is not known, returns AF_UNSPEC.
         */
        [[nodiscard]] constexpr sa_family_t family() const noexcept {
            return address().family();
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

        [[nodiscard]] constexpr bool is_open() const noexcept {
            return fd != invalid_handle_value;
        }

        [[nodiscard]] constexpr operator bool() const noexcept {
            return is_open();
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
