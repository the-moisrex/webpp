// Created by moisrex on 4/23/20.

#ifndef WEBPP_MAP_H
#define WEBPP_MAP_H

#include "../utils/traits.h"

#include <vector>

// Traits aware map:
namespace webpp::stl {

    template <typename Traits, typename T,
              typename Allocator = typename Traits::template allocator<T>>
    using vector = ::std::vector<T, Allocator>;

} // namespace webpp::stl

#endif // WEBPP_MAP_H
