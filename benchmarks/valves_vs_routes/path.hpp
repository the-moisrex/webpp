// Created by moisrex on 4/29/20.

#ifndef WEBPP_HTTP_ROUTE_PATH_HPP
#define WEBPP_HTTP_ROUTE_PATH_HPP

#include "../../webpp/extensions/extension.hpp"
#include "../../webpp/std/optional.hpp"
#include "../../webpp/std/tuple.hpp"
#include "../../webpp/uri/path.hpp"
#include "./route.hpp"

namespace webpp::http {

    template <typename T>
    concept PathContext = Context<T> && requires(T ctx) { ctx.path; };


    template <typename T>
    concept has_variable_name = requires(T seg) {
                                    { seg.variable_name } -> stl::convertible_to<stl::string_view>;
                                };

    /**
     * Check if the specified segment can parse a uri segment into the
     * specified type T
     * @tparam SegType
     * @tparam T
     */
    template <typename ContextType, typename SegType, typename T>
    concept can_parse_to = requires(SegType seg, ContextType ctx) {
                               { seg.template parse<T>(ctx) } -> stl::same_as<stl::optional<T>>;
                           };

    template <typename CtxT>
    concept HasPathExtension = Context<CtxT> && requires(CtxT ctx) { ctx.path; };


    /**
     * This class is used as a field type in the context type of the
     * internal sub routes of the "path" sub route.
     */
    template <Context ContextType, typename PathType, typename UriSegmentsType>
    struct path_field {
        using context_type           = ContextType;
        using traits_type            = typename context_type::traits_type;
        using string_view_type       = traits::string_view<traits_type>;
        using path_type              = PathType;
        using segments_type          = UriSegmentsType;
        using segments_iterator_type = typename segments_type::iterator;

        segments_type          segments{};
        segments_iterator_type current_segment{};


        /**
         * Next Segment
         */
        void next_segment() noexcept {
            ++current_segment;
        }

        /**
         * Check there is any other segments left or not
         */
        [[nodiscard]] bool is_last_segment() const noexcept {
            return current_segment == segments.cend() - 1;
        }

