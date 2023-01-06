// Created by moisrex on 4/23/20.

#ifndef WEBPP_VECTOR_H
#define WEBPP_VECTOR_H

#include "../traits/default_traits.hpp"
#include "std.hpp"

#include <vector>

// Traits aware vector:
namespace webpp::istl {

    template <typename T,
              Traits TraitsType  = default_traits,
              typename Allocator = traits::general_allocator<TraitsType, T>>
    using vector = stl::vector<T, Allocator>;

    template <typename T,
              Traits TraitsType  = default_traits,
              typename Allocator = traits::local_allocator<TraitsType, T>>
    using local_vector = stl::vector<T, Allocator>;

} // namespace webpp::istl

#endif // WEBPP_VECTOR_H
