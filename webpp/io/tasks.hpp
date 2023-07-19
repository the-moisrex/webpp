// Created by moisrex on 7/5/23.

#ifndef WEBPP_TASKS_HPP
#define WEBPP_TASKS_HPP

#include "../std/type_traits.hpp"

namespace webpp {

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



    struct task {

        constexpr task(task const&) noexcept            = default;
        constexpr task(task&&) noexcept                 = default;
        constexpr task& operator=(task const&) noexcept = default;
        constexpr task& operator=(task&&) noexcept      = default;

        constexpr ~task() noexcept = default;

        template <typename F>
        consteval auto then(F&& func) noexcept {}
    };

} // namespace webpp

#endif // WEBPP_TASKS_HPP
