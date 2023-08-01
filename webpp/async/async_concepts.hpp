// Created by moisrex on 7/22/23.

#ifndef WEBPP_ASYNC_CONCEPTS_HPP
#define WEBPP_ASYNC_CONCEPTS_HPP

#include "../std/concepts.hpp"
#include "../std/coroutine.hpp"
#include "../std/iterator.hpp"
#include "../std/ranges.hpp"
#include "../std/type_traits.hpp"

namespace webpp::async {


    /**
     * A type that holds callbacks
     */
    template <typename T>
    concept AsyncPromise = requires(T awaitable, stl::true_type lambda) {
                               { awaitable.then(lambda) } noexcept -> stl::same_as<T&>;
                               { awaitable.except(lambda) } noexcept -> stl::same_as<T&>;
                               { awaitable >> lambda } noexcept -> stl::same_as<T&>;
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


    /**
     * Scheduler is something that the user will enqueue their work with it. This should be lightweight.
     */
    template <typename T>
    concept Scheduler = stl::regular<T> && (sizeof(T) <= sizeof(void*));


    /**
     * Execution Context is where the I/O operations and Async Tasks are processed.
     */
    template <typename T>
    concept ExecutionContext = requires(T async) {
                                   requires stl::is_nothrow_default_constructible_v<T>;

                                   // Run all the tasks
                                   async.run();

                                   // This is what gets stored in the "enabled_traits" objects.
                                   { async.scheduler() } -> Scheduler;
                               };


    namespace details {
        template <typename T>
        concept BasicTask = stl::movable<T> && stl::is_nothrow_move_constructible_v<T> && stl::copyable<T>;
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
    concept YieldableTask = details::BasicTask<T> && requires(T task) {
                                                         { stl::begin(task) } noexcept -> TaskYielder;
                                                         { stl::end(task) } noexcept -> TaskYielder;
                                                     };

    template <typename T>
    concept OneShotTask = details::BasicTask<T> && requires(T task) {
                                                       { task() };
                                                   };

    template <typename T>
    concept Task = YieldableTask<T> || OneShotTask<T>;

    /**
     * Any type that has the ability to be chained up with other types of callables.
     */
    template <typename T>
    concept Chainable = stl::regular<T> && requires(T task, stl::true_type lambda) {
                                               task.then(lambda);
                                               task.on_error(lambda);
                                               task.finally(lambda);
                                               task >> lambda; // same as then
                                               task | lambda;  // on error
                                               task& lambda;   // finally
                                           };

    /**
     * Event Chain is an intrusive list of tasks (callbacks/io-operations/...).
     */
    template <typename T>
    concept TaskChain = Chainable<T> && requires(T task) {
                                            // For convenience, nobody should be calling this
                                            task(); // run the task chain fully

                                            // Iterate the task, hopefully in a non-blocking way
                                            task.advance();
                                        };

    /**
     * Event Loop can hold a list of Event-Chains and will call their "continuation"s.
     * Event loops are supposed to be run inside an Execution Context (a thread-pool or something).
     */
    template <typename T>
    concept EventLoop = requires(T loop) { loop(); };


} // namespace webpp::async

#endif // WEBPP_ASYNC_CONCEPTS_HPP
