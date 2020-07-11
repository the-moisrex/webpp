// Created by moisrex on 4/23/20.

#ifndef WEBPP_MAP_H
#define WEBPP_MAP_H

#include "../traits/std_traits.hpp"
#include "./std.hpp"

#include <map>

// Traits aware map:
namespace webpp::istl {
    template <typename Traits, typename Key, typename T,
              typename Compare = stl::less<Key>,
              typename Allocator =
                typename Traits::template allocator<stl::pair<const Key, T>>>
    using map = stl::map<Key, T, Compare, Allocator>;

    template <typename Traits, typename Key, typename T,
              typename Compare = stl::less<Key>,
              typename Allocator =
                typename Traits::template allocator<stl::pair<const Key, T>>>
    using multimap = stl::multimap<Key, T, Compare, Allocator>;

} // namespace webpp::stl

#endif // WEBPP_MAP_H
