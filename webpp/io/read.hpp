// Created by moisrex on 7/4/23.

#ifndef WEBPP_IO_READ_HPP
#define WEBPP_IO_READ_HPP

#include "./io_traits.hpp"
#include "open.hpp"

namespace webpp::io {

    struct async_read_some {
        enum read_status { idle, requested, done } status;

        void set_value(auto io, int file_descriptor, stl::size_t amount, char* buf) noexcept {
            // request a read, and set a callback
            const auto val = io.request_read(file_descriptor, amount, buf, *this);
            if (val != 0) {
                set_error(io, val);
                status = done;
                return;
            }
            status = requested;
        }

        // callback from io
        void operator()(auto io, int read) noexcept {
            status = done;
            set_done(io, read);
        }
    };


    template <typename Task>
    struct async_read_until {
        Task task;

        void operator()(auto io, int file_descriptor, stl::size_t amount, char* buf) const noexcept {
            int read_size = 0;
            for (;;) {
                read_size = task(io, file_descriptor, amount, buf);
                if (read_size <= -1) [[unlikely]] {
                    set_error(io, read_size);
                    return;
                } else if (read_size == 0) {
                    set_done(io);
                } else {
                    set_value(io, file_descriptor, buf, read_size);
                }
                buf += read_size;
            }
        }
    };


    struct async_file_stats {
        void operator()(auto io, int file_descriptor) const noexcept {
            const auto stats = io.request_file_stats();
            if (stats != 0) {
                set_error(io, stats);
            }
            set_value(io, stats);
        }
    };


    constexpr auto read(int file_descriptor) noexcept {
        return just(file_descriptor) >> async_file_stats{} >> let_value([](auto stats) {
                   return stats.size();
               }) >>
               async_do_until(async_read_some{});
    }

    constexpr auto read(std::filesystem::path const& file_path) noexcept {
        // 1. open the file
        // 2. get the size of the file
        // 3. get buffer
        // 4. read some
        return open(file_path) >> read;
    }

} // namespace webpp::io

#endif // WEBPP_IO_READ_HPP
