// Created by moisrex on 12/16/20.

#ifndef WEBPP_BUFFER_HPP
#define WEBPP_BUFFER_HPP

#include "../std/std.hpp"

#include <array>

namespace webpp {

    template <stl::size_t max_size = 1024, typename BuffType = char>
    struct stack : public stl::array<BuffType, max_size> {
        using array_type = stl::array<BuffType, max_size>;
    };


} // namespace webpp

#endif // WEBPP_BUFFER_HPP
