// Created by moisrex on 6/8/23.

#ifndef WEBPP_SOCKET_ADDRESS_HPP
#define WEBPP_SOCKET_ADDRESS_HPP

#include "../ip/address.hpp"
#include "./os.hpp"

namespace webpp {

    static constexpr void to_addr(ipv4& to_ip, sockaddr_in from_in) noexcept {
        to_ip = from_in.sin_addr.s_addr; // s_addr is uint32_t
    }

    static constexpr void to_addr(address& to_ip, sockaddr_in from_in) noexcept {
        to_ip = ipv4{from_in.sin_addr.s_addr}; // s_addr is uint32_t
    }

    static constexpr void to_addr(ipv6& to_ip, sockaddr_in6 const& from_in) noexcept {
        to_ip = from_in.sin6_addr.s6_addr;
    }

    static constexpr void to_addr(address& to_ip, sockaddr_in6 const& from_in) noexcept {
        to_ip = ipv6{from_in.sin6_addr.s6_addr};
    }


    /**
     * Generate a ipv4/ipv6/address from the specified sockaddr_in
     */
    template <typename Addr = address>
    static constexpr Addr from_addr(sockaddr_in from_in) noexcept {
        Addr to_ip;
        to_addr(to_ip, from_in);
        return to_ip;
    }

    /**
     * Generate a ipv4/ipv6/address from the specified sockaddr_in6
     */
    template <typename Addr = address>
        requires(std::same_as<Addr, ipv4>) // it's ipv6, can't be put into ipv4
    static constexpr Addr from_addr(sockaddr_in6 const& from_in) noexcept {
        Addr to_ip;
        to_addr(to_ip, from_in);
        return to_ip;
    }


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
            std::memcpy(&addr_, addr, n);
        }


        /**
         * Constructs an address.
         * @param addr The buffer holding the address.
         * @param n The number of valid bytes in the address
         */
        sock_address_any(const sockaddr_storage& addr, socklen_t n) noexcept : addr_len{n} {
            std::memcpy(&addr_, &addr, n);
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
            return addr_.ss_family;
        }

        /**
         * Gets a pointer to this object cast to a sockaddr.
         */
        [[nodiscard]] const sockaddr* sockaddr_ptr() const noexcept {
            return reinterpret_cast<const sockaddr*>(&addr_);
        }

        /**
         * Gets a pointer to this object cast to a sockaddr.
         */
        [[nodiscard]] sockaddr* sockaddr_ptr() noexcept {
            return reinterpret_cast<sockaddr*>(&addr_);
        }

        [[nodiscard]] socklen_t* socklen_ptr() noexcept {
            return &addr_len;
        }

        [[nodiscard]] socklen_t const* socklen_ptr() const noexcept {
            return &addr_len;
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept {
            return addr_len <= max_address_size;
        }

        [[nodiscard]] constexpr operator bool() const noexcept {
            return is_valid();
        }

      private:
        // Storage for any kind of socket address
        sockaddr_storage addr_{};

        // Length of the address (in bytes)
        socklen_t addr_len{max_address_size};
    };

    [[nodiscard]] static constexpr bool operator==(const sock_address_any& lhs,
                                                   const sock_address_any& rhs) noexcept {
        return lhs.size() == rhs.size() &&
               std::memcmp(lhs.sockaddr_ptr(), rhs.sockaddr_ptr(), lhs.size()) == 0;
    }

    [[nodiscard]] static constexpr bool operator!=(const sock_address_any& lhs,
                                                   const sock_address_any& rhs) noexcept {
        return !operator==(lhs, rhs);
    }

} // namespace webpp

#endif // WEBPP_SOCKET_ADDRESS_HPP
