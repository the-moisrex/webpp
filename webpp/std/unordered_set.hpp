// Created by moisrex on 4/23/20.

#ifndef WEBPP_UNORDERED_SET_H
#define WEBPP_UNORDERED_SET_H

#include "../traits/traits.hpp"
#include "std.hpp"

#include <unordered_set>

// Traits aware map:
namespace webpp::istl {

    template <Traits TraitsType,
              typename Key,
              typename Hash      = stl::hash<Key>,
              typename KeyEqual  = stl::equal_to<Key>,
              typename Allocator = typename TraitsType::template allocator<Key>>
    using unordered_set = stl::unordered_set<Key, Hash, KeyEqual, Allocator>;

    template <Traits TraitsType,
              typename Key,
              typename Hash      = stl::hash<Key>,
              typename KeyEqual  = stl::equal_to<Key>,
              typename Allocator = typename TraitsType::template allocator<Key>>
    using unordered_multiset = stl::unordered_multiset<Key, Hash, KeyEqual, Allocator>;

} // namespace webpp::istl

#endif // WEBPP_UNORDERED_SET_H
