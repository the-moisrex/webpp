// Created by moisrex on 4/22/20.
#ifndef WEBPP_TRAITS_H
#define WEBPP_TRAITS_H

#include "../std/concepts.hpp"

namespace webpp {


    template <typename From, typename To>
    using to_alloc = typename stl::allocator_traits<From>::template rebind_alloc<To>;

    //    /**
    //     * todo: Use this, or remove this
    //     */
    //    template <typename FromTraits, typename ToTraits>
    //    struct traits_convert {
    //        using ft = FromTraits;
    //        using tt = ToTraits;
    //
    //        using ft_char_type = typename ft::char_type;
    //        using tt_char_type = typename tt::char_type;
    //
    //        using ft_string_view_type = typename ft::string_view_type;
    //        using tt_string_view_type = typename tt::string_view_type;
    //
    //        using ft_string_type = typename ft::string_type;
    //        using tt_string_type = typename tt::string_type;
    //
    //        /**
    //         * Converting the char_type
    //         */
    //        static constexpr auto to_char_type(ft_char_type ch) noexcept {
    //            return static_cast<tt_char_type>(ch);
    //        }
    //
    //        //        static constexpr auto to_string_view(ft_string_view_type const
    //        //        &sv) noexcept {
    //        //            // todo: very bad thing:
    //        //            return tt_string_view_type{sv.data(), sv.size()};
    //        //        }
    //        //
    //        static constexpr auto to_string(ft_string_type const& str) noexcept {
    //            tt_string_type res;
    //            res.resize(str.size() + 1);
    //            auto it = res.begin();
    //            for (auto c : str)
    //                *it++ = to_char_type(c);
    //            *it = to_char_type('\0');
    //            return res;
    //        }
    //    };

} // namespace webpp

#endif // WEBPP_TRAITS_H
