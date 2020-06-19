// Created by moisrex on 4/29/20.

#ifndef WEBPP_OPATH_H
#define WEBPP_OPATH_H

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
    template <typename Traits, typename OPathType, typename NextOPathType>
    struct basic_opath {

        using next_opath_type =
          stl::remove_reference_t<stl::remove_cv_t<NextOPathType>>;

        next_opath_type next_value;

        constexpr basic_opath(OPathType&&       super,
                              next_opath_type&& _next) noexcept
          : OPathType(stl::move(super)),
            next_value(stl::move(_next)) {
        }

        constexpr basic_opath(OPathType const&       super,
                              const next_opath_type& _next) noexcept
          : OPathType(super),
            next_value(_next) {
        }

        constexpr basic_opath(OPathType const&  super,
                              next_opath_type&& _next) noexcept
          : OPathType(super),
            next_value(stl::move(_next)) {
        }

        constexpr basic_opath(OPathType&&            super,
                              next_opath_type const& _next) noexcept
          : OPathType(stl::move(super)),
            next_value(_next) {
        }

        constexpr basic_opath(basic_opath const& v) noexcept = default;
        constexpr basic_opath(basic_opath&& v) noexcept      = default;

        constexpr basic_opath&
                               operator=(basic_opath const& v) noexcept = default;
        constexpr basic_opath& operator=(basic_opath&&) noexcept = default;
    };


    /**
     * If it's the last opath node
     */
    template <>
    struct basic_opath<void> {};


    template <Traits TraitsType, typename OPathType,
              typename NextOPathType = void>
    struct opath_condition : basic_opath<TraitsType, OPathType, NextOPathType> {

        using opath_type = stl::remove_reference_t<stl::remove_cv_t<OPathType>>;
        using next_opath_type =
          stl::remove_reference_t<stl::remove_cv_t<NextOPathType>>;
        using traits_type = TraitsType;
        using super       = basic_opath<TraitsType, OPathType, NextOPathType>;
        using str_view_t  = typename traits_type::string_view_type;

        opath_type value

          // using parent constructors
          using super::super;



        template <typename NNextOPathType>
        constexpr auto operator/(NNextOPathType&& next_opath) const noexcept {
            using next_opath_t = NNextOPathType;
            using new_opath_t =
              opath_condition<TraitsType,
                              opath_condition<opath_type, next_opath_type>,
                              next_opath_t>;
            return new_opath_t(*this, stl::forward<NNextOPathType>(next_opath));
        }



        constexpr auto operator[](str_view_t const& segment) const noexcept {
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
    };


    struct opath {

        template <Traits TraitsType>
        struct route_type
          : public route<opath_condition<TraitsType, void, void>> {
            using route<opath_condition<traits_type, void, void>>::route;
        };
    };


} // namespace webpp::routes


#endif // WEBPP_OPATH_H
