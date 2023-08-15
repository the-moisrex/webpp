// Created by moisrex on 8/1/23.

#ifndef WEBPP_ASYNC_ASYNC_HPP
#define WEBPP_ASYNC_ASYNC_HPP

#include "../std/tag_invoke.hpp"

namespace webpp::async {

    /**
     * Connect two tasks together and (depending on the implementation) get either a task chain,
     * or get an intrusive list of tasks if possible.
     *
     * Usage:
     * @code
     *    connect(previous_task, new_task);
     * @endcode
     */
    inline constexpr struct connect_tag {
        template <typename PrevTask, typename NewTask>
        constexpr auto operator()(PrevTask&& prev_task, NewTask&& new_task) const {
            if constexpr (requires { prev_task.connect(stl::forward<NewTask>(new_task)); }) {
                return prev_task.connect(stl::forward<PrevTask>(prev_task));
            } else if constexpr (stl::tag_invocable<connect_tag, PrevTask, NewTask>) {
                return stl::tag_invoke(*this,
                                       stl::forward<PrevTask>(prev_task),
                                       stl::forward<NewTask>(new_task));
            } else {
                static_assert_false(T, "Cannot create the object.");
            }
        }
    } connect;


    /**
     * Start/Continue Operation
     * This CPO (Customization Point Object) helps to advance a connected task
     */
    inline constexpr struct advance_tag {

        /// calls tag_invoke(advance, task)
        template <typename T>
            requires(stl::tag_invocable<advance_tag, T>)
        [[nodiscard]] constexpr bool operator()(T&& task) const
          noexcept(noexcept(stl::nothrow_tag_invocable<advance_tag, T>)) {
            return stl::tag_invoke(*this, stl::forward<T>(task));
        }

        /// calls task.advance()
        template <typename T>
            requires requires(T task) { task.advance(); }
        [[nodiscard]] friend constexpr bool
        tag_invoke(advance_tag, T&& task) noexcept(noexcept(stl::forward<T>(task).advance())) {
            using return_type = stl::remove_cvref_t<decltype(stl::forward<T>(task).advance())>;
            if constexpr (stl::same_as<return_type, bool>) {
                return stl::forward<T>(task).advance();
            } else if constexpr (stl::is_void_v<return_type>) {
                stl::forward<T>(task).advance();
                return false; // don't continue
            } else {
                static_cast<void>(stl::forward<T>(task).advance());
                return false;
            }
        }
    } advance;


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



    ////////////////////////////// Async Concepts //////////////////////////////



    namespace details {
        template <typename T>
        concept BasicTask = stl::movable<T> && stl::is_nothrow_move_constructible_v<T> && stl::copyable<T> &&
                            requires(T task1, T task2) {
                                { connect(task1, task2) }; // todo: inspect the returned type
                            };
    }

    /**
     * Task Yielder, yields values
     * It's kinda an iterator
     */
    template <typename T>
    concept TaskYielder = stl::movable<T> &&
                          requires(T iter) {
                              // almost weakly incrementable
                              { ++iter } noexcept -> stl::same_as<T&>;
                              // almost indirectly readable
                              { *iter } noexcept;
                              // is_done
                              { static_cast<bool>(iter) } noexcept;
                          }
    // #ifdef __cpp_lib_ranges
    //                           && std::ranges::view<T>
    // #endif
    // #ifdef __cpp_lib_coroutine
    //                           && istl::CoroutineAwaiter<T>
    // #endif
    ;

    template <typename T>
    concept IterableTask = details::BasicTask<T> && requires(T task) {
                                                        { stl::begin(task) } noexcept -> TaskYielder;
                                                        { stl::end(task) } noexcept -> TaskYielder;
                                                    };

    template <typename T>
    concept AdvancableTask = details::BasicTask<T> && requires(T task) {
                                                          { advance(task) } -> stl::same_as<bool>;
                                                      };

    template <typename T>
    concept Task = IterableTask<T> || AdvancableTask<T>;

    /**
     * Scheduler is something that the user will enqueue their work with it. This should be lightweight.
     */
    template <typename T>
    concept Scheduler = Task<T> && (sizeof(T) <= sizeof(void*)) && stl::copyable<T> && stl::movable<T> &&
                        stl::is_nothrow_move_assignable_v<T> && stl::is_nothrow_move_constructible_v<T>;

    /**
     * Execution Context is where the I/O operations and Async Tasks are processed.
     * An execution context is also a task, this way it'll be possible to chain the execution contexts up,
     * and so each execution context's "advance"ments are synchronized with the parent execution context
     * because it's being called by the parent execution context.
     */
    template <typename T>
    concept ExecutionContext = Task<T> && requires(T async) {
                                              // This is what gets stored in the "enabled_traits" objects.
                                              { async.scheduler() } -> Scheduler;
                                          };


    /**
     * Any type that has the ability to be chained up with other types of callables.
     */
    template <typename T>
    concept Chainable = requires(T task, stl::true_type lambda) {
                            task.then(lambda);
                            task.on_error(lambda);
                            task.finally(lambda);
                            task >> lambda; // same as then
                            task | lambda;  // on error
                            task& lambda;   // finally
                        };

    /**
     * Chainable Task is a list of tasks (callbacks/io-operations/...).
     */
    template <typename T>
    concept ChainableTask = Chainable<T> && Task<T>;

    /**
     * Event Loop can hold a list of Event-Chains and will call their "continuation"s.
     * Event loops are supposed to be run inside an Execution Context (a thread-pool or something).
     */
    template <typename T>
    concept EventLoop =
      ExecutionContext<T> && requires(T loop, stl::true_type event, stl::false_type lambda) {
                                 loop.on(event, lambda);
                                 loop.call(event);
                             };

    /**
     * Thread pool class helps to implement a vector/list of threads and push
     * tasks into this thread pool.
     *
     * List of features I'd like to see in the future:
     * - [ ] Fewer run-time overhead features:
     *   - [ ] Register methods before using them multiple times.
     *     Use cases:
     *     - [ ] Buffering the requests
     *     - [ ] Processing user requests (even parsing the request)
     * - [ ] Priority scheduling:
     *   - [ ] defer
     *   - [ ] dispatch
     *   - [ ] post
     * - [ ] Run in a specific thread
     *   - [ ] By thread id
     *   - [ ] By thread index
     *   - [ ] In the last thread which that function was processed on
     * - [ ] Stop, Pause, Continue, Start methods
     * - [ ] Join threads
     * - [ ] Underlying thread class:
     *   - [ ] std::thread
     *   - [ ] std::jthread
     *   - [ ] boost::thread
     *   - [ ] POSIX
     * - [ ] Constexpr way to hash a function object into a known number in the thread pool
     */
    template <typename T>
    concept ThreadPool = requires(T tp, stl::true_type lambda) {
                             tp.post(lambda);
                             tp.defer(lambda); // todo: fix these 3; I don't think they have the correct args
                             tp.dispatch(lambda);
                         };

} // namespace webpp::async

#endif // WEBPP_ASYNC_ASYNC_HPP
