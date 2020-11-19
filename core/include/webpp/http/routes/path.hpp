// Created by moisrex on 4/29/20.

#ifndef WEBPP_PATH_H
#define WEBPP_PATH_H

#include "../../std/optional.hpp"
#include "../../uri/uri_string.hpp"
#include "../../utils/fixed_string.hpp"
#include "route.hpp"

namespace webpp {

    //    namespace details {
    //        struct fake_appended_extension {
    //            struct path_type {
    //                const stl::string_view current_segment = "";
    //            } path{};
    //        };
    //        using fake_path_context_type = typename fake_context_type::template
    //        context_type_with_appended_extensions<fake_appended_extension>;
    //    }
    //
    template <typename T>
    concept PathContext = Context<T>&& requires(T ctx) {
        {ctx.path};
    };


    template <typename T>
    concept has_variable_name = requires(T seg) {
        { seg.variable_name }
        ->stl::convertible_to<stl::string_view>;
    };

    /**
     * Check if the specified segment can parse a uri segment into the
     * specified type T
     * @tparam SegType
     * @tparam T
     */
    template <typename ContextType, typename SegType, typename T>
    concept can_parse_to = requires(SegType seg, ContextType ctx) {
        { seg.template parse<T>(ctx) }
        ->stl::same_as<stl::optional<T>>;
    };

    /**
     * This class is used as a field type in the context type of the
     * internal sub routes of the "path" sub route.
     */
    template <Context ContextType, typename PathType, typename UriSegmentsType>
    struct path_field {
        using context_type           = ContextType;
        using traits_type            = typename context_type::traits_type;
        using string_view_type       = typename traits_type::string_view_type;
        using path_type              = PathType;
        using segments_type          = UriSegmentsType;
        using segments_iterator_type = typename segments_type::iterator;

        // todo: also give access to the segments the user specified

        segments_type          segments{};
        segments_iterator_type current_segment{};

        path_type* pth = nullptr;

        bool next_segment() noexcept {
            // todo: should this method be private?
            return ++current_segment != segments.end();
        }

        //        template <fixed_string segment_variable_name>
        //        [[nodiscard]] constexpr auto const& segment() const noexcept {
        //            // using seg_type = ;
        //        }

        auto const& segment(string_view_type const& segment_var_name) const noexcept {}

        template <typename T>
        T segment(string_view_type const& segment_variable_name) const noexcept {
            return pth->template get<T>(*this);
        }
    };

    /**
     * This context extension will be used in the "path"
     */
    template <typename PathType, typename UriSegmentsType>
    struct path_context_extension {

        struct path_extension {

            template <Traits TraitsType, typename ContextType>
            struct type : public ContextType {

                template <typename... Args>
                constexpr type(Args&&... args) noexcept : ContextType{stl::forward<Args>(args)...} {}

                path_field<ContextType, PathType, UriSegmentsType> path{};
            };
        };

        using context_extensions = extension_pack<path_extension>;
    };

    template <typename T, typename PathType, typename UriSegmentsType>
    concept Segment = requires(T seg) {
        // todo: update this
        seg.template operator()<typename fake_context_type::template context_type_with_appended_extensions<
          path_context_extension<PathType, UriSegmentsType>>>;
    };

    /**
     * Operator Path:
     *
     * Features:
     *   - [ ]
     *
     * Examples:
     *  - path()/"page"/integer("page_num")/"profile"/user_id("user_id")
     */
    template <typename SegmentType = void, typename NextSegmentType = void>
    struct path {

        static constexpr bool has_segment      = !stl::is_void_v<SegmentType>;
        static constexpr bool has_next_segment = !stl::is_void_v<NextSegmentType>;

        using path_type = path<SegmentType, NextSegmentType>;
        // false_type is just used for "void" type; we could have avoided this
        // with some semi-clever inheritance way, but that would cause more
        // code bloat that it would fix any problem
        using segment_type =
          stl::conditional_t<has_segment, stl::remove_cvref_t<SegmentType>, stl::false_type>;
        using next_segment_type =
          stl::conditional_t<has_next_segment, stl::remove_cvref_t<NextSegmentType>, stl::false_type>;

        // if the SegmentType is itself a path
        static constexpr bool is_segment_nested =
          has_segment &&
          stl::same_as<segment_type,
                       path<typename segment_type::segment_type, typename segment_type::next_segment_type>>;

        // if the NextSegmentType is itself a path (which normally should not happen)
        static constexpr bool is_next_segment_nested =
          has_next_segment &&
          stl::same_as<next_segment_type, path<typename next_segment_type::segment_type,
                                               typename next_segment_type::next_segment_type>>;

        segment_type      segment{};
        next_segment_type next_segment{};

      private:
        template <typename NextSegType>
        struct make_a_path {
            NextSegType new_next_segment;

            [[nodiscard]] bool operator()(PathContext auto const& ctx) const noexcept {
                if constexpr (requires { {new_next_segment == ""}; }) {
                    return new_next_segment == *ctx.path.current_segment;
                } else if constexpr (requires { {"" == new_next_segment}; }) {
                    return *ctx.path.current_segment == new_next_segment;
                } else {
                    return false; // should not happen
                }
            }
        };

