// Created by moisrex on 4/29/20.

#ifndef WEBPP_PATH_H
#define WEBPP_PATH_H

#include "../../utils/fixed_string.h"
#include "route.h"

#include <type_traits>

namespace webpp::routes {

    /**
     * This class is used as a field type in the context type of the
     * internal sub routes of the "path" sub route.
     */
    template <Traits TraitsType, typename PathType, typename UriSegmentsType>
    struct path_field {
        using traits_type            = TraitsType;
        using string_view_type       = typename traits_type::string_view_type;
        using path_type              = PathType;
        using segments_type          = UriSegmentsType;
        using segments_iterator_type = typename segments_type::iterator;

        /**
         * I know I'm not using a raw pointer here; it's okay because it's a
         * pointer to an object on stack. Its lifetime is guaranteed to be
         * longer than this object.
         */
        path_type const* the_path = nullptr;

        // todo: also give access to the segments the user specified

        segments_type          segments{};
        segments_iterator_type current_segment{};

      private:
        friend path_type;

        bool next_segment() noexcept {
            // todo: should this method be private?
            return ++current_segment != segments.end();
        }

      public:
        template <fixed_string segment_variable_name>
        [[nodiscard]] constexpr auto const& segment() const noexcept {
            // using seg_type = ;
        }

        template <typename T>
        constexpr T const&
        segment(string_view_type const& segment_variable_name) const noexcept {

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
                      segment_variable_name);
                }
            } else { // this type has a variable name, so we check it
                if (segment_type::variable_name == segment_variable_name) {
                    return segment_variable_name;
                }
            }
        }
    }

    /**
     * This context extension will be used in the "path" so the user will be
     * use something like this:
     *   ctx.segments["page_number"]
     */
    template <typename PathType, typename UriSegmentsType>
    struct path_context_extension {

        template <Traits TraitsType>
        struct path_extension {
            path_field<TraitsType, PathType, UriSegmentsType> path;
        };

        template <Traits TraitsType>
        using context_extensions = extension_pack<path_extension>;
    };

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

        using path_type = path<SegmentType, NextSegmentType>;
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

        [[nodiscard]] auto operator(Context auto& ctx) noexcept {
            // handle inside-sub-route internal segment is done in this method

            using context_type     = decltype(ctx);
            using traits_type      = typename context_type::traits_type;
            using string_view_type = typename traits_type::string_view_type;
            constexpr bool has_path_extension = requires {
                { ctx.path }
                ->stl::same_as<path_field>;
            };

            // run the user's codes; hopefully this will optimized away
            constexpr auto run = [&](auto&& ctx) {
                if constexpr (has_segment) {
                    using result_type =
                      stl::invoke_result_t<segment, decltype(ctx)>;

                    // if the result of this segment is void
                    if constexpr (stl::is_void_v<result_type>) {
                        segment(ctx);
                        if constexpr (has_next_segment) {
                            return next_segment(ctx);
                        }
                        return void;
                    }

                    // if the result of calling this segment is NOT void
                    auto res = segment(ctx);
                    if constexpr (stl::is_same_v<result_type, bool>) {
                        // don't check the rest of the segments if it's not a
                        // match for the current segment
                        if (!res)
                            return false;
                    }
                    if constexpr (has_next_segment) {
                        if constexpr (Response<result_type>) {
                            return res;
                        } else if constexpr (Context<result_type>) {
                            // context switching is happening
                            return next_segment(stl::move(res));
                        }
                        return next_segment(ctx);
                    }

                    // return the results of the this segment because it's the
                    // last segment
                    return res;
                }

                return void; // explicitly saying that we have no return type
            };

            if constexpr (!has_path_extension) {

                // parse the uri
                // todo: we can optimize this, right? it parses the whole uri, do we need the whole uri? I think yes
                // fixme: should we decode it? if we decode it we need to care about the UTF-8 stuff as well?
                auto segments =
                  basic_uri<traits_type, false>{ctx.request.request_uri()}
                    .path_structured();
                using uri_segments_type = decltype(segments);

                // context switching
                auto new_ctx = ctx.clone<
                  path_context_extension<path_type, uri_segments_type>>();

                new_ctx.path.segments = stl::move(segments);

                // nothing to do if the segment counts don't match
                if (!new_ctx.path.next_segment())
                    return false;

                return run(stl::move(new_ctx));
            } else {

                // nothing to do if we the user's requesting a segment that we
                // don't have
                if (!ctx.path.next_segment())
                    return false;
                return run(stl::forward<context_type>(ctx));
            }
        }
    };



} // namespace webpp::routes

#endif // WEBPP_PATH_H
