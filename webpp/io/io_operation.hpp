// Created by moisrex on 7/4/23.

#ifndef WEBPP_IO_OPERATION_HPP
#define WEBPP_IO_OPERATION_HPP

#include "../std/type_traits.hpp"

namespace webpp::io::inline syscall {

    template <typename T>
    concept AsyncSyscallOperation = requires(T op, stl::true_type lambda) {
                                        op.then(lambda);
                                        { op | lambda };
                                    };

    template <typename CurrentOp>
    struct syscall_operation {
        using current_operation = CurrentOp;

        // Schedule a new operation after the current one is done
        template <AsyncSyscallOperation NewOp>
        constexpr auto then(NewOp&& new_op) const noexcept {
            // todo
        }

        template <AsyncSyscallOperation NewOp>
        consteval auto operator|(NewOp&& new_op) const noexcept {
            return then(stl::forward<NewOp>(new_op));
        }

        auto operator co_await() {
            // todo
        }
    };

} // namespace webpp::io::inline syscall

#endif // WEBPP_IO_OPERATION_HPP
