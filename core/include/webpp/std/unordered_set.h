// Created by moisrex on 4/23/20.

#ifndef WEBPP_UNORDERED_SET_H
#define WEBPP_UNORDERED_SET_H

#include "../traits/std_traits.h"

#include <unordered_set>

// Traits aware map:
namespace webpp::stl {

    template <typename Traits, typename Key, typename Hash = std::hash<Key>,
              typename KeyEqual  = std::equal_to<Key>,
              typename Allocator = typename Traits::template allocator<Key>>
    using unordered_set = ::std::unordered_set<Key, Hash, KeyEqual, Allocator>;

    template <typename Traits, typename Key, typename Hash = std::hash<Key>,
              typename KeyEqual  = std::equal_to<Key>,
              typename Allocator = typename Traits::template allocator<Key>>
    using unordered_multiset =
      ::std::unordered_multiset<Key, Hash, KeyEqual, Allocator>;

} // namespace webpp::stl

#endif // WEBPP_UNORDERED_SET_H
