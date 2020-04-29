// Created by moisrex on 4/23/20.

#ifndef WEBPP_SET_H
#define WEBPP_SET_H

#include "../traits/std_traits.h"

#include <set>

// Traits aware map:
namespace webpp::stl {
    template <typename Traits, typename Key,
              typename Compare   = ::std::less<Key>,
              typename Allocator = typename Traits::template allocator<Key>>
    using set = ::std::set<Key, Compare, Allocator>;

    template <typename Traits, typename Key,
              typename Compare   = ::std::less<Key>,
              typename Allocator = typename Traits::template allocator<Key>>
    using multiset = ::std::multiset<Key, Compare, Allocator>;

} // namespace webpp::stl

#endif // WEBPP_SET_H
