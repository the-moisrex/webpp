// Created by moisrex on 4/23/20.

#ifndef WEBPP_UNORDERED_MAP_H
#define WEBPP_UNORDERED_MAP_H

#include "../traits/traits_concepts.h"
#include "./std.h"

#include <unordered_map>

// Traits aware map:
namespace webpp::stl {

    template <
      Traits TraitsType, typename Key, typename T,
      typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>,
      typename Allocator =
        typename TraitsType::template allocator<std::pair<const Key, T>>>
    using unordered_map = std::unordered_map<Key, T, Hash, KeyEqual, Allocator>;

    template <
      Traits TraitsType, typename Key, typename T,
      typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>,
      typename Allocator =
        typename TraitsType::template allocator<std::pair<const Key, T>>>
    using unordered_multimap =
      std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator>;

} // namespace webpp::stl

#endif // WEBPP_UNORDERED_MAP_H
