#ifndef WEBPP_IO_IO_CONCEPTS_HPP
#define WEBPP_IO_IO_CONCEPTS_HPP

#include "../async/async_concepts.hpp"


namespace webpp::io {

    /**
     * An I/O-Task is an special type of async operation.
     * We need an special type of async operation for I/O operations because this project will probably
     * support multiple underlying I/O interfaces (io_uring, epoll, ...).
     */
    template <typename T>
    concept IOTask = async::Task<T>;

    template <typename T>
    concept IOScheduler =
      async::Scheduler<T> && requires(T sched, char* data, unsigned long long size, int fd) {
                                 { sched.read(fd, data, size) } noexcept -> IOTask;
                                 { sched.write(fd, data, size) } noexcept -> IOTask;
                             };


    template <typename T>
    concept IOIterator = requires {

                         };

    template <typename T>
    concept IOTraits = requires { requires IOIterator<typename T::iterator>; };


} // namespace webpp::io

#endif // WEBPP_IO_IO_CONCEPTS_HPP
