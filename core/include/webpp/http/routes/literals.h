// Created by moisrex on 4/28/20.

#ifndef WEBPP_LITERALS_H
#define WEBPP_LITERALS_H

#include "../../traits/std_traits.h"

namespace webpp::inline literals {


    inline namespace std_traits {

        /*
        constexpr path<::webpp::std_traits>
        operator""_path(const char* str, std::size_t len) noexcept {
            return path<::webpp::std_traits>{
              typename ::webpp::std_traits::string_view_type{str, len}};
        }
         */

    } // namespace std_traits

} // namespace webpp::inline literals

#endif // WEBPP_LITERALS_H
