// Created by moisrex on 4/23/20.

#ifndef WEBPP_MAP_H
#define WEBPP_MAP_H

#include "../utils/traits.h"

#include <unordered_map>

// Traits aware map:
namespace webpp::stl {

    template <typename Traits, typename Key, typename T,
              typename Hash     = std::hash<Key>,
              typename KeyEqual = std::equal_to<Key>,
              typename Allocator =
                typename Traits::template allocator<::std::pair<const Key, T>>>
    class unordered_map
      : public ::std::unordered_map<Key, T, Hash, KeyEqual, Allocator> {};

    template <typename Traits, typename Key, typename T,
              typename Hash     = std::hash<Key>,
              typename KeyEqual = std::equal_to<Key>,
              typename Allocator =
                typename Traits::template allocator<::std::pair<const Key, T>>>
    class unordered_multimap
      : public ::std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator> {};

} // namespace webpp::stl

#endif // WEBPP_MAP_H
