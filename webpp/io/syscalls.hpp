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

    /// Customization Point Object for syscalls
    inline constexpr struct syscall_tag {

        template <typename SyscallOperation, typename Sched, typename... Args>
        static constexpr bool is_supported = stl::tag_invocable<SyscallOperation, Sched&, Args...>;

        template <typename SyscallOperation, typename Sched, typename... Args>
        constexpr decltype(auto) operator()(SyscallOperation, Sched& sched, Args&&... args) const
          noexcept(stl::nothrow_tag_invocable<SyscallOperation, Sched&, Args...>) {

            // a nice error message
            static_assert(stl::tag_invocable<SyscallOperation, Sched&, Args...>,
                          "IO operation is not implemented.");

            return stl::tag_invoke(SyscallOperation{}, sched, stl::forward<Args>(args)...);
        }
    } syscall;

    struct syscall_open {};
    struct syscall_read {};


#undef impl_syscall

} // namespace webpp::io::inline syscall_operations

#endif // WEBPP_IO_SYSCALLS_HPP
