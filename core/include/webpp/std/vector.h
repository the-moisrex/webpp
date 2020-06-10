// Created by moisrex on 4/23/20.

#ifndef WEBPP_VECTOR_H
#define WEBPP_VECTOR_H

#include "../traits/traits_concepts.h"
#include "./std.h"

#include <vector>

// Traits aware vector:
namespace webpp::stl {

    template <Traits TraitsType, typename T,
              typename Allocator = typename TraitsType::template allocator<T>>
    using vector = stl::vector<T, Allocator>;

} // namespace webpp::stl

#endif // WEBPP_VECTOR_H
