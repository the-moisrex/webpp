// Created by moisrex on 4/23/20.

#ifndef WEBPP_VECTOR_H
#define WEBPP_VECTOR_H

#include "../traits/std_traits.h"

#include <vector>

// Traits aware vector:
namespace webpp::stl {

    template <typename Traits, typename T,
              typename Allocator = typename Traits::template allocator<T>>
    using vector = ::std::vector<T, Allocator>;

} // namespace webpp::stl

#endif // WEBPP_VECTOR_H
