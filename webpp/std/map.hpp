// Created by moisrex on 4/23/20.

#ifndef WEBPP_STD_MAP_HPP
#define WEBPP_STD_MAP_HPP

#include "../memory/allocators.hpp"
#include "../traits/std_traits.hpp"
#include "std.hpp"

#include <map>

// Traits aware map:
namespace webpp::istl {
    template <typename Traits,
              typename Key,
              typename T,
              typename Compare   = stl::less<Key>,
              typename Allocator = traits::allocator_type_of<Traits, stl::pair<Key const, T>>>
    using map = stl::map<Key, T, Compare, Allocator>;

    template <typename Traits,
              typename Key,
              typename T,
              typename Compare   = stl::less<Key>,
              typename Allocator = traits::allocator_type_of<Traits, stl::pair<Key const, T>>>
    using multimap = stl::multimap<Key, T, Compare, Allocator>;


    // todo: use better thing here that supports the allocator pack idea
    template <typename StringType = stl::string, typename AllocType = typename StringType::allocator_type>
    using map_of_strings =
      stl::map<StringType,
               StringType,
               stl::less<StringType>,
               rebind_allocator<AllocType, stl::pair<stl::add_const_t<StringType>, StringType>>>;

} // namespace webpp::istl

#endif // WEBPP_STD_MAP_HPP
