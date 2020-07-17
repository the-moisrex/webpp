// Created by moisrex on 4/29/20.

#ifndef WEBPP_PATH_H
#define WEBPP_PATH_H

#include "../../std/optional.hpp"
#include "../../utils/fixed_string.hpp"
#include "../../utils/uri.hpp"
#include "route.hpp"

#include <type_traits>

namespace webpp::routes {

    struct fake_path_context_type : fake_context_type {
        struct path_type {
            const stl::string_view current_segment = "";
        } path{};
    };

    template <typename T>
    concept Segment = requires(T seg, fake_path_context_type ctx) {
        {seg(ctx)};
    };

    template <typename T>
    concept ComparableToString = requires(T obj) {
        {obj == ""};
    }
    || requires(T obj) {
        {"" == obj};
    };

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

        auto const& segment(string_view_type const& segment_var_name) const noexcept {
        }

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

        template <Traits TraitsType, typename ContextType>
        struct path_extension : public virtual ContextType {

            template <typename... Args>
            constexpr path_extension(Args&&... args) noexcept : ContextType{stl::forward<Args>(args)...} {
            }


            path_field<ContextType, PathType, UriSegmentsType> path{};
        };

        template <Traits TraitsType, typename ContextType>
        using context_extensions = extension_pack<path_extension<TraitsType, ContextType>>;
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

        // if the NextSegmentType is itself a path (which normally should not
        // happen)
        static constexpr bool is_next_segment_nested =
          has_next_segment &&
          stl::same_as<next_segment_type, path<typename next_segment_type::segment_type,
                                               typename next_segment_type::next_segment_type>>;

        segment_type      segment{};
        next_segment_type next_segment{};

        /**
         * Add a new segment to the path and get a new path containing that specific segment and all the
         * previous ones
         */
        template <Segment NewNextSegmentType>
        constexpr auto operator/(NewNextSegmentType&& new_next_segment) const noexcept {
            if constexpr (!has_segment) {
                return route<path<NewNextSegmentType, void>>{path<NewNextSegmentType, void>{
                  .segment = stl::forward<NewNextSegmentType>(new_next_segment)}};
            } else if constexpr (!has_next_segment) {
                return route<path<segment_type, NewNextSegmentType>>{path<segment_type, NewNextSegmentType>{
                  .segment = segment, .next_segment = new_next_segment}};
            } else {
                using next_segment_t   = NewNextSegmentType;
                using new_segment_type = path<path<segment_type, next_segment_type>, next_segment_t>;
                return route<new_segment_type>{new_segment_type{
                  .segment = *this, .next_segment = stl::forward<NewNextSegmentType>(new_next_segment)}};
            }
        }

              private:
                template <typename NextSegType>
                struct make_a_path {
                    stl::decay_t<stl::remove_cvref_t<NextSegType>> new_next_segment;

                    auto operator()(PathContext auto const& ctx) const noexcept {
                        if constexpr (requires { {new_next_segment == ""}; }) {
                            return new_next_segment == ctx.path.current_segment;
                        } else if constexpr (requires { {"" == new_next_segment}; }) {
                            return ctx.path.current_segment == new_next_segment;
                        } else {
                            return false; // should not happen
                        }
                    }
                };

              public:

        /**
         * Convert those segments that can be compared with a string, to a normal segment type that have
         * an operator(context)
         */
        template <typename NewSegType>
        requires(ComparableToString<stl::remove_cvref_t<NewSegType>>&&
                   stl::is_class_v<stl::remove_cvref_t<NewSegType>> &&
                 !Segment<stl::remove_cvref_t<NewSegType>>) constexpr auto
        operator/(NewSegType new_next_segment) const noexcept {
//            return operator/([=](PathContext auto const& ctx) {
//                if constexpr (requires { {next_segment == ""}; }) {
//                    return new_next_segment == ctx.path.current_segment;
//                } else if constexpr (requires { {"" == new_next_segment}; }) {
//                    return ctx.path.current_segment == new_next_segment;
//                } else {
//                    return false; // should not happen
//                }
//            });
                        using type = stl::remove_cvref_t<NewSegType>;
                        return operator/(make_a_path<type>{.new_next_segment =
                        stl::move(new_next_segment)});
        }


        /**
         * Convert different original types to normal callable segments
         */
        template <typename NewSegType>
        constexpr auto operator/(NewSegType&& new_next_segment) const noexcept {
            using seg_type = stl::remove_cvref_t<NewSegType>;

            // int_type[N] => string_view
            if constexpr (stl::is_array_v<seg_type> &&
                          stl::is_integral_v<stl::remove_all_extents_t<seg_type>>) {
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
                return operator/([=](PathContext auto& ctx) {
                    return to<seg_type>(ctx.path.current_segment) == new_next_segment;
                });
            } else {

                throw stl::invalid_argument("The specified segment is not a valid segment type;"
                                            " it should be callable or comparable to a string "
                                            "or a known type that we can make it callable.");
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
                                                   T>)&&has_variable_name<segment_type>) {
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
                                                   T>)&&has_variable_name<next_segment_type>) {
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

        template <typename ContextType>
        requires(Context<stl::remove_cvref_t<ContextType>>) [[nodiscard]] auto
        operator()(ContextType&& ctx) noexcept {
            // handle inside-sub-route internal segment is done in this method

            using context_type                = stl::remove_cvref_t<ContextType>;
            using traits_type                 = typename context_type::traits_type;
            constexpr bool has_path_extension = requires {
                {ctx.path};
            };

            // run the user's codes; hopefully this will optimized away
            auto run = [&](auto&& ctx) {
                if constexpr (has_segment) {
                    using result_type = stl::invoke_result_t<segment_type, decltype(ctx)>;

                    // if the result of this segment is void
                    if constexpr (stl::is_void_v<result_type>) {
                        segment(ctx);
                        if constexpr (has_next_segment) {
                            return next_segment(ctx);
                        }
                        return;
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

                return; // explicitly saying that we have no return type
            };

            if constexpr (!has_path_extension) {

                // parse the uri
                // todo: we can optimize this, right? it parses the whole uri, do we need the whole uri? I think yes
                // fixme: should we decode it? if we decode it we need to care about the UTF-8 stuff as well?
                auto uri_segments =
                  basic_uri<traits_type, false>{ctx.request.request_uri()}.path_structured();
                using uri_segments_type = decltype(uri_segments);

                // context switching
                auto new_ctx = ctx.template clone<path_context_extension<path_type, uri_segments_type>>();
                static_assert(
                  requires { {new_ctx.path}; },
                  "For some reason, we're not able to perform context switching.");

                new_ctx.path.segments        = stl::move(uri_segments);
                new_ctx.path.current_segment = new_ctx.path.segments.begin();
                new_ctx.path.pth             = this;

                // nothing to do if the segment counts don't match
                if (new_ctx.path.current_segment == new_ctx.path.current_segment.end())
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

    // the root path
    constexpr auto root = path{} / "";

    // relative path
    constexpr auto relative = path{};

} // namespace webpp::routes

#endif // WEBPP_PATH_H
