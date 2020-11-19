// Created by moisrex on 4/23/20.

#ifndef WEBPP_MAP_H
#define WEBPP_MAP_H

#include "../traits/std_traits.hpp"
#include "../utils/allocators.hpp"
#include "./std.hpp"

#include <map>

// Traits aware map:
namespace webpp::istl {
    template <typename Traits, typename Key, typename T, typename Compare = stl::less<Key>,
              typename Allocator = typename Traits::template allocator<stl::pair<const Key, T>>>
    using map = stl::map<Key, T, Compare, Allocator>;

    template <typename Traits, typename Key, typename T, typename Compare = stl::less<Key>,
              typename Allocator = typename Traits::template allocator<stl::pair<const Key, T>>>
    using multimap = stl::multimap<Key, T, Compare, Allocator>;


    template <typename StringType = stl::string,
              typename AllocType  = typename stl::remove_cvref_t<StringType>::allocator_type>
    using map_of_strings =
      stl::map<stl::remove_cvref_t<StringType>, stl::remove_cvref_t<StringType>,
               stl::less<stl::remove_cvref_t<StringType>>,
               rebind_allocator<AllocType, stl::pair<stl::add_const_t<stl::remove_cvref_t<StringType>>,
                                                     stl::remove_cvref_t<StringType>>>>;

} // namespace webpp::istl

#endif // WEBPP_MAP_H
