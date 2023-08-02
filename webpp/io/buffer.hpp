// Created by moisrex on 8/2/23.

#ifndef WEBPP_IO_BUFFER_HPP
#define WEBPP_IO_BUFFER_HPP

#include "../std/span.hpp"

#if __has_include(<sys/uio.h>)
#    include <sys/uio.h>
#else
struct iovec {
    void*  iov_base; /* Starting address */
    size_t iov_len;  /* Size of the memory pointed to by iov_base. */
};
#endif


namespace webpp::io {

    template <typename Allocator = stl::allocator<char>>
    struct buffer {
        using allocator_type = Allocator;
        using alloc_traits   = stl::allocator_traits<allocator_type>;

        buffer(buffer const&)                          = delete;
        buffer& operator=(buffer const&)               = delete;
        constexpr buffer(buffer&&) noexcept            = default;
        constexpr buffer& operator=(buffer&&) noexcept = default;

        constexpr buffer(allocator_type const& inp_alloc, void* base, stl::size_t len) noexcept
          : allocator{inp_alloc},
            iov{base, len} {}
        constexpr buffer(allocator_type const& inp_alloc, char* base, stl::size_t len) noexcept
          : allocator{inp_alloc},
            iov{static_cast<void*>(base), len} {}

        buffer(allocator_type const& inp_alloc, stl::size_t len)
          : allocator{inp_alloc},
            iov{.iov_base = alloc_traits::allocate(allocator, iov.iov_len), .iov_len = len} {}

        ~buffer() {
            alloc_traits::deallocate(allocator, iov.iov_base, iov.iov_len);
        }

        operator iovec() const noexcept {
            return iov;
        }

        [[nodiscard]] constexpr iovec as_iovec() const noexcept {
            return iov;
        }

        [[nodiscard]] constexpr allocator_type const& get_allocator() const noexcept {
            return allocator;
        }

      private:
        [[no_unique_address]] allocator_type allocator;
        iovec                                iov{nullptr, 0};
    };

    template <typename Allocator = stl::allocator<buffer<>>>
    struct buffer_manager {
        using allocator_type = Allocator;
        using buffer_allocator_type =
          typename stl::allocator_traits<allocator_type>::template rebind_alloc<char>;
        using buffer_type = buffer<buffer_allocator_type>;

      private:
        stl::vector<buffer_type, allocator_type> buffers;
    };

} // namespace webpp::io

#endif // WEBPP_IO_BUFFER_HPP
