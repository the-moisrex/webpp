// Created by moisrex on 4/23/20.

#ifndef WEBPP_SET_H
#define WEBPP_SET_H

#include "../traits/std_traits.hpp"
#include "./std.hpp"

#include <set>

// Traits aware map:
namespace webpp::istl {
    template <typename Traits, typename Key, typename Compare = stl::less<Key>,
              typename Allocator = typename Traits::template allocator<Key>>
    using set = stl::set<Key, Compare, Allocator>;

    template <typename Traits, typename Key, typename Compare = stl::less<Key>,
              typename Allocator = typename Traits::template allocator<Key>>
    using multiset = stl::multiset<Key, Compare, Allocator>;

} // namespace webpp::stl

#endif // WEBPP_SET_H
