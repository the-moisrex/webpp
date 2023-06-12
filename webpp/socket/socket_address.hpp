// Created by moisrex on 6/8/23.

#ifndef WEBPP_SOCKET_ADDRESS_HPP
#define WEBPP_SOCKET_ADDRESS_HPP

#include "../ip/address.hpp"
#include "./os.hpp"
#include "./socket_bytes.hpp"

namespace webpp {
    //////////////////////////////////////// To ipv4/ipv6/addresses ////////////////////////////////////////

    static constexpr void to_addr(ipv4& to_ip, sockaddr_in from_in) noexcept {
        to_ip = static_cast<stl::uint32_t>(ntoh(from_in.sin_addr.s_addr)); // s_addr is uint32_t
    }

    static constexpr void to_addr(address& to_ip, sockaddr_in from_in) noexcept {
        to_ip = ipv4{static_cast<stl::uint32_t>(ntoh(from_in.sin_addr.s_addr))}; // s_addr is uint32_t
    }

    static constexpr void to_addr(ipv6& to_ip, sockaddr_in6 const& from_in) noexcept {
        to_ip = from_in.sin6_addr.s6_addr;
    }

    static constexpr void to_addr(address& to_ip, sockaddr_in6 const& from_in) noexcept {
        to_ip = ipv6{from_in.sin6_addr.s6_addr};
    }

    static constexpr void to_addr(address& to_ip, in_addr from_in) noexcept {
        to_ip = ipv4{static_cast<stl::uint32_t>(ntoh(from_in.s_addr))};
    }

    static constexpr void to_addr(ipv4& to_ip, in_addr from_in) noexcept {
        to_ip = static_cast<stl::uint32_t>(ntoh(from_in.s_addr));
    }

    static constexpr void to_addr(address& to_ip, in6_addr const& from_in) noexcept {
        to_ip = ipv6{from_in.s6_addr};
    }

