#ifndef WEBPP_IO_IO_CONCEPTS_HPP
#define WEBPP_IO_IO_CONCEPTS_HPP

#include <concepts>
#include <ranges>

namespace webpp::io {

    /**
     * A Task is a "view" of some functions that may or may not try to do some IO operation asynchronously
     */
    template <typename T>
    concept Task = std::ranges::view<T> && requires(T task) {
                                               task.then([] {}); // then
                                               task | [] {};
                                               task | [](auto) {};
                                           };

    template <typename T>
    concept IOService = requires(T io, char* data, unsigned long long size, int fd) {
                            { io.read(fd, data, size) } noexcept -> Task;
                            { io.write(fd, data, size) } noexcept -> Task;
                        };


} // namespace webpp::io

#endif // WEBPP_IO_IO_CONCEPTS_HPP