        /**
         * Checks if we're on the last segment and the last segment is empty string
         */
        [[nodiscard]] bool is_empty_last() const noexcept {
            return is_last_segment() && current_segment->empty();
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


    namespace details {

        template <typename NextSegType>
        struct make_a_path {
            NextSegType segment;

            template <PathContext PCType>
            [[nodiscard]] constexpr bool operator()(PCType const& ctx) const noexcept {
                if constexpr (requires {
                                  { segment == "" };
                              }) {
                    return segment == *ctx.path.current_segment;
                } else if constexpr (requires {
                                         { "" == segment };
                                     }) {
                    return *ctx.path.current_segment == segment;
                } else {
                    return false; // should not happen
                }
            }
        };

    } // namespace details

    /**
     * Path Route:
     *   This is a path type that will be used in the router and is a Route.
     *   This struct helps the user to identify a route by its recorded HTTP request's path.
     *
     * Examples:
     *  - path() / "page" / integer("page_num") / "profile" / user_id("user_id")
     */
    template <typename... Segments>
    struct path : stl::tuple<Segments...> {

        using tuple_type = stl::tuple<Segments...>;
        using path_type  = path<Segments...>;

        using stl::tuple<Segments...>::tuple; // constructors

        /**
         * Convert different original types to normal callable segments
         */
        template <typename NewSegType>
        constexpr auto operator/(NewSegType&& next_segment) const noexcept {
            using seg_type = stl::remove_cvref_t<NewSegType>;

            /*if constexpr (Segment<seg_type, path_type,
                                  decltype(basic_uri<fake_traits_type, false>{}.path_structured())>) {
            } else*/
            if constexpr (stl::is_array_v<seg_type> &&
                          stl::is_integral_v<stl::remove_all_extents_t<seg_type>>) {
                // int_type[N] => string_view
                using char_type     = stl::remove_all_extents_t<seg_type>;
                using str_view_type = stl::basic_string_view<char_type>;
                return operator/<str_view_type>(stl::forward<NewSegType>(next_segment));
            } else if constexpr (stl::is_pointer_v<seg_type> &&
                                 stl::is_integral_v<stl::remove_pointer_t<seg_type>>) {
                // char* => string_view
                using char_type = stl::remove_pointer_t<seg_type>;
                return operator/<stl::basic_string_view<char_type>>(stl::forward<NewSegType>(next_segment));
            } else if constexpr (stl::is_integral_v<seg_type>) {
                // integral types
                return operator/([=](PathContext auto const& ctx) constexpr noexcept -> bool {
                    return to<seg_type>(ctx.path.current_segment) == next_segment;
                });
            } else if constexpr (istl::ComparableToString<seg_type> && stl::is_class_v<seg_type>) {

                // Convert those segments that can be compared with a string, to a normal segment
                // type that have an operator(context)
                return operator/(
                  details::make_a_path<seg_type>{.segment = stl::forward<NewSegType>(next_segment)});
            } else {
                // segment
                using new_path_type  = path<Segments..., NewSegType>;
                using new_route_type = route<new_path_type>;
                return new_route_type{
                  ([&, this]<stl::size_t... index>(stl::index_sequence<index...>) constexpr noexcept {
                      return new_path_type{stl::get<index>(*this)..., stl::forward<NewSegType>(next_segment)};
                  })(stl::make_index_sequence<size()>{})};

                // todo: or give a compile time error if you can
                //                return operator/([=](Context auto const& ctx) constexpr noexcept {
                //                    return ctx.error(500u,
                //                                     "The specified segment is not a valid segment type;"
                //                                     " it should be callable or comparable to a string "
                //                                     "or a known type that we can make it callable.",
                //                                     next_segment);
                //                });
            }
        }


      private:
        /**
         * Call the segment and catch the exceptions if there are any
         */
        template <typename CallableT, typename... Args>
        static inline bool call_and_catch(CallableT&& callable, Args&&... args) noexcept {
            if constexpr (stl::is_nothrow_invocable_v<CallableT, Args...>) {
                // It's noexcept, we call it knowing that.
                return callable(stl::forward<Args>(args)...);
            } else if constexpr (stl::is_invocable_v<CallableT, Args...>) {
                try {
                    return callable(stl::forward<Args>(args)...);
                } catch (...) {
                    return false;
                    // todo: add a warning here for the user
                }
            } else {
                // todo: add an error here
                return false;
            }
        }

        /**
         * call the specified segment with the right argument and handle the exceptions
         */
        [[nodiscard]] static inline bool call_segment(auto&& seg, auto&& ctx, auto const& req) noexcept {
            using context_type = decltype(ctx);
            using req_type     = decltype(req);
            using seg_type     = stl::remove_cvref_t<decltype(seg)>;
            if constexpr (stl::is_invocable_v<seg_type, context_type, req_type>) {
                bool const res = call_and_catch(seg, ctx, req);
                ctx.path.next_segment();
                return res;
            } else if constexpr (stl::is_invocable_v<seg_type, req_type, context_type>) {
                bool const res = call_and_catch(seg, req, ctx);
                ctx.path.next_segment();
                return res;
            } else if constexpr (stl::is_invocable_v<seg_type, context_type>) {
                bool const res = call_and_catch(seg, ctx);
                ctx.path.next_segment();
                return res;
            } else if constexpr (stl::is_invocable_v<seg_type>) {
                bool const res = call_and_catch(seg);
                ctx.path.next_segment();
                return res;
            } else {
                static_assert_false(seg_type, "The specified segment in the path cannot be called.");
            }
        }

      public:
        static constexpr stl::size_t size() noexcept {
            return sizeof...(Segments);
        }


        template <typename ContextType, typename ReqType>
        constexpr auto switch_context(ContextType&& ctx, ReqType const& req) const noexcept {

            using context_type = stl::remove_cvref_t<ContextType>;
            using traits_type  = typename context_type::traits_type;
            using string_type  = traits::general_string<traits_type>;

            if constexpr (HasPathExtension<context_type>) {
                return ctx;
            } else {

                // Performing in-place context switching (meaning we switch the context for the current
                // segment and not just the next segment)

                // parse the uri
                // todo: we can optimize this, right? it parses the whole uri, do we need the whole uri? I think yes
                // fixme: should we decode it? if we decode it we need to care about the UTF-8 stuff as well?
                // todo: move this parsing into the request so we don't have to do it more than once for one request
                uri::basic_path<string_type> uri_segments{alloc::allocator_for<string_type>(ctx)};
                if (!uri_segments.parse(req.uri())) {
                    uri_segments.clear();
                } else {
                    uri_segments.fix();
                }
                using uri_segments_type = decltype(uri_segments);


                // context switching
                auto new_ctx = ctx.template clone<path_context_extension<path_type, uri_segments_type>>();
                static_assert(
                  requires { new_ctx.path; },
                  "For some reason, we're not able to perform context switching.");

                new_ctx.path.segments        = stl::move(uri_segments);
                new_ctx.path.current_segment = new_ctx.path.segments.begin();

                return new_ctx;
            }
        }

        /**
         * Check if the specified context has a path extension or not
         */
        template <Context CtxT>
        static constexpr bool verify_context(CtxT const& ctx) noexcept {
            if constexpr (HasPathExtension<CtxT>) {
                // the URI is empty, so no checking it
                // todo: we used to check the errors of the segments here, but the path class doesn't provide that anymore
                return !ctx.path.segments.empty();
            } else {
                return false;
            }
        }

        template <typename ContextType>
            requires(Context<stl::remove_cvref_t<ContextType>>) // Context
        [[nodiscard]] bool operator()(ContextType&& ctx, HTTPRequest auto&& req) noexcept {
            // handle inside-sub-route internal segment is done in this method

            if constexpr (HasPathExtension<ContextType>) {
                // we do have the path extension applied, so we're safe to run it
                return (ctx.path.segments.size() == size()) && // Don't bother checking the path
                                                               // if the size of the paths are not equal
                       verify_context(ctx) &&                  // Check if the request and context are valid
                       ([&, this]<stl::size_t... index>(stl::index_sequence<index...>) constexpr noexcept {
                           // First, increment the current segment in the path extension
                           // then call the segment
                           // then check whether the next segment needs to be called or not
                           return (call_segment(stl::get<index>(*this), ctx, req) && ...);
                       })(stl::make_index_sequence<size()>{});
            } else {
                // Inject path extension with the help of "context switching"
                // Attention: this better be done outside the path struct because if it's for example done
                // in the router, then all the paths will have access to the same parsed URI info instead
                // of re-parsing on each route.


                // re-running this member function with the new switched context type
                return operator()(switch_context(stl::forward<ContextType>(ctx), req), req);
            }
        }


        template <istl::String StrT = stl::string>
        void to_string(StrT& out) const {
            ([&, this]<stl::size_t... index>(stl::index_sequence<index...>) constexpr noexcept {
                (([&](auto const& seg) {
                     out.append(" /");
                     if constexpr (requires { out.append(seg.data(), seg.size()); }) {
                         out.append(seg.data(), seg.size());
                     } else {
                         out.append(" [path]");
                     }
                 })(stl::get<index>(*this)),
                 ...);
            })(stl::make_index_sequence<size()>{});
        }
    };

    // the root path
    constexpr auto root = path{} / "";

    // relative path
    constexpr path relative{};



    inline namespace literals {
        constexpr auto operator""_path(const char* str, std::size_t len) noexcept {
            return path{} / stl::string_view{str, len};
        }
    } // namespace literals


} // namespace webpp::http

#endif // WEBPP_HTTP_ROUTE_PATH_HPP