    static constexpr void to_addr(ipv6& to_ip, in6_addr const& from_in) noexcept {
        to_ip = ipv6{from_in.s6_addr};
    }

    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)

    // technically sockaddr should not be able to hold ipv6 addresses thus this should be undefined behaviour
    static inline void to_addr(address& to_ip, sockaddr const& from_in) noexcept {
        switch (from_in.sa_family) {
            case AF_INET: to_addr(to_ip, reinterpret_cast<sockaddr_in const&>(from_in)); break;
            case AF_INET6: to_addr(to_ip, reinterpret_cast<sockaddr_in6 const&>(from_in)); break;
            default: to_ip = address::invalid();
        }
    }

    // technically sockaddr should not be able to hold ipv6 addresses thus this should be undefined behaviour
    static inline void to_addr(ipv6& to_ip, sockaddr const& from_in) noexcept {
        switch (from_in.sa_family) {
            case AF_INET6: to_addr(to_ip, reinterpret_cast<sockaddr_in6 const&>(from_in)); break;
            default: to_ip = ipv6::invalid();
        }
    }

    static inline void to_addr(ipv4& to_ip, sockaddr const& from_in) noexcept {
        switch (from_in.sa_family) {
            case AF_INET: to_addr(to_ip, reinterpret_cast<sockaddr_in const&>(from_in)); break;
            default: to_ip = ipv4::invalid();
        }
    }

    static inline void to_addr(address& to_ip, sockaddr_storage const& from_in) noexcept {
        switch (from_in.ss_family) {
            case AF_INET: to_addr(to_ip, reinterpret_cast<sockaddr_in const&>(from_in)); break;
            case AF_INET6: to_addr(to_ip, reinterpret_cast<sockaddr_in6 const&>(from_in)); break;
            default: to_ip = address::invalid();
        }
    }

    static inline void to_addr(ipv4& to_ip, sockaddr_storage const& from_in) noexcept {
        switch (from_in.ss_family) {
            case AF_INET: to_addr(to_ip, reinterpret_cast<sockaddr_in const&>(from_in)); break;
            default: to_ip = ipv4::invalid();
        }
    }

    static inline void to_addr(ipv6& to_ip, sockaddr_storage const& from_in) noexcept {
        switch (from_in.ss_family) {
            case AF_INET6: to_addr(to_ip, reinterpret_cast<sockaddr_in6 const&>(from_in)); break;
            default: to_ip = ipv6::invalid();
        }
    }
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)


    /**
     * Generate a ipv4/ipv6/address from the specified sockaddr/sockaddr_in/...
     */
    template <typename Addr = address, typename SocketIPType>
        requires(
          istl::
            part_of<SocketIPType, sockaddr, sockaddr_in, sockaddr_in6, in_addr, in6_addr, sockaddr_storage> &&
          istl::part_of<Addr, address, ipv4, ipv6>)
    static inline Addr make_addr(SocketIPType const& from_in) noexcept {
        Addr to_ip{};
        to_addr(to_ip, from_in);
        return to_ip;
    }


    //////////////////////////////////////// To Socket Addresses ////////////////////////////////////////

    static constexpr void to_sock_addr(sockaddr_in& to_addr, ipv4 from_ip) noexcept {
        to_addr.sin_family      = AF_INET;
        to_addr.sin_addr.s_addr = hton(from_ip.integer()); // s_addr is uint32_t
    }

    static constexpr void to_sock_addr(in_addr& to_addr, ipv4 from_ip) noexcept {
        to_addr.s_addr = hton(from_ip.integer());
    }

    static constexpr void to_sock_addr(in6_addr& to_addr, ipv6 const& from_ip) noexcept {
        // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
        const auto octets   = from_ip.octets8();
        to_addr.s6_addr[0]  = octets[0];
        to_addr.s6_addr[1]  = octets[1];
        to_addr.s6_addr[2]  = octets[2];
        to_addr.s6_addr[3]  = octets[3];
        to_addr.s6_addr[4]  = octets[4];
        to_addr.s6_addr[5]  = octets[5];
        to_addr.s6_addr[6]  = octets[6];
        to_addr.s6_addr[7]  = octets[7];
        to_addr.s6_addr[8]  = octets[8];
        to_addr.s6_addr[9]  = octets[9];
        to_addr.s6_addr[10] = octets[10];
        to_addr.s6_addr[11] = octets[11];
        to_addr.s6_addr[12] = octets[12];
        to_addr.s6_addr[13] = octets[13];
        to_addr.s6_addr[14] = octets[14];
        to_addr.s6_addr[15] = octets[15];
        // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
    }

    static constexpr void to_sock_addr(sockaddr_in6& to_addr, ipv6 const& from_ip) noexcept {
        to_addr.sin6_family = AF_INET6;
        to_sock_addr(to_addr.sin6_addr, from_ip);
    }

    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
    static inline void to_sock_addr(sockaddr_storage& to_addr, address const& from_ip) noexcept {
        if (from_ip.is_v4()) {
            to_sock_addr(reinterpret_cast<sockaddr_in&>(to_addr), from_ip.as_v4());
        } else if (from_ip.is_v6()) {
            to_sock_addr(reinterpret_cast<sockaddr_in6&>(to_addr), from_ip.as_v6());
        } else {
            // invalid ip address
        }
    }

    static inline void to_sock_addr(sockaddr_storage& to_addr, ipv4 from_ip) noexcept {
        to_sock_addr(reinterpret_cast<sockaddr_in&>(to_addr), from_ip);
    }

    static inline void to_sock_addr(sockaddr_storage& to_addr, ipv6 const& from_ip) noexcept {
        to_sock_addr(reinterpret_cast<sockaddr_in6&>(to_addr), from_ip);
    }

    // technically sockaddr cannot hold ipv6 addresses
    //    static inline void to_sock_addr(sockaddr& to_addr, socklen_t& len, address const& from_ip) noexcept
    //    {
    //        if (from_ip.is_v4()) {
    //            to_sock_addr(reinterpret_cast<sockaddr_in&>(to_addr), from_ip.as_v4());
    //            len = sizeof(sockaddr_in);
    //        } else if (from_ip.is_v6()) {
    //            to_sock_addr(reinterpret_cast<sockaddr_in6&>(to_addr), from_ip.as_v6());
    //            len = sizeof(sockaddr_in6);
    //        } else {
    //            // invalid ip address
    //        }
    //    }
    //
    //    static inline void to_sock_addr(sockaddr& to_addr, socklen_t& len, ipv4 from_ip) noexcept {
    //        to_sock_addr(reinterpret_cast<sockaddr_in&>(to_addr), from_ip);
    //        len = sizeof(sockaddr_in);
    //    }
    //
    //    // technically sockaddr cannot hold ipv6 addresses
    //    static inline void to_sock_addr(sockaddr& to_addr, socklen_t& len, ipv6 const& from_ip) noexcept {
    //        to_sock_addr(reinterpret_cast<sockaddr_in6&>(to_addr), from_ip);
    //        len = sizeof(sockaddr_in6);
    //    }

    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)


    /**
     * Generate a sockaddr_in/... from the specified ipv4/ipv6/address
     * `sockaddr` is always used as a middle-man and should not be "made"; if you need a `sockaddr`,
     * then you have to make a sockaddr_in or sockaddr_in6 and cast them out and make sure that socklen_t will
     * contain the right size as well.
     */
    template <typename SocketIPType, typename Addr>
        requires(istl::part_of<SocketIPType, sockaddr_storage> &&
                 istl::part_of<stl::remove_cvref_t<Addr>, address, ipv4, ipv6>)
    [[nodiscard]] static inline SocketIPType make_sock_addr(Addr&& from_in) noexcept {
        SocketIPType sock_addr{}; // init with zeros
        to_sock_addr(sock_addr, std::forward<Addr>(from_in));
        return sock_addr;
    }

    // some of these functions are constexpr-friendly
    template <typename SocketIPType, typename Addr>
        requires(istl::part_of<SocketIPType, sockaddr_in, sockaddr_in6, in_addr, in6_addr> &&
                 istl::part_of<stl::remove_cvref_t<Addr>, address, ipv4, ipv6>)
    [[nodiscard]] static constexpr SocketIPType make_sock_addr(Addr&& from_in) noexcept {
        SocketIPType sock_addr{}; // initialize with zeros
        to_sock_addr(sock_addr, std::forward<Addr>(from_in));
        return sock_addr;
    }


    //////////////////////////////////////// Address Wrappers ////////////////////////////////////////

    /**
     * Generic socket address.
     *
     * This is a wrapper around `sockaddr_storage` which holds family address.
     * This should have enough memory to contain any address struct for the system on which it is compiled.
     */
    class sock_address_any {
        // The max size of an address, in bytes
        static constexpr size_t max_address_size = sizeof(sockaddr_storage);

      public:
        constexpr sock_address_any() noexcept                                   = default;
        constexpr sock_address_any(sock_address_any const&) noexcept            = default;
        constexpr sock_address_any(sock_address_any&&) noexcept                 = default;
        constexpr sock_address_any& operator=(sock_address_any const&) noexcept = default;
        constexpr sock_address_any& operator=(sock_address_any&&) noexcept      = default;
        constexpr ~sock_address_any() noexcept                                  = default;

        static constexpr sock_address_any invalid() noexcept {
            sock_address_any addr{};
            addr.addr_len = max_address_size + 1;
            return addr;
        }

        /**
         * Constructs an address.
         * @param addr Pointer to a buffer holding the address.
         * @param n The number of valid bytes in the address
         */
        sock_address_any(const sockaddr* addr, socklen_t n) noexcept : addr_len{n} {
            std::memcpy(&addr_storage, addr, n);
        }


        /**
         * Constructs an address.
         * @param addr The buffer holding the address.
         * @param n The number of valid bytes in the address
         */
        sock_address_any(const sockaddr_storage& addr, socklen_t n) noexcept : addr_len{n} {
            std::memcpy(&addr_storage, &addr, n);
        }

        /**
         * Copies another address to this one.
         * @param addr The other address to copy into this one.
         */
        // constexpr sock_address_any(const sock_address& addr)
        //   : sock_address_any(addr.sockaddr_ptr(), addr.size()) {}

        /**
         * Get the size of the address. The number of bytes that are a valid part of the address.
         */
        [[nodiscard]] constexpr socklen_t size() const noexcept {
            return addr_len;
        }

        [[nodiscard]] constexpr sa_family_t family() const noexcept {
            return addr_storage.ss_family;
        }

        // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
        /**
         * Gets a pointer to this object cast to a sockaddr.
         */
        [[nodiscard]] const sockaddr* sockaddr_ptr() const noexcept {
            return reinterpret_cast<const sockaddr*>(&addr_storage);
        }

        /**
         * Gets a pointer to this object cast to a sockaddr.
         */
        [[nodiscard]] sockaddr* sockaddr_ptr() noexcept {
            return reinterpret_cast<sockaddr*>(&addr_storage);
        }
        // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)

        [[nodiscard]] constexpr socklen_t* socklen_ptr() noexcept {
            return &addr_len;
        }

        [[nodiscard]] constexpr socklen_t const* socklen_ptr() const noexcept {
            return &addr_len;
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            // should we check the validity of the sockaddr_storage as well?
            return addr_len <= max_address_size;
        }

        [[nodiscard]] explicit constexpr operator bool() const noexcept {
            return is_valid();
        }

        [[nodiscard]] operator address() const noexcept {
            return make_addr<address>(addr_storage);
        }

        [[nodiscard]] operator ipv4() const noexcept {
            return make_addr<ipv4>(addr_storage);
        }

        [[nodiscard]] operator ipv6() const noexcept {
            return make_addr<ipv6>(addr_storage);
        }

        [[nodiscard]] constexpr bool operator==(const sock_address_any& rhs) const noexcept {
            return size() == rhs.size() && std::memcmp(sockaddr_ptr(), rhs.sockaddr_ptr(), size()) == 0;
        }

        [[nodiscard]] constexpr bool operator!=(const sock_address_any& rhs) const noexcept {
            return !operator==(rhs);
        }

      private:
        // Storage for any kind of socket address
        sockaddr_storage addr_storage{};

        // Length of the address (in bytes)
        socklen_t addr_len{max_address_size};
    };

} // namespace webpp

#endif // WEBPP_SOCKET_ADDRESS_HPP
