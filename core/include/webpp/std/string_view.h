// Created by moisrex on 4/23/20.

#ifndef WEBPP_STRING_VIEW_H
#define WEBPP_STRING_VIEW_H

#include "../utils/traits.h"

#include <string_view>

// Traits aware string_view:
namespace webpp::stl {

    template <typename Traits     = webpp::std_traits,
              typename CharT      = typename Traits::char_type,
              typename CharTraits = typename Traits::char_traits>
    class basic_string_view
      : public ::std::basic_string_view<CharT, CharTraits> {};

} // namespace webpp::stl

#endif // WEBPP_STRING_VIEW_H
