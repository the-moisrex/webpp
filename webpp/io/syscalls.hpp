// Created by moisrex on 8/7/23.

#ifndef WEBPP_IO_SYSCALLS_HPP
#define WEBPP_IO_SYSCALLS_HPP

#include "../std/tag_invoke.hpp"

/// This file/namespace defines all possible I/O system calls supported by
/// each "(a)sync I/O interfaces" like io_uring, ... .
///
/// Each IO interface will implement a syscall by providing a Customization Point.
/// Each "(a)sync algorithm" that needs to call these syscalls, they can call it like this:
/// @code
///   syscall(io_operation_tag, scheduler_ref, args...)
/// @endcode
/// io_operation_tag is the syscall tag (read, write, ...)
/// scheduler_ref is a reference to the "(a)sync I/O interface"'s scheduler
namespace webpp::io::inline syscall_operations {

    /// Customization Point Object for read
    inline constexpr struct syscall_tag {
        template <typename SyscallOperation, typename Sched, typename... Args>
        constexpr decltype(auto) operator()(SyscallOperation, Sched& sched, Args&&... args) const noexcept {
            return stl::tag_invoke(syscall_tag{}, SyscallOperation{}, sched, stl::forward<Args>(args)...);
        }
    } syscall;


    inline constexpr struct open_tag {
    } open;

    inline constexpr struct read_t {
        template <typename Sched, typename CallbackType>
        friend int tag_invoke(syscall_tag,
                              read_t,
                              Sched&         sched,
                              char*          buf,
                              stl::size_t    amount,
                              CallbackType&& callback) noexcept {
            if constexpr (requires {
                              {
                                  sched.read(buf, amount, stl::forward<CallbackType>(callback))
                              } noexcept -> stl::same_as<int>;
                          }) {
                return sched.read(buf, amount, stl::forward<CallbackType>(callback));
            } else {
                static_assert_false(Sched, "IO operation is not implemented.");
            }
            return 0;
        }
    } read;
} // namespace webpp::io::inline syscall_operations

#endif // WEBPP_IO_SYSCALLS_HPP
