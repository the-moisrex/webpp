// Created by moisrex on 6/6/23.

#ifndef WEBPP_IP_ENDPOINT_HPP
#define WEBPP_IP_ENDPOINT_HPP

#include "../socket/socket.hpp"
#include "../std/concepts.hpp"
#include "../std/vector.hpp"
#include "ip_address.hpp"



namespace webpp {

    template <typename T>
    concept Endpoint = requires(T ep) {
                           requires stl::destructible<T>;
                           requires stl::derived_from<ip_address, T>;

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
    struct endpoint : public ip_address {
        enum protocol { tcp = SOCK_STREAM, udp = SOCK_DGRAM };

        constexpr endpoint(protocol inp_proto, ip_address inp_addr, stl::uint16_t inp_port) noexcept
          : ip_address{inp_addr},
            proto{inp_proto},
            port_num{inp_port} {}
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

        [[nodiscard]] constexpr ip_address& as_address() noexcept {
            return static_cast<ip_address&>(*this);
        }

        [[nodiscard]] constexpr ip_address const& as_address() const noexcept {
            return static_cast<ip_address const&>(*this);
        }


        [[nodiscard]] bool is_bindable() const noexcept {
            return this->is_valid() &&
                   basic_socket{as_address(), static_cast<int>(proto), 0, port_num}.is_valid();
        }

      private:
        protocol      proto    = udp;
        stl::uint16_t port_num = 0;
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
