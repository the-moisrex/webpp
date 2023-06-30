#ifndef WEBPP_IO_IO_CONCEPTS_HPP
#define WEBPP_IO_IO_CONCEPTS_HPP

#include "../std/coroutine.hpp"

#include <ranges>

namespace webpp::io {

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
     * An I/O-Task is a "view" of some functions that may or may not try to do
     * some IO operation asynchronously
     */
    template <typename T>
    concept IOTask =
      std::ranges::view<T> && istl::CoroutineAwaiter<T> && requires(T task, stl::true_type lambda) {
                                                               task.then(lambda);
                                                               task >> lambda;
                                                           };

    template <typename T>
    concept IOService = requires(T io, char* data, unsigned long long size, int fd) {
                            { io.read(fd, data, size) } noexcept -> IOTask;
                            { io.write(fd, data, size) } noexcept -> IOTask;
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
     * Scheduler is something that the user will enqueue their work with it. This should be light weight.
     */
    template <typename T>
    concept IOScheduler = requires(T sched) {
                              requires stl::destructible<T>;
                              requires stl::copy_constructible<T>;
                              requires stl::equality_comparable<T>;
                          };

    /**
     * Execution Context is where the I/O operations and Async Tasks are processed.
     */
    template <typename T>
    concept ExecutionContext = requires(T io) {
                                   requires stl::is_nothrow_default_constructible_v<T>;

                                   // Run all the tasks
                                   io.run();

                                   // This is what gets stored in the "enabled_traits" objects.
                                   { io.scheduler() } -> IOScheduler;
                               };

} // namespace webpp::io

#endif // WEBPP_IO_IO_CONCEPTS_HPP
