// Created by moisrex on 4/18/23.

#ifndef WEBPP_HTTP_ROUTER_DISABLER_HPP
#define WEBPP_HTTP_ROUTER_DISABLER_HPP

#include "valves.hpp"

namespace webpp::http {

    /**
     * Simple Route Disabler Valve
     *
     * Usage:
     * @code
     *  route_disabler disabler{true};
     *
     *  dynamic_router router // ... ;
     *
     *  router += root / "pages" >> disabler >> &pages::index;
     * @endcode
     */
    struct route_disabler : valve<route_disabler> {

        constexpr route_disabler() noexcept = default;
        constexpr route_disabler(bool inp_is_enabled) noexcept : is_enabled_value{inp_is_enabled} {}
        constexpr route_disabler(route_disabler const&) noexcept                  = delete;
        constexpr route_disabler(route_disabler&&) noexcept                       = default;
        constexpr route_disabler& operator=(route_disabler&&) noexcept            = delete;
        constexpr route_disabler& operator=(route_disabler const& other) noexcept = default;
        constexpr ~route_disabler() noexcept                                      = default;

        constexpr route_disabler& operator=(bool inp_is_enabled) noexcept {
            is_enabled_value = inp_is_enabled;
            return *this;
        }

        constexpr void enable() noexcept {
            is_enabled_value = true;
        }

        constexpr void disable() noexcept {
            is_enabled_value = false;
        }

        constexpr bool toggle() noexcept {
            return (is_enabled_value = !is_enabled_value);
        }

        constexpr bool set(bool inp_is_enabled = true) noexcept {
            return (is_enabled_value = inp_is_enabled);
        }

        template <Traits TraitsType>
        constexpr bool operator()(basic_context<TraitsType>&) const noexcept {
            return is_enabled();
        }

        constexpr void to_string(istl::String auto& out) const {
            if (is_enabled()) {
                out.append(" enabled(true)");
            } else {
                out.append(" enabled(false)");
            }
        }

        [[nodiscard]] constexpr bool is_disabled() const noexcept {
            return !is_enabled();
        }

        [[nodiscard]] constexpr bool is_enabled() const noexcept {
            return is_enabled_value;
        }

      private:
        bool is_enabled_value = true;
    };

} // namespace webpp::http


#endif // WEBPP_HTTP_ROUTER_DISABLER_HPP
