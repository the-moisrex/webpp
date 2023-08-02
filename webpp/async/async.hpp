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
        [[nodiscard]] constexpr stl::tag_invoke_result_t<set_done_tag, T> operator()(T&& next) const
          noexcept(stl::nothrow_tag_invocable<set_done_tag, T>) {
            return stl::tag_invoke(*this, stl::forward<T>(next));
        }

        // default impl
        template <typename T>
        [[nodiscard]] friend constexpr decltype(auto) tag_invoke(set_done_tag, T&& next) noexcept {
            if constexpr (requires { next.set_done(); }) {
                return stl::forward<T>(next).set_done();
            } else {
                return stl::forward<T>(next);
            }
        }
    } set_done;


    // todo: set_value
    // todo: set_error

} // namespace webpp::async

#endif // WEBPP_ASYNC_ASYNC_HPP
