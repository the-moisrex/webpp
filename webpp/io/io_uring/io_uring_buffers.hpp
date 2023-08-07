// Created by moisrex on 8/2/23.

#ifndef WEBPP_IO_URING_BUFFERS_HPP
#define WEBPP_IO_URING_BUFFERS_HPP

#include "../buffer.hpp"

namespace webpp::io {

    template <typename Allocator = stl::allocator<stl::byte>>
    struct io_uring_buffer_manager {};

} // namespace webpp::io

#endif // WEBPP_IO_URING_BUFFERS_HPP
