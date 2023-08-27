// Created by moisrex on 7/4/23.

#ifndef WEBPP_IO_READ_HPP
#define WEBPP_IO_READ_HPP

#include "../std/ranges.hpp"
#include "../std/tag_invoke.hpp"
#include "./buffer.hpp"
#include "./io_concepts.hpp"
#include "./open.hpp"
#include "./syscalls.hpp"

namespace webpp::io {

    struct async_read_some {
        void set_value(IOScheduler auto io, int file_descriptor, buffer_span buf) noexcept {
            // request a read, and set a callback
            if (const auto val = syscall(read, io, file_descriptor, buf, *this); val != 0) {
                set_error(io, val);
            }
        }

        // callback from io
        void operator()(IOScheduler auto io, int read_amount) noexcept {
            set_done(io, read_amount);
        }
    };

    template <typename Task>
    struct async_until {
        [[no_unique_address]] Task task;

        constexpr auto connect(IOScheduler auto io) && noexcept {
            return operation_status{async::connect(io, stl::move(task))};
        }

      private:
        struct operation_status {
            [[no_unique_address]] async::connected_type<Task> task_state;
            int                                        read_size = 0;

            template <typename... Args>
            [[nodiscard]] constexpr bool operator()(IOScheduler auto io, Args&&... args) noexcept {
                // todo: how we're supposed to call this in an async way?
                async::advance(io, task_state, stl::forward<Args>(args)...);
                if (read_size <= -1) [[unlikely]] {
                    set_error(task_state, read_size);
                    set_error(io, read_size);
                    return false;
                } else if (read_size == 0) {
                    set_done(task_state);
                    set_done(io);
                } else {
                    set_value(task_state, read_size);
                    set_value(io, file_descriptor, buffer_span{it, static_cast<stl::size_t>(read_size)});
                }
                return true;
            }
        };
    };


    inline constexpr void async_file_stats(IOScheduler auto io, int file_descriptor) noexcept {
        const auto stats = io.request_file_stats(file_descriptor);
        if (stats != 0) {
            set_error(io, stats);
        }
        set_value(io, stats);
    }


    constexpr auto read(int file_descriptor) noexcept {
        // todo: how to pass the buffer?
        return just(file_descriptor) >> async_file_stats >> let_value([](auto stats) {
                   return stats.size();
               }) >>
               async_until(async_read_some{});
    }

    /// read the file/... check by chunk
    constexpr auto read_chunked(int file_descriptor) noexcept {
        // todo: how to pass the buffer?
        return just(file_descriptor) >> async_file_stats >> let_value([](auto stats) {
                   return stats.size();
               }) >>
               async_until(async_read_some{});
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
