// Created by moisrex on 12/16/20.

#ifndef WEBPP_MEMORY_STACK_HPP
#define WEBPP_MEMORY_STACK_HPP

#include "../std/std.hpp"

#include <array>

namespace webpp {

    template <stl::size_t max_size = 1024, typename BuffType = char> // NOLINT(*-magic-numbers)
    struct stack : public stl::array<BuffType, max_size> {
        using array_type = stl::array<BuffType, max_size>;
    };


} // namespace webpp

#endif // WEBPP_MEMORY_STACK_HPP
