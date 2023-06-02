#ifndef WEBPP_IO_IO_CONCEPTS_HPP
#define WEBPP_IO_IO_CONCEPTS_HPP

namespace webpp::io {

    template <typename T>
    concept Task = requires(T task) {
                       task.then([] {}); // then
                   };

    template <typename T>
    concept IOService = requires(T io, char* data, unsigned long long size, int fd) {
                            { io.read(fd, data, size) } noexcept -> Task;
                            { io.write(fd, data, size) } noexcept -> Task;
                        };
} // namespace webpp::io

#endif // WEBPP_IO_IO_CONCEPTS_HPP
