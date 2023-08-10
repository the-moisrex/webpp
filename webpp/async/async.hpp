// Created by moisrex on 8/1/23.

#ifndef WEBPP_ASYNC_ASYNC_HPP
#define WEBPP_ASYNC_ASYNC_HPP

#include "../std/tag_invoke.hpp"

namespace webpp::async {

    inline constexpr struct create_instance_tag {
        template <typename T, typename ExecContextType, typename PreviousType>
        constexpr auto
        operator()(stl::type_identity<T>, ExecContextType&& ctx, PreviousType&& previous) const noexcept {
            if constexpr (stl::tag_invocable<stl::type_identity<T>,
                                             create_instance_tag,
                                             ExecContextType,
                                             PreviousType>) {
                return stl::tag_invoke(*this, ctx, stl::forward<PreviousType>(previous));
            } else if constexpr (stl::is_constructible_v<T,
                                                         create_instance_tag,
                                                         ExecContextType,
                                                         PreviousType>) {
                return T{*this, ctx, stl::forward<PreviousType>(previous)};
            } else if constexpr (stl::is_constructible_v<T, ExecContextType, PreviousType>) {
                return T{ctx, stl::forward<PreviousType>(previous)};
            } else if constexpr (stl::is_constructible_v<T, PreviousType>) {
                return T{stl::forward<PreviousType>(previous)};
            } else if constexpr (stl::is_constructible_v<T, ExecContextType>) {
                return T{ctx};
            } else if constexpr (stl::is_default_constructible_v<T>) {
                return T{};
            } else {
                static_assert_false(T, "Cannot create the obejct.");
            }
        }
    } create_instance;

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
