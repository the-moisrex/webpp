#ifndef WEBPP_IO_IO_CONCEPTS_HPP
#define WEBPP_IO_IO_CONCEPTS_HPP

#include "../async/async_concepts.hpp"
#include "../std/coroutine.hpp"

#include <ranges>

namespace webpp::io {

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
    concept IOScheduler =
      async::Scheduler<T> && requires(T sched, char* data, unsigned long long size, int fd) {
                                 { sched.read(fd, data, size) } noexcept -> IOTask;
                                 { sched.write(fd, data, size) } noexcept -> IOTask;
                             };


} // namespace webpp::io

#endif // WEBPP_IO_IO_CONCEPTS_HPP
