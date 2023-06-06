// Created by moisrex on 6/6/23.

#ifndef WEBPP_IP_ENDPOINT_HPP
#define WEBPP_IP_ENDPOINT_HPP

#include "../std/concepts.hpp"
#include "../std/vector.hpp"
#include "address.hpp"


#ifdef _WIN32
#    include <winsock2.h>
#    include <ws2tcpip.h>
#    pragma comment(lib, "Ws2_32.lib")
#else
#    include <arpa/inet.h>
#    include <netdb.h>
#    include <netinet/in.h>
#    include <sys/socket.h>
#    include <unistd.h>
#endif

namespace webpp {

    template <typename T>
    concept Endpoint = requires(T ep) {
                           requires stl::destructible<T>;
                           requires stl::derived_from<address, T>;

                           // it should be noexcept, even if you have to use "try-catch"
                           { ep.is_bindable() } noexcept -> stl::same_as<bool>;

                           { ep.is_tcp() } noexcept -> stl::same_as<bool>;
                           { ep.is_udp() } noexcept -> stl::same_as<bool>;
                           { ep.port() } noexcept -> stl::same_as<stl::uint16_t>;
                       };

    template <typename T>
    concept EndpointList = requires(T ep) {
                               { stl::begin(ep) } -> stl::random_access_iterator;
                               { stl::end(ep) } -> stl::random_access_iterator;
                           };

    /**
     * Endpoint is a combination of
     *   - Protocol (TCP/UDP/...)
     *   - Bind-able IP-Address
     *   - Port Number
     *
     * Attention: endpoint does not include a domain name because the developer may like to use custom-routing
     *            to manage the domains; for example he/she may want to the sub-domain to be a username
     *            which is read from the database; like some typical blogging websites that each username
     *            has a sub-domain for him/herself.
     */
    struct endpoint : public address {
        enum protocol {
            tcp = 6,
            udp = 17,
            // icmp   = 1,
            // igmp   = 2,
            // sctp   = 132,
            // gre    = 47,
            // esp    = 50,
            // ah     = 51,
            // icmpv6 = 58
        };

        constexpr endpoint(protocol inp_proto, address inp_addr) noexcept
          : address{inp_addr},
            proto{inp_proto} {}
        constexpr endpoint(endpoint const&) noexcept            = default;
        constexpr endpoint(endpoint&&) noexcept                 = default;
        constexpr endpoint& operator=(endpoint const&) noexcept = default;
        constexpr endpoint& operator=(endpoint&&) noexcept      = default;
        constexpr ~endpoint() noexcept                          = default;

        [[nodiscard]] constexpr bool is_udp() const noexcept {
            return proto == udp;
        }

        [[nodiscard]] constexpr bool is_tcp() const noexcept {
            return proto == tcp;
        }

        [[nodiscard]] constexpr bool operator==(protocol inp_proto) const noexcept {
            return proto == inp_proto;
        }

        [[nodiscard]] constexpr stl::uint16_t port() const noexcept {
            return port_num;
        }


        [[nodiscard]] bool is_bindable() const noexcept {
            if (!this->is_valid()) {
                return false;
            }
            if (this->is_loopback() || this->is_zero()) {
                return true;
            }
            /*
                        if (int sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol); sockfd == -1) {
                            return false;
                        } else {

                            int bind_status = bind(sockfd, p->ai_addr, p->ai_addrlen);
            #ifdef _WIN32
                            closesocket(sockfd);
            #else
                            close(sockfd);
            #endif
                            if (bind_status == 0) {
                                return true;
                            }
                        }
            */
            return false;
        }

      private:
        protocol      proto;
        stl::uint16_t port_num;
    };



    template <typename Vec = stl::vector<endpoint>>
    struct endpoint_list : Vec {
        using Vec::Vec;
    };


    template <stl::size_t N>
    struct endpoint_array : endpoint_list<stl::array<endpoint, N>> {
        using endpoint_list<stl::array<endpoint, N>>::endpoint_list;
    };



    template <typename... Up>
        requires((stl::same_as<endpoint, Up> && ...))
    endpoint_array(endpoint, Up...) -> endpoint_array<(1 + sizeof...(Up))>;

    template <stl::size_t N>
    endpoint_array(const endpoint (&)[N]) -> endpoint_array<N>;


} // namespace webpp

#endif // WEBPP_IP_ENDPOINT_HPP
