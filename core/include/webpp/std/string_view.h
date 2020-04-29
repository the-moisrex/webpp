// Created by moisrex on 4/23/20.

#ifndef WEBPP_STRING_VIEW_H
#define WEBPP_STRING_VIEW_H

#include "../traits/std_traits.h"

#include <string_view>

// Traits aware string_view:
namespace webpp::stl {

    template <typename Traits     = webpp::std_traits,
              typename CharT      = typename Traits::char_type,
              typename CharTraits = typename Traits::char_traits>
    using basic_string_view = ::std::basic_string_view<CharT, CharTraits>;

} // namespace webpp::stl

#endif // WEBPP_STRING_VIEW_H
