// Created by moisrex on 6/6/23.

#ifndef WEBPP_IP_ENDPOINT_HPP
#define WEBPP_IP_ENDPOINT_HPP

#include "../socket/socket.hpp"
#include "../std/concepts.hpp"
#include "../std/vector.hpp"
#include "ip_address.hpp"

#include <array>

namespace webpp {

    template <typename T>
    concept Endpoint = requires(stl::remove_cvref_t<T> ep) {
                           requires stl::destructible<stl::remove_cvref_t<T>>;
                           requires stl::derived_from<stl::remove_cvref_t<T>, ip_address>;

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
                               { *stl::begin(ep) } -> Endpoint;
                           };

    /**
     * IP Endpoint is a combination of
     *   - Protocol (TCP/UDP/...)
     *   - Bind-able IP-Address
     *   - Port Number
     *
     * This is IP-Endpoint and you should not and cannot use it for e.g. unix domain socket.
     *
     * Attention: endpoint does not include a domain name because the developer may like to use custom-routing
     *            to manage the domains; for example he/she may want to the sub-domain to be a username
     *            which is read from the database; like some typical blogging websites that each username
     *            has a sub-domain for him/herself.
     */
    struct ip_endpoint : public ip_address {
        enum protocol { tcp = SOCK_STREAM, udp = SOCK_DGRAM };

        constexpr ip_endpoint(protocol inp_proto, ip_address inp_addr, stl::uint16_t inp_port) noexcept
          : ip_address{inp_addr},
            proto{inp_proto},
            port_num{inp_port} {}
        constexpr ip_endpoint(ip_endpoint const&) noexcept            = default;
        constexpr ip_endpoint(ip_endpoint&&) noexcept                 = default;
        constexpr ip_endpoint& operator=(ip_endpoint const&) noexcept = default;
        constexpr ip_endpoint& operator=(ip_endpoint&&) noexcept      = default;
        constexpr ip_endpoint() noexcept                              = default;
        constexpr ~ip_endpoint() noexcept                             = default;

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


        /**
         * Check if the specified endpoint is bind-able.
         * There can be multiple reasons why it's not bind-able.
         */
        [[nodiscard]] bool is_bindable() const noexcept {
            return this->is_valid() &&
                   basic_socket{as_address(), static_cast<int>(proto), 0, port_num}.is_valid();
        }

      private:
        protocol      proto    = udp;
        stl::uint16_t port_num = 0;
    };



    template <typename Vec = stl::vector<ip_endpoint>>
    struct ip_endpoint_list : Vec {
        using Vec::Vec;
    };


    template <stl::size_t N>
    using ip_endpoint_array = stl::array<ip_endpoint, N>;

} // namespace webpp

#endif // WEBPP_IP_ENDPOINT_HPP
