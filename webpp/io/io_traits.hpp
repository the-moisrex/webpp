// Created by moisrex on 7/29/23.

#ifndef WEBPP_IO_TRAITS_HPP
#define WEBPP_IO_TRAITS_HPP

#if __has_include(<liburing.h>)
#    include "io_uring.hpp"
#else
#    error "No underlying I/O operation is supported on this platform."
#endif

namespace webpp::io {
#ifdef WEBPP_IO_URING_SUPPORT
    struct default_io_traits {
        template <typename ValueType = int>
        using iterator = io_uring_syscall_iterator<ValueType>;
    };
#endif

    // helpers
    namespace traits {
        template <IOTraits IOT, typename ValueType = int>
        using iterator = typename IOT::iterator<ValueType>;


        template <IOTraits IOT, typename ValueType = int>
        struct syscallify {
            using iterator = traits::iterator<IOT, ValueType>;

            [[nodiscard]] constexpr iterator begin() const noexcept {
                return {};
            }

            [[nodiscard]] constexpr iterator begin() const noexcept {
                return {};
            }
        };
    } // namespace traits

} // namespace webpp::io

#endif // WEBPP_IO_TRAITS_HPP
