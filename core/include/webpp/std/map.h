// Created by moisrex on 4/23/20.

#ifndef WEBPP_MAP_H
#define WEBPP_MAP_H

#include "../utils/traits.h"

#include <map>

// Traits aware map:
namespace webpp::stl {
    template <typename Traits, typename Key, typename T,
              typename Compare = ::std::less<Key>,
              typename Allocator =
                typename Traits::template allocator<::std::pair<const Key, T>>>
    using map = ::std::map<Key, T, Compare, Allocator>;

    template <typename Traits, typename Key, typename T,
              typename Compare = ::std::less<Key>,
              typename Allocator =
                typename Traits::template allocator<::std::pair<const Key, T>>>
    using multimap = ::std::multimap<Key, T, Compare, Allocator>;

} // namespace webpp::stl

#endif // WEBPP_MAP_H
