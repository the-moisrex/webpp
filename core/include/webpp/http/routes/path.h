// Created by moisrex on 4/29/20.

#ifndef WEBPP_PATH_H
#define WEBPP_PATH_H

#include "route.h"

#include <type_traits>

namespace webpp::routes {

    /**
     * Operator Path:
     *
     * Features:
     *   - [ ]
     *
     * Examples:
     *  - opath()/"page"/integer("page_num")/"profile"/user_id("user_id")
     */
    template <typename PathType = void, typename NextPathType = void>
    struct path {

        static constexpr bool has_value      = !stl::is_void_v<PathType>;
        static constexpr bool has_next_value = !stl::is_void_v<NextPathType>;

        // false_type is just used for "void" type; we could have avoided this
        // with some semi-clever inheritance way, but that would cause more
        // code bloat that it would fix any problem
        using opath_type = stl::conditional_t<
          has_value, stl::remove_reference_t<stl::remove_cv_t<PathType>>,
          stl::false_type>;
        using next_opath_type = stl::conditional_t<
          has_next_value,
          stl::remove_reference_t<stl::remove_cv_t<NextPathType>>,
          stl::false_type>;

        opath_type      value;
        next_opath_type next_value;


        template <typename NNextPathType>
        constexpr auto operator/(NNextPathType&& next_opath) const noexcept {
            using next_opath_t = NNextPathType;
            using new_opath_t =
              path<path<opath_type, next_opath_type>, next_opath_t>;
            return new_opath_t(*this, stl::forward<NNextPathType>(next_opath));
        }

        // todo: use something else other than stl::string_view; can we use traits here?
        constexpr auto
        operator[](stl::string_view const& segment) const noexcept {
            if constexpr (stl::is_void_v<opath_type>) {
                throw stl::invalid_argument(
                  "The specified segment name does not exist in this opath.");
            } else if constexpr (!requires(opeath_type op) {
                                     {op.variable_name};
                                 }) { // if it doesn't have a variable name
                if constexpr (stl::is_void_v<next_opath_type>) {
                    throw stl::invalid_argument(
                      "The specified segment name does not exists in this opath");
                } else {
                    // this segment doesn't have a variable name
                    // check the next segment:
                    return next_opath_type::template operator[]<Type>(segment);
                }
            } else { // this type has a variable name, so we check it
                if (opeath_type::variable_name == segment) {
                    return value;
                }
            }
        }


        [[nodiscard]] auto operator(Context auto& ctx) noexcept {
        }
    };



} // namespace webpp::routes


#endif // WEBPP_PATH_H
