#ifndef WEBPP_IO_IO_CONCEPTS_HPP
#define WEBPP_IO_IO_CONCEPTS_HPP

#include "../std/coroutine.hpp"

#include <ranges>

namespace webpp::io {

    /**
     * An I/O-Task is a "view" of some functions that may or may not try to do
     * some IO operation asynchronously
     */
    template <typename T>
    concept IOTask = std::ranges::view<T> && istl::CoroutineAwaiter<T> && requires(T task) {
                                                                              task.then([] {}); // then
                                                                              task | [] {};
                                                                              task | [](auto) {};
                                                                          };

    template <typename T>
    concept IOService = requires(T io, char* data, unsigned long long size, int fd) {
                            { io.read(fd, data, size) } noexcept -> IOTask;
                            { io.write(fd, data, size) } noexcept -> IOTask;
                        };


} // namespace webpp::io

#endif // WEBPP_IO_IO_CONCEPTS_HPP
