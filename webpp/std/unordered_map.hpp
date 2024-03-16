// Created by moisrex on 4/23/20.

#ifndef WEBPP_UNORDERED_MAP_H
#define WEBPP_UNORDERED_MAP_H

#include "../traits/traits.hpp"
#include "std.hpp"

#include <unordered_map>

// Traits aware map:
namespace webpp::istl {

    template <Traits TraitsType,
              typename Key,
              typename T,
              typename Hash      = stl::hash<Key>,
              typename KeyEqual  = stl::equal_to<Key>,
              typename Allocator = typename TraitsType::template allocator<stl::pair<Key const, T>>>
    using unordered_map = stl::unordered_map<Key, T, Hash, KeyEqual, Allocator>;

    template <Traits TraitsType,
              typename Key,
              typename T,
              typename Hash      = stl::hash<Key>,
              typename KeyEqual  = stl::equal_to<Key>,
              typename Allocator = typename TraitsType::template allocator<stl::pair<Key const, T>>>
    using unordered_multimap = stl::unordered_multimap<Key, T, Hash, KeyEqual, Allocator>;

} // namespace webpp::istl

#endif // WEBPP_UNORDERED_MAP_H
