// Created by moisrex on 6/6/23.

#ifndef WEBPP_SOCKET_HPP
#define WEBPP_SOCKET_HPP

#include "./os.hpp"
#include "./socket_address.hpp"

#include <cerrno>
#include <utility>

namespace webpp {


    /**
     * RAII and singleton class to initialize and then shut down the socket.
     *
     * This is only required on Windows, but is harmless on other platforms.
     * On POSIX, the initializer sets optional parameters for the library, and the destructor does nothing.
     */
    class socket_initializer {
        socket_initializer() noexcept {
#ifdef MSVC_COMPILER
            WSADATA wsadata;
            if (inet iResult = ::WSAStartup(MAKEWORD(2, 0), &wsadata); iResult != NO_ERROR) {
                wprintf(L"WSAStartup() failed with error: %d\n", iResult);
                exit(EXIT_FAILURE);
            }
#elif defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 199'309L)
            // ignoring the signal the new way
            // https://pubs.opengroup.org/onlinepubs/9699919799/functions/sigaction.html
            // Set up the signal handler using sigaction()
            struct sigaction sa;      // NOLINT(cppcoreguidelines-pro-type-member-init)
            sa.sa_handler = SIG_IGN;  // Set the signal handler to SIG_IGN to ignore the signal
            ::sigemptyset(&sa.sa_mask);
            sa.sa_flags = SA_RESTART; // Set the SA_RESTART flag to automatically restart system calls
                                      // interrupted by the signal

            if (::sigaction(SIGPIPE, &sa, nullptr) == -1) {
                perror("sigaction");
                exit(EXIT_FAILURE);
            }

#else
            // ignore signals on socket write errors.
            static_cast<void>(stl::signal(SIGPIPE, SIG_IGN));
#endif
        }

      public:
        socket_initializer(socket_initializer const&)            = delete;
        socket_initializer(socket_initializer&&)                 = delete;
        socket_initializer& operator=(socket_initializer const&) = delete;
        socket_initializer& operator=(socket_initializer&&)      = delete;

        /**
         * Creates the initializer singleton on the first call as a static.
         * It'll get destructed on program termination with the other static objects in
         * reverse order as they were created
         */
        static void initialize() noexcept {
            [[maybe_unused]] static socket_initializer const sock_init;
        }

#ifdef MSVC_COMPILER
        ~socket_initializer() noexcept {
            ::WSACleanup();
        }
#else
        ~socket_initializer() noexcept                           = default;
#endif
    };

    using ipproto_type = int;

    /**
     * Standard well-defined IP protocols
     *
     * The ones that start with WIN32 are from here:
     *   https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-socket
     * The rest are from <netinet/in.h>
     *
     * This is a struct >> enum and not "enum struct" because ipproto can have other values as well,
     * and we don't want you to keep casting around.
     */
    struct ipproto {
        enum : ipproto_type {
            ip       = 0,  // Dummy protocol for TCP.
            icmp     = 1,  // Internet Control Message Protocol.
            igmp     = 2,  // Internet Group Management Protocol.
            rfcomm   = 3,  // WIN32 - The Bluetooth Radio Frequency Communications (Bluetooth RFCOMM) protocol
            ipip     = 4,  // IPIP tunnels (older KA9Q tunnels use 94).
            tcp      = 6,  // Transmission Control Protocol.
            egp      = 8,  // Exterior Gateway Protocol.
            pup      = 12, // PUP protocol.
            udp      = 17, // User Datagram Protocol.
            idp      = 22, // XNS IDP protocol.
            tp       = 29, // SO Transport Protocol Class 4.
            dccp     = 33, // Datagram Congestion Control Protocol.
            ipv6     = 41, // IPv6 header.
            rsvp     = 46, // Reservation Protocol.
            gre      = 47, // General Routing Encapsulation.
            esp      = 50, // encapsulating security payload.
            ah       = 51, // authentication header.
            icmpv6   = 58, // WIN32 - The Internet Control Message Protocol Version 6 (ICMPv6).
            mtp      = 92, // Multicast Transport Protocol.
            beetph   = 94, // IP option pseudo header for BEET.
            encap    = 98, // Encapsulation Header.
            pim      = 103, // Protocol Independent Multicast.
            comp     = 108, // Compression Header Protocol.
            rm       = 113, // WIN32 - The PGM protocol for reliable multicast.
            l2tp     = 115, // Layer 2 Tunnelling Protocol.
            sctp     = 132, // Stream Control Transmission Protocol.
            udplite  = 136, // UDP-Lite protocol.
            mpls     = 137, // MPLS in IP.
            ethernet = 143, // Ethernet-within-IPv6 Encapsulation.
            raw      = 255, // Raw IP packets.
            mptcp    = 262, // Multipath TCP connection.
            max             // just to indicate the max value
        };
    };

