// Created by moisrex on 8/1/23.

#ifndef WEBPP_ASYNC_ASYNC_HPP
#define WEBPP_ASYNC_ASYNC_HPP

#include "../std/tag_invoke.hpp"

namespace webpp::async {

    /**
     * Set Done
     */
    inline constexpr struct set_done_tag {

        // Customization Point
        template <typename T>
            requires stl::tag_invocable<set_done_tag, T>
        constexpr stl::tag_invoke_result_t<set_done_tag, T> operator()(T&& next) const
          noexcept(stl::nothrow_tag_invocable<set_done_tag, T>) {
            return stl::tag_invoke(*this, stl::forward<T>(next));
        }

        // default impl
        template <typename T>
        friend constexpr decltype(auto) tag_invoke(set_done_tag, T&& next) noexcept {
            if constexpr (requires { next.set_done(); }) {
                return stl::forward<T>(next).set_done();
            } else {
                // nothing to do
            }
        }
    } set_done;


    /**
     * Set Value
     */
    inline constexpr struct set_value_tag {

        // Customization Point
        template <typename T, typename... Args>
            requires stl::tag_invocable<set_value_tag, T, Args...>
        constexpr stl::tag_invoke_result_t<set_value_tag, T> operator()(T&& next, Args&&... args) const
          noexcept(stl::nothrow_tag_invocable<set_value_tag, T, Args...>) {
            return stl::tag_invoke(*this, stl::forward<T>(next), stl::forward<Args>(args)...);
        }

        // default impl
        template <typename T, typename... Args>
        friend constexpr decltype(auto) tag_invoke(set_value_tag, T&& next, Args&&... args) noexcept {
            if constexpr (requires { next.set_value(stl::forward<Args>(args)...); }) {
                return stl::forward<T>(next).set_value(stl::forward<Args>(args)...);
            } else {
                // ignoring setting values
            }
        }
    } set_value;


    // todo: set_error

} // namespace webpp::async

#endif // WEBPP_ASYNC_ASYNC_HPP
