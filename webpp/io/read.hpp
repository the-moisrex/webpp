// Created by moisrex on 7/4/23.

#ifndef WEBPP_IO_READ_HPP
#define WEBPP_IO_READ_HPP

#include "../std/ranges.hpp"
#include "../std/tag_invoke.hpp"
#include "./buffer.hpp"
#include "./syscalls.hpp"
#include "open.hpp"

namespace webpp::io {

    struct async_read_some {
        void set_value(auto io, int file_descriptor, buffer_span buf) noexcept {
            // request a read, and set a callback
            if (const auto val = syscall(read, io, file_descriptor, buf, *this); val != 0) {
                set_error(io, val);
            }
        }

        // callback from io
        void operator()(auto io, int read_amount) noexcept {
            set_done(io, read_amount);
        }
    };

    template <typename Task>
    struct async_read_until {
        Task task;

        void operator()(auto io, int file_descriptor, buffer_span buf) const noexcept {
            int  read_size = 0;
            auto it        = buf.begin();
            for (;;) {
                read_size = task(io, file_descriptor, buf);
                if (read_size <= -1) [[unlikely]] {
                    set_error(io, read_size);
                    return;
                } else if (read_size == 0) {
                    set_done(io);
                } else {
                    set_value(io, file_descriptor, buffer_span{it, static_cast<stl::size_t>(read_size)});
                }
                it += read_size;
            }
        }
    };


    inline constexpr void async_file_stats(auto io, int file_descriptor) noexcept {
        const auto stats = io.request_file_stats(file_descriptor);
        if (stats != 0) {
            set_error(io, stats);
        }
        set_value(io, stats);
    }


    constexpr auto read(int file_descriptor) noexcept {
        return just(file_descriptor) >> async_file_stats >> let_value([](auto stats) {
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