    /**
     * Basic Socket is just a wrapper around either a file descriptor or a SOCKET based on the platform.
     */
    struct basic_socket {
        // Default listener Queue Size (backlog)
        // The value of the file "/proc/sys/net/core/somaxconn" (or sysctl one) relates to this value.
        static constexpr int default_queue_size = SOMAXCONN;

#ifdef MSVC_COMPILER
        using native_handle_type                                 = SOCKET;
        static constexpr native_handle_type invalid_handle_value = INVALID_SOCKET;
#else
        using native_handle_type                                 = int;
        static constexpr native_handle_type invalid_handle_value = -1;
#endif

        // get an invalid socket
        static basic_socket invalid() noexcept {
            return {};
        }

      private:
        /**
         * Checks the value and if less than zero, sets last error.
         */
        [[nodiscard]] bool check_ret_bool(stl::integral auto ret) const noexcept {
            // doesn't really matter if we use SOCKET_ERROR or not!
#ifdef MSVC_COMPILER
            last_errno = ret == SOCKET_ERROR ? errno : 0;
#else
            last_errno     = ret == -1 ? errno : 0;
#endif
            return ret >= 0;
        }

      public:
        basic_socket(int const domain, int const type, ipproto_type const protocol = ipproto::ip) noexcept
          : fd{(socket_initializer::initialize(), ::socket(domain, type, protocol))} {}

        basic_socket(ip_address const   addr,
                     int const          type,
                     ipproto_type const protocol = ipproto::ip) noexcept
          : fd{(socket_initializer::initialize(),
                addr.is_valid() ? ::socket(addr.is_v4() ? AF_INET : AF_INET6, type, protocol)
                                : invalid_handle_value)} {}

        basic_socket(ip_address const   addr,
                     int const          type,
                     ipproto_type const protocol,
                     in_port_t const    port) noexcept
          : basic_socket{addr, type, protocol} {
            if (is_valid()) {
                this->bind(addr, port);
            }
        }

        basic_socket() noexcept {
            socket_initializer::initialize();
        }

        explicit basic_socket(native_handle_type const inp_desc) noexcept : fd(inp_desc) {
            // no need to initialize, they already got a socket!
        }

        basic_socket(basic_socket const& other) noexcept
          : fd{(socket_initializer::initialize(), other.clone().release())} {}

        basic_socket(basic_socket&& other) noexcept : fd{other.release()} {}

        basic_socket& operator=(basic_socket const& other) noexcept {
            if (this != &other && other.fd != fd) {
                other.clone().swap(*this);
            }
            return *this;
        }

        basic_socket& operator=(basic_socket&&) noexcept = default;

        basic_socket& operator=(native_handle_type const inp_desc) noexcept {
            if (inp_desc != fd) {
                close();
                fd = inp_desc;
                clear_error();
            }
            return *this;
        }

        ~basic_socket() noexcept {
            close();
        }

        basic_socket& swap(basic_socket& other) noexcept {
            using stl::swap;
            swap(fd, other.fd);
            swap(last_errno, other.last_errno);
            return *this;
        }

        /**
         * Releases the ownership of the socket handle.
         * This member function puts this class into the "invalid" state.
         */
        native_handle_type release() noexcept {
            return stl::exchange(fd, invalid_handle_value);
        }