      public:
        /**
         * Convert different original types to normal callable segments
         */
        template <typename NewSegType>
        constexpr auto operator/(NewSegType&& new_next_segment) const noexcept {
            using seg_type = stl::remove_cvref_t<NewSegType>;

            /*if constexpr (Segment<seg_type, path_type,
                                  decltype(basic_uri<fake_traits_type, false>{}.path_structured())>) {
            } else*/
            if constexpr (stl::is_array_v<seg_type> &&
                          stl::is_integral_v<stl::remove_all_extents_t<seg_type>>) {
                // int_type[N] => string_view
                using char_type = stl::remove_all_extents_t<seg_type>;
                return operator/
                  <stl::basic_string_view<char_type>>(stl::forward<NewSegType>(new_next_segment));
            } else if constexpr (stl::is_pointer_v<seg_type> &&
                                 stl::is_integral_v<stl::remove_pointer_t<seg_type>>) {
                // char* => string_view
                using char_type = stl::remove_pointer_t<seg_type>;
                return operator/
                  <stl::basic_string_view<char_type>>(stl::forward<NewSegType>(new_next_segment));
            } else if constexpr (stl::is_integral_v<seg_type>) {
                // integral types
                return operator/([=](PathContext auto const& ctx) constexpr noexcept->bool {
                    return to<seg_type>(ctx.path.current_segment) == new_next_segment;
                });
            } else if constexpr (istl::ComparableToString<seg_type> && stl::is_class_v<seg_type>) {

                // Convert those segments that can be compared with a string, to a normal segment
                // type that have an operator(context)


                //            return operator/([=](PathContext auto const& ctx) {
                //                if constexpr (requires { {next_segment == ""}; }) {
                //                    return new_next_segment == ctx.path.current_segment;
                //                } else if constexpr (requires { {"" == new_next_segment}; }) {
                //                    return ctx.path.current_segment == new_next_segment;
                //                } else {
                //                    return false; // should not happen
                //                }
                //            });
                return operator/
                  (make_a_path<seg_type>{.new_next_segment = stl::forward<NewSegType>(new_next_segment)});
            } else {
                // segment

                // Add a new segment to the path and get a new path containing that specific segment
                // and all the previous ones
                if constexpr (!has_segment) {
                    return route<path<seg_type, void>>{
                      path<seg_type, void>{.segment = stl::forward<NewSegType>(new_next_segment)}};
                } else if constexpr (!has_next_segment) {
                    return route<path<segment_type, seg_type>>{
                      path<segment_type, seg_type>{.segment = segment, .next_segment = new_next_segment}};
                } else {
                    using next_segment_t   = seg_type;
                    using new_segment_type = path<path<segment_type, next_segment_type>, next_segment_t>;
                    return route<new_segment_type>{
                      new_segment_type{.segment      = *this,
                                       .next_segment = stl::forward<NewSegType>(new_next_segment)}};
                }

                // todo: or give a compile time error if you can
                //                return operator/([=](Context auto const& ctx) constexpr noexcept {
                //                    return ctx.error(500u,
                //                                     "The specified segment is not a valid segment type;"
                //                                     " it should be callable or comparable to a string "
                //                                     "or a known type that we can make it callable.",
                //                                     new_next_segment);
                //                });
            }
        }

        /**
         * Get the number or segments in this path
         */
        [[nodiscard]] static constexpr stl::size_t size() noexcept {
            stl::size_t _size = 0;
            if constexpr (has_segment) {
                if constexpr (requires { {segment_type::size()}; }) {
                    _size += segment_type::size();
                } else {
                    ++_size;
                }
            }
            if constexpr (has_next_segment) {
                if constexpr (requires { {next_segment_type::size()}; }) {
                    _size += next_segment_type::size();
                } else {
                    ++_size;
                }
            }
            return _size;
        }

        /**
         * Get a segment (or something which that segment can be parsed into) based on the specified variable
         * name
         */
        template <Context ContextType, typename T>
        [[nodiscard]] stl::optional<T> get(ContextType const&      ctx,
                                           stl::string_view const& variable_name) const noexcept {
            if constexpr (has_segment) {
                if constexpr (is_segment_nested) {
                    if (auto res = segment.template get<T>(variable_name))
                        return res;
                } else if constexpr ((stl::is_convertible_v<T, segment_type> ||
                                      can_parse_to<ContextType, segment_type,
                                                   T>) &&has_variable_name<segment_type>) {
                    if (variable_name == segment.variable_name) {
                        if constexpr (stl::is_convertible_v<T, segment_type>) {
                            return segment;
                        } else if constexpr (can_parse_to<ContextType, segment_type, T>) {
                            return segment.template parse<T>(ctx);
                        }
                    }
                } else {
                    // we should return stl::nullopt but we avoid returning
                    // anything here because we need to check the
                    // next_segment_type as well.
                }
            }

            if constexpr (has_next_segment) {
                if constexpr (is_next_segment_nested) {
                    if (auto res = next_segment.template get<T>(variable_name))
                        return res;
                } else if constexpr ((stl::is_convertible_v<T, next_segment_type> ||
                                      can_parse_to<ContextType, next_segment_type,
                                                   T>) &&has_variable_name<next_segment_type>) {
                    if (variable_name == next_segment.variable_name) {
                        if constexpr (stl::is_convertible_v<T, next_segment_type>) {
                            return next_segment;
                        } else if constexpr (can_parse_to<ContextType, next_segment_type, T>) {
                            return next_segment.template parse<T>(ctx);
                        }
                    }
                }
            }

            return stl::nullopt;
        }

