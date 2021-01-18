// Created by moisrex on 4/28/20.

#ifndef WEBPP_STD_TRAITS_H
#define WEBPP_STD_TRAITS_H

#include "../logs/std_logger.hpp"
#include "../memory/std_allocator_pack.hpp"

#include <string>
#include <string_view>

namespace webpp {

    template <typename CharT,
              typename CharTraits        = stl::char_traits<CharT>,
              typename AllocatorDescType = stl::allocator_descriptors>
    struct basic_std_traits {
        using traits_type           = basic_std_traits<CharT, CharTraits, AllocatorDescType>;
        using char_type             = CharT;
        using char_traits           = CharTraits;
        using logger_type           = stderr_logger;
        using allocator_descriptors = AllocatorDescType;
        using string_view           = stl::basic_string_view<char_type, char_traits>;

        template <typename AllocT>
        using string = stl::basic_string<char_type, char_traits, AllocT>;
    };

    using std_traits    = basic_std_traits<char>;
    using std_wtraits   = basic_std_traits<wchar_t>;
    using std_u8traits  = basic_std_traits<char8_t>;
    using std_u16traits = basic_std_traits<char16_t>;
    using std_u32traits = basic_std_traits<char32_t>;

} // namespace webpp

#endif // WEBPP_STD_TRAITS_H
