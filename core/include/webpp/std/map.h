// Created by moisrex on 4/23/20.

#ifndef WEBPP_MAP_H
#define WEBPP_MAP_H

#include "std.h"

#if __has_include(<map>)
#    define STD_MAP STLLIB_STANDARD
#    include <map>
#elif __has_include(<experimental/map>)
#    define STD_MAP STLLIB_EXPERIMENTAL
#    include <experimental/map>
#elif __has_include(<boost/containers/map.hpp>)
#    define STD_MAP STLLIB_BOOST
#    include <boost/containers/map.hpp>
namespace std {
    using multimap = boost::multimap;
    using map      = boost::map;
} // namespace std
#endif


#include "../utils/traits.h"

// Traits aware map:
namespace webpp::stl {
    template <typename Traits, typename Key, typename T,
              typename Compare = ::std::less<Key>,
              typename Allocator =
                typename Traits::template allocator<::std::pair<const Key, T>>>
    class map : public ::std::map<Key, T, Compare, Allocator> {};

    template <typename Traits, typename Key, typename T,
              typename Compare = ::std::less<Key>,
              typename Allocator =
                typename Traits::template allocator<::std::pair<const Key, T>>>
    class multimap : public ::std::multimap<Key, T, Compare, Allocator> {};

} // namespace webpp::stl

#endif // WEBPP_MAP_H
