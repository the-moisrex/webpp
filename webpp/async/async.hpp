// Created by moisrex on 8/1/23.

#ifndef WEBPP_ASYNC_ASYNC_HPP
#define WEBPP_ASYNC_ASYNC_HPP

#include "../std/tag_invoke.hpp"

namespace webpp::async {

    /**
     * Create an Instance of the specified async operation
     *
     * Usage:
     * @code
     *           connect(stl::type_identity<algorithm>{},
     *                   execution_context,
     *                   previous_algorithm,
     *                   args...);
     * @endcode
     */
    inline constexpr struct connect_tag {
        template <typename T, typename ExecContextType, typename PreviousType>
        constexpr auto
        operator()(stl::type_identity<T>, ExecContextType&& ctx, PreviousType&& previous) const {
            if constexpr (stl::tag_invocable<connect_tag,
                                             stl::type_identity<T>,
                                             ExecContextType,
                                             PreviousType>) {
                return stl::tag_invoke(*this,
                                       stl::type_identity<T>{},
                                       ctx,
                                       stl::forward<PreviousType>(previous));
            } else if constexpr (stl::is_constructible_v<T, connect_tag, ExecContextType, PreviousType>) {
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
    } connect;


    /**
     * Start Operation
     * This CPO (Customization Point Object) helps to start a connected task
     */
    inline constexpr struct start_tag {

        /// calls tag_invoke(start, task)
        template <typename T>
            requires(stl::tag_invocable<start_tag, T>)
        [[nodiscard]] constexpr decltype(auto) operator()(T&& task) const
          noexcept(noexcept(stl::nothrow_tag_invocable<start_tag, T>)) {
            return stl::tag_invoke(*this, stl::forward<T>(task));
        }

        /// calls task.start()
        template <typename T>
            requires requires(T task) { task.start(); }
        [[nodiscard]] constexpr decltype(auto) operator()(T&& task) const
          noexcept(noexcept(stl::forward<T>(task).start())) {
            return stl::forward<T>(task).start();
        }
    } start;


    /**
     * Yield Value
     * Pass the specified value to the sub-tasks to handle
     */
    inline constexpr struct yield_value_tag {

        /// calls tag_invoke(yield_value, task, value)
        template <typename TaskT, typename ValueT>
            requires(stl::tag_invocable<yield_value_tag, TaskT, ValueT>)
        constexpr void operator()(TaskT&& task, ValueT&& value) const
          noexcept(stl::nothrow_tag_invocable<yield_value_tag, TaskT, ValueT>) {
            stl::tag_invoke(*this, stl::forward<TaskT>(task), stl::forward<ValueT>(value));
        }

        /// calls task.yield_value(value)
        template <typename TaskT, typename ValueT>
            requires(requires(TaskT task, ValueT value) { task.yield_value(value); })
        constexpr void operator()(TaskT&& task, ValueT&& value) const
          noexcept(noexcept(stl::forward<TaskT>(task).yield_value(stl::forward<ValueT>(value)))) {
            stl::forward<TaskT>(task).yield_value(stl::forward<ValueT>(value));
        }
    } yield_value;

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