      private:
        /**
         * Call the segment and catch the exceptions if there are any
         */
        static inline bool call_and_catch(auto&& callable, auto&&... args) noexcept {
            if constexpr (stl::is_nothrow_invocable_v<decltype(callable), decltype(args)...>) {
                // It's noexcept, we call it knowing that.
                return callable(stl::forward<decltype(args)>(args)...);
            } else if constexpr (stl::is_invocable_v<decltype(callable), decltype(args)...>) {
                try {
                    return callable(stl::forward<decltype(args)>(args)...);
                } catch (...) { return false; }
            } else {
                return false;
            }
        }

        /**
         * call the specified segment with the right argument and handle the exceptions
         */
        [[nodiscard]] static inline bool call_segment(auto&& seg, auto&& ctx, auto const& req) noexcept {
            using context_type = decltype(ctx);
            using req_type     = decltype(req);
            using seg_type     = decltype(seg);
            if constexpr (stl::is_invocable_v<seg_type, context_type, req_type>) {
                return call_and_catch(seg, ctx, req);
            } else if constexpr (stl::is_invocable_v<seg_type, req_type, context_type>) {
                return call_and_catch(seg, req, ctx);
            } else if constexpr (stl::is_invocable_v<seg_type, context_type>) {
                return call_and_catch(seg, ctx);
            } else if constexpr (stl::is_invocable_v<seg_type>) {
                return call_and_catch(seg);
            } else {
                throw stl::invalid_argument("The specified segment in the path cannot be called.");
            }
        }

      public:
        template <typename ContextType>
        requires(Context<stl::remove_cvref_t<ContextType>>) [[nodiscard]] bool
        operator()(ContextType&& ctx, Request auto const& req) noexcept {
            // handle inside-sub-route internal segment is done in this method

            using context_type                = stl::remove_cvref_t<ContextType>;
            using context_ref_type            = stl::add_lvalue_reference_t<context_type>;
            using traits_type                 = typename context_type::traits_type;
            constexpr bool has_path_extension = requires {
                {ctx.path};
            };

            if constexpr (!has_segment) {
                return true;
            } else {

                // Switching the context if it doesn't have a path in it
                if constexpr (!has_path_extension) {
                    // Performing in-place context switching (meaning we switch the context for the current
                    // segment and not just the next segment)

                    // parse the uri
                    // todo: we can optimize this, right? it parses the whole uri, do we need the whole uri? I think yes
                    // fixme: should we decode it? if we decode it we need to care about the UTF-8 stuff as well?
                    // todo: move this parsing into the request so we don't have to do it more than once for one request
                    auto uri_segments = uri::uri_string<traits_type, false>{req.request_uri()}.path_structured();
                    using uri_segments_type = decltype(uri_segments);


                    // context switching
                    auto new_ctx = ctx.template clone<path_context_extension<path_type, uri_segments_type>>();
                    static_assert(
                      requires { {new_ctx.path}; },
                      "For some reason, we're not able to perform context switching.");

                    new_ctx.path.segments        = stl::move(uri_segments);
                    new_ctx.path.current_segment = new_ctx.path.segments.begin();
                    new_ctx.path.pth             = this;

                    // todo: (is there something we should do here?) nothing to do if the segment counts don't match

                    // re-running the this member function with the new switched context type
                    return operator()(stl::move(new_ctx), req);
                } else {

                    // nothing to do if we the user's requesting a segment that we don't have
                    if (!ctx.path.next_segment())
                        return false;

                    using result_type = stl::invoke_result_t<segment_type, context_ref_type>;

                    // if the result of this segment is void
                    if constexpr (stl::is_void_v<result_type>) {
                        call_segment(segment, ctx, req);
                        if constexpr (has_next_segment) {
                            return call_segment(next_segment, ctx, req);
                        } else {
                            return true;
                        }
                    } else {

                        // if the result of calling this segment is NOT void
                        auto res = call_segment(segment, ctx, req);
                        if constexpr (stl::is_same_v<result_type, bool>) {
                            // don't check the rest of the segments if it's not a
                            // match for the current segment
                            if (!res)
                                return false;
                        }
                        if constexpr (has_next_segment) {
                            return call_segment(next_segment, ctx, req);
                        } else {
                            // return the results of the this segment because it's the last segment
                            return res;
                        }
                    }
                }
            }
        }
    };

    // the root path
    constexpr auto root = path{} / "";

    // relative path
    constexpr path relative{};

} // namespace webpp

#endif // WEBPP_PATH_H
