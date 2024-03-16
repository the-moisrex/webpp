// Created by moisrex on 8/2/23.

#ifndef WEBPP_IO_BUFFER_HPP
#define WEBPP_IO_BUFFER_HPP

#include "../std/span.hpp"
#include "../std/tag_invoke.hpp"
#include "../std/vector.hpp"

#if __has_include(<sys/uio.h>)
#    include <sys/uio.h>
#else
struct iovec {
    void*  iov_base; /* Starting address */
    size_t iov_len;  /* Size of the memory pointed to by iov_base. */
};
#endif




namespace webpp::io {

    using buffer_span = stl::span<stl::byte>;
    using buffer_view = stl::span<stl::byte const>;

    /**
     * @class buffer
     * @brief Represents a buffer with a specified length.
     *
     * The buffer class provides a way to store and manipulate a buffer of data.
     * It is implemented as a struct with member functions and data.
     */
    template <typename Allocator = stl::allocator<stl::byte>>
    struct buffer {
        using allocator_type = Allocator;
        using alloc_traits   = stl::allocator_traits<allocator_type>;
        using pointer        = typename alloc_traits::pointer;

        buffer(buffer const&)                          = delete;
        buffer& operator=(buffer const&)               = delete;
        constexpr buffer(buffer&&) noexcept            = default;
        constexpr buffer& operator=(buffer&&) noexcept = default;

        explicit buffer(stl::size_t len, allocator_type const& inp_alloc = {})
          : allocator{inp_alloc},
            iov{.iov_base = alloc_traits::allocate(allocator, iov.iov_len), .iov_len = len} {}

        ~buffer() {
            alloc_traits::deallocate(allocator, reinterpret_cast<pointer>(iov.iov_base), iov.iov_len);
        }

        [[nodiscard]] explicit operator iovec() const noexcept {
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
        using buffer_allocator_type =
          typename stl::allocator_traits<Allocator>::template rebind_alloc<stl::byte>;
        using buffer_type    = buffer<buffer_allocator_type>;
        using allocator_type = stl::allocator_traits<Allocator>::template rebind_alloc<buffer_type>;

        explicit constexpr buffer_manager(allocator_type const& inp_alloc = {}) noexcept
          : buffers{inp_alloc} {}

        buffer_type& new_buffer(stl::size_t len) {
            return buffers.emplace_back(len, buffers.get_allocator());
        }

        [[nodiscard]] explicit constexpr operator iovec*() const noexcept {
            return buffers.data();
        }

        [[nodiscard]] constexpr iovec* iovecs_ptr() const noexcept {
            return buffers.data();
        }

        [[nodiscard]] constexpr stl::size_t size() const noexcept {
            return buffers.size();
        }

      private:
        stl::vector<buffer_type, allocator_type> buffers;
    };

} // namespace webpp::io

#endif // WEBPP_IO_BUFFER_HPP
