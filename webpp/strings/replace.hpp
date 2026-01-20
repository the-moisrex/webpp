// Created by moisrex on 1/20/26.

#ifndef WEBPP_REPLACE_HPP
#define WEBPP_REPLACE_HPP

#include "../std/string.hpp"
#include "../std/string_view.hpp"

namespace webpp {

    template <typename CharT, typename AllocT>
    static constexpr stl::size_t replace_all(
      stl::basic_string<CharT, stl::char_traits<CharT>, AllocT>& inout,
      stl::basic_string_view<CharT>                              what,
      stl::basic_string_view<CharT>                              with) {
        using input_string_type = stl::basic_string<CharT, stl::char_traits<CharT>, AllocT>;
        stl::size_t count{};
        for (typename input_string_type::size_type pos{};; pos += with.size(), ++count) {
            pos = inout.find(what.data(), pos, what.size());
            if (pos == input_string_type::npos) {
                break;
            }
            inout.replace(pos, what.size(), with.data(), with.size());
        }
        return count;
    }

} // namespace webpp

#endif // WEBPP_REPLACE_HPP
