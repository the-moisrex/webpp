// Created by moisrex on 4/23/20.

#ifndef WEBPP_VECTOR_H
#define WEBPP_VECTOR_H

#include "../traits/traits.hpp"
#include "./std.hpp"

#include <vector>

// Traits aware vector:
namespace webpp::istl {

    template <Traits TraitsType, typename T,
              typename Allocator = typename TraitsType::template allocator<T>>
    using vector = stl::vector<T, Allocator>;

} // namespace webpp::stl

#endif // WEBPP_VECTOR_H
