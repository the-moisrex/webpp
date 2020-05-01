// Created by moisrex on 4/29/20.

#ifndef WEBPP_OPATH_H
#define WEBPP_OPATH_H

#include "route.h"

#include <type_traits>

namespace webpp::valves {

    /**
     * Operator Path:
     *
     * Features:
     *   - [ ]
     *
     * Examples:
     *  - opath::get()/"page"/integer("page_num")/"profile"/user_id("user_id")
     */


    template <typename A, typename = void>
    struct has_variable_name : std::false_type {};

    template <typename A>
    struct has_variable_name<A,
                             std::void_t<decltype(A::variable_name, (void)0)>>
      : std::true_type {};


    template <typename Traits, typename OPathType, typename NextOPathType>
    struct basic_opath : public OPathType {

        using next_opath_type =
          std::remove_reference_t<std::remove_cv_t<NextOPathType>>;

        next_opath_type next;

        constexpr basic_opath(OPathType&&       super,
                              next_opath_type&& _next) noexcept
          : OPathType(std::move(super)),
            next(std::move(_next)) {
        }

        constexpr basic_opath(OPathType const&       super,
                              const next_opath_type& _next) noexcept
          : OPathType(super),
            next(_next) {
        }

        constexpr basic_opath(OPathType const&  super,
                              next_opath_type&& _next) noexcept
          : OPathType(super),
            next(std::move(_next)) {
        }

        constexpr basic_opath(OPathType&&            super,
                              next_opath_type const& _next) noexcept
          : OPathType(std::move(super)),
            next(_next) {
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
    template <typename Traits, typename OPathType>
    struct basic_opath<Traits, OPathType, void> : public OPathType {
        using OPathType::OPathType;
    };


    template <typename Traits, typename OPathType,
              typename NextOPathType = void>
    struct opath_condition : basic_opath<Traits, OPathType, NextOPathType> {
        static_assert(
          is_traits_v<Traits>,
          "The specified template parameter is not a valid traits type.");

        using opath_type = std::remove_reference_t<std::remove_cv_t<OPathType>>;
        using next_opath_type =
          std::remove_reference_t<std::remove_cv_t<NextOPathType>>;
        using traits     = Traits;
        using super      = basic_opath<Traits, OPathType, NextOPathType>;
        using str_view_t = typename traits::string_view_type;


        // using parent constructors
        using super::super;



        template <typename NNextOPathType>
        constexpr auto operator/(NNextOPathType&& next_opath) const noexcept {
            using next_opath_t = NNextOPathType;
            using new_opath_t =
              opath_condition<Traits,
                              opath_condition<opath_type, next_opath_type>,
                              next_opath_t>;
            return new_opath_t(*this, std::forward<NNextOPathType>(next_opath));
        }



        template <typename Type>
        constexpr auto operator[](str_view_t const& segment) const noexcept {
            if constexpr (std::is_void_v<opath_type>) {
                throw std::invalid_argument(
                  "The specified segment name does not exist in this opath.");
            } else if constexpr (!has_variable_name<opath_type>::value) {
                if constexpr (std::is_void_v<next_opath_type>) {
                    throw std::invalid_argument(
                      "The specified segment name does not exists in this opath");
                } else {
                    // this segment doesn't have a variable name
                    // check the next segment:
                    return next_opath_type::template operator[]<Type>(segment);
                }
            } else {
            }
        }
    };


    template <typename Traits>
    struct opath : public valve<opath_condition<Traits, void, void>> {
        static_assert(
          is_traits_v<Traits>,
          "The specified template parameter is not a valid traits type.");
        using traits = Traits;

        using valve<opath_condition<traits, void, void>>::valve;
    };


} // namespace webpp::valves


#endif // WEBPP_OPATH_H