        /**
         * close the socket if it's open;
         *  - on error,   the last_error will be filled
         *  - on success, the socket's state is the same as `basic_socket::invalid()`
         */
        bool close() noexcept {
            if (!is_open()) {
                return true;
            }
#ifdef MSVC_COMPILER
            bool const val = check_ret_bool(::closesocket(fd));
#else
            bool const val = check_ret_bool(::close(fd));
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
        [[nodiscard]] basic_socket clone() const noexcept {
            native_handle_type cur_handle = invalid_handle_value;
#ifdef MSVC_COMPILER
            WSAPROTOCOL_INFOW protInfo;
            if (::WSADuplicateSocketW(handle_, ::GetCurrentProcessId(), &protInfo) == 0) {
                h = check_socket(::WSASocketW(AF_INET, SOCK_STREAM, 0, &protInfo, 0, WSA_FLAG_OVERLAPPED));
            }
#else
            cur_handle     = ::dup(fd);
#endif
            return basic_socket{cur_handle};
        }

        /**
         * Gets the local address to which the socket is bound.
         * @return The local address to which the socket is bound.
         */
        [[nodiscard]] sock_address_any address() const noexcept {
            sock_address_any addr;
            if (check_ret_bool(::getsockname(fd, addr.sockaddr_ptr(), addr.socklen_ptr()))) {
                return addr;
            }
            return sock_address_any::invalid();
        }

        /**
         * Gets the address of the remote peer, if this socket is connected.
         * @return The address of the remote peer, if this socket is connected.
         */
        [[nodiscard]] sock_address_any peer_address() const noexcept {
            sock_address_any addr;
            if (check_ret_bool(::getpeername(fd, addr.sockaddr_ptr(), addr.socklen_ptr()))) {
                return addr;
            }
            return sock_address_any::invalid();
        }

        /**
         * Binds the socket to the specified address.
         */
        bool bind(sock_address_any const& addr) noexcept {
            return check_ret_bool(::bind(fd, addr.sockaddr_ptr(), addr.size()));
        }

        // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
        bool bind(ipv4 ip, stl::uint16_t const port) noexcept {
            auto addr     = make_sock_addr<sockaddr_in>(ip);
            addr.sin_port = static_cast<in_port_t>(hton(port));
            return check_ret_bool(::bind(fd, reinterpret_cast<sockaddr const*>(&addr), sizeof(sockaddr_in)));
        }

        bool bind(ipv6 const& ip, stl::uint16_t const port) noexcept {
            auto addr      = make_sock_addr<sockaddr_in6>(ip);
            addr.sin6_port = static_cast<in_port_t>(hton(port));
            return check_ret_bool(::bind(fd, reinterpret_cast<sockaddr const*>(&addr), sizeof(sockaddr_in6)));
        }

        bool bind(struct ip_address const& ip, stl::uint16_t port) noexcept {
            if (ip.is_valid()) {
                return ip.pick([this, port](auto inp_ip) noexcept {
                    return this->bind(inp_ip, port);
                });
            }
            return false;
        }

        // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)

        /**
         * Start listening
         * @param queue_size The listener queue size
         */
        bool listen(int const queue_size = default_queue_size) noexcept {
            return check_ret_bool(::listen(fd, queue_size));
        }

        /**
         * Bind and Listen
         * @param ip
         * @param port
         * @param queue_size
         * @return true if successful
         */
        bool listen(ipv4 const          ip,
                    stl::uint16_t const port,
                    int const           queue_size = default_queue_size) noexcept {
            return bind(ip, port) && listen(queue_size);
        }

        /**
         * Bind and Listen
         * @param ip
         * @param port
         * @param queue_size
         * @return true if successful
         */
        bool listen(ipv6 const&         ip,
                    stl::uint16_t const port,
                    int const           queue_size = default_queue_size) noexcept {
            return bind(ip, port) && listen(queue_size);
        }

        /**
         * Bind and Listen
         * @param ip
         * @param port
         * @param queue_size
         * @return true if successful
         */
        bool listen(struct ip_address const& ip,
                    stl::uint16_t const      port,
                    int const                queue_size = default_queue_size) noexcept {
            return bind(ip, port) && listen(queue_size);
        }

        /**
         * Gets the value of a socket option.
         * This is a thin wrapper for the system `getsockopt`.
         *
         * @param level The protocol level at which the option resides, such as SOL_SOCKET.
         * @param optname The option passed directly to the protocol module.
         * @param optval The buffer for the value to retrieve
         * @param optlen length of the buffer as input, and the length of the value retrieved on return.
         *
         * @return bool true if the value was retrieved, false on error.
         */
        bool get_option(int level, int optname, void* optval, socklen_t* optlen) const noexcept {
#ifdef MSVC_COMPILER
            if (optval && optlen) {
                int len = static_cast<int>(*optlen);
                if (check_ret_bool(::getsockopt(fd, level, optname, static_cast<char*>(optval), &len))) {
                    *optlen = static_cast<socklen_t>(len);
                    return true;
                }
            }
            return false;
#else
            return check_ret_bool(::getsockopt(fd, level, optname, optval, optlen));
#endif
        }

        /**
         * Gets the value of a socket option.
         *
         * @param level The protocol level at which the option resides, such as SOL_SOCKET.
         * @param optname The option passed directly to the protocol module.
         * @param val The value to retrieve
         *
         * @return bool true if the value was retrieved, false on error.
         */
        template <typename T>
        bool get_option(int const level, int const optname, T* val) const noexcept {
            socklen_t len = sizeof(T);
            return get_option(level, optname, (void*) val, &len);
        }

        /**
         * Sets the value of a socket option.
         * This is a thin wrapper for the system `setsockopt`.
         *
         * @param level The protocol level at which the option resides, such as SOL_SOCKET.
         * @param optname The option passed directly to the protocol module.
         * @param optval The buffer with the value to set.
         * @param optlen Contains the length of the buffer.
         *
         * @return bool true if the value was set, false on error.
         */
        bool set_option(int level, int optname, void const* optval, socklen_t optlen) noexcept {
#ifdef MSVC_COMPILER
            return check_ret_bool(
              ::setsockopt(fd, level, optname, static_cast<char const*>(optval), static_cast<int>(optlen)));
#else
            return check_ret_bool(::setsockopt(fd, level, optname, optval, optlen));
#endif
        }

        /**
         * Sets the value of a socket option.
         *
         * @param level The protocol level at which the option resides, such as SOL_SOCKET.
         * @param optname The option passed directly to the protocol module.
         * @param val The value
         *
         * @return bool true if the value was set, false on error
         */
        template <typename T>
        bool set_option(int const level, int const optname, T const& val) noexcept {
            return set_option(level, optname, (void*) &val, sizeof(T));
        }

        bool set_non_blocking(bool on = true) noexcept {
#ifdef MSVC_COMPILER
            unsigned long mode = on ? 1 : 0;
            return check_ret_bool(::ioctlsocket(fd, FIONBIO, &mode));
#else
            return set_flag(O_NONBLOCK, on);
#endif
        }
#ifndef MSVC_COMPILER

        int get_flags() const noexcept {
            int const flags = ::fcntl(fd, F_GETFL, 0); // NOLINT(cppcoreguidelines-pro-type-vararg)
            last_errno      = (flags == -1) ? errno : 0;
            return flags;
        }

        bool set_flags(int const flags) noexcept {
            if (::fcntl(fd, F_SETFL, flags) == -1) { // NOLINT(cppcoreguidelines-pro-type-vararg)
                last_errno = errno;
                return false;
            }
            return true;
        }

        bool set_flag(int const flag, bool const on = true) noexcept {
            int const flags = get_flags();
            if (flags == -1) {
                return false;
            }
            return set_flags(on ? (flags | flag) : (flags & ~flag));
        }

        [[nodiscard]] bool is_non_blocking() const noexcept {
            int const flags = get_flags();
            return flags != -1 && ((flags & O_NONBLOCK) != 0);
        }

#endif

        /**
         * Shuts down all or part of the full-duplex connection.
         * @param how Which part of the connection should be shut:
         *  	@li SHUT_RD   (0) Further reads disallowed.
         *  	@li SHUT_WR   (1) Further writes disallowed
         *  	@li SHUT_RDWR (2) Further reads and writes disallowed.
         * @return true on success, false on error.
         */
        bool shutdown(int const how = SHUT_RDWR) noexcept {
            if (is_open()) {
                return check_ret_bool(::shutdown(release(), how));
            }
            return false;
        }

        /**
         * Replaces the underlying managed socket object.
         * @param h The new socket handle to manage.
         */
        void reset(native_handle_type d = invalid_handle_value) {
            if (d != fd) {
                close();
                fd = d;
            }
            clear_error();
        }

        /**
         * Gets the network family of the address to which the socket is bound.
         *
         * @return The network family of the address (AF_INET, etc) to which the socket is bound.
         * If the socket is not bound, or the address is not known, returns AF_UNSPEC.
         */
        [[nodiscard]] sa_family_t family() const noexcept {
            return address().family();
        }

        [[nodiscard]] native_handle_type native_handle() const noexcept {
            return fd;
        }

        [[nodiscard]] bool operator==(basic_socket const& other) const noexcept {
            return fd == other.fd;
        }

        [[nodiscard]] bool operator!=(basic_socket const& other) const noexcept {
            return fd != other.fd;
        }

        [[nodiscard]] bool is_open() const noexcept {
            return fd != invalid_handle_value;
        }

        [[nodiscard]] bool is_valid() const noexcept {
            return is_open() && is_error_free();
        }

        [[nodiscard]] bool is_error_free() const noexcept {
            return last_errno == 0;
        }

        [[nodiscard]] explicit operator bool() const noexcept {
            return is_valid();
        }

        [[nodiscard]] int last_error() const noexcept {
            return last_errno;
        }

        /**
         * Clears the error flag for the object.
         * @param val The value to set the flag, normally zero.
         */
        void clear_error(int const val = 0) noexcept {
            last_errno = val;
        }

      private:
        native_handle_type fd         = invalid_handle_value;
        mutable int        last_errno = 0;
    };

} // namespace webpp

#include <functional>

template <>
struct std::hash<webpp::basic_socket> {
    size_t operator()(webpp::basic_socket const& s) const noexcept {
        return hash<webpp::basic_socket::native_handle_type>()(s.native_handle());
    }
};

#endif // WEBPP_SOCKET_HPP
