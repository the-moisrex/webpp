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
    template <typename SegmentType = void, typename NextSegmentType = void>
    struct path {

        static constexpr bool has_segment = !stl::is_void_v<SegmentType>;
        static constexpr bool has_next_segment =
          !stl::is_void_v<NextSegmentType>;

        // false_type is just used for "void" type; we could have avoided this
        // with some semi-clever inheritance way, but that would cause more
        // code bloat that it would fix any problem
        using segment_type = stl::conditional_t<
          has_segment, stl::remove_reference_t<stl::remove_cv_t<PathType>>,
          stl::false_type>;
        using next_segment_type = stl::conditional_t<
          has_next_segment,
          stl::remove_reference_t<stl::remove_cv_t<NextPathType>>,
          stl::false_type>;

        segment_type      segment;
        next_segment_type next_segment;


        template <typename NewNextSegmentType>
        constexpr auto
        operator/(NewNextSegmentType&& next_segment) const noexcept {
            using next_segment_t = NewNextSegmentType;
            using new_segment_type =
              path<path<segment_type, next_segment_type>, next_segment_t>;
            return new_segment_type(
              *this, stl::forward<NewNextSegmentType>(next_segment));
        }

        // todo: use something else other than stl::string_view; can we use traits here?
        constexpr auto
        operator[](stl::string_view const& segment) const noexcept {

            // if it doesn't have a variable name
            constexpr bool has_segment_variable_name =
              requires(segment_type op) {
                {op.variable_name};
            };

            // if segment is not void
            if constexpr (!has_segment) {
                // should we:
                // throw error, (kill the program)
                // remove the noexcept, (so the user can handle it)
                // or just avoid the error and return something useless?
                throw stl::invalid_argument(
                  "The specified segment name does not exist in this opath.");
            }

            if constexpr (!has_segment_variable_name) {
                if constexpr (stl::is_void_v<next_segment_type>) {
                    throw stl::invalid_argument(
                      "The specified segment name does not exists in this opath");
                } else {
                    // this segment doesn't have a variable name
                    // check the next segment:
                    return next_segment_type::template operator[]<Type>(
                      segment);
                }
            } else { // this type has a variable name, so we check it
                if (segment_type::variable_name == segment) {
                    return segment;
                }
            }
        }


        [[nodiscard]] auto operator(Context auto& ctx) noexcept {
            // handle inside-sub-route internal segment:

            // context switching
            auto new_ctx = ctx.clone<path_extension>();
            if constexpr (has_segment) {
                segment(new_ctx); // todo: how to handle the results?
            }
            if constexpr (has_next_segment) {
                next_segment(
                  new_ctx); // todo: how to get the segment's result to this guy?
            }

            // todo: what to return here?
        }
    };



} // namespace webpp::routes


#endif // WEBPP_PATH_H
