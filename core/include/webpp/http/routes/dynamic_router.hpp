#ifndef WEBPP_DYNAMIC_ROUTER_HPP
#define WEBPP_DYNAMIC_ROUTER_HPP

#include "../../extensions/extension.hpp"
#include "../../std/functional.hpp"
#include "../../std/string.hpp"
#include "../../std/vector.hpp"
#include "../../traits/default_traits.hpp"
#include "../../traits/enable_traits.hpp"
#include "../http_concepts.hpp"
#include "../request.hpp"
#include "../request_view.hpp"
#include "../response.hpp"
#include "../status_code.hpp"
#include "./context.hpp"
#include "route.hpp"

#include <any>

namespace webpp::http {

    namespace details {

        template <typename Callable, typename ContextType>
        struct route_traits {
            using callable_type      = stl::remove_cvref_t<Callable>;
            using context_type       = ContextType;
            using request_type       = typename context_type::request_type;
            using response_type      = typename context_type::response_type;
            using response_body_type = typename response_type::body_type;
            using invocable_inorder_type =
              istl::invocable_inorder<callable_type, context_type&, request_type&, response_type&>;
            using return_type = stl::remove_cvref_t<typename invocable_inorder_type::result>;

            // static_assert(invocable_inorder_type::value, "We're not able to call your route.");

            static constexpr auto call(Callable&&    callable,
                                       context_type& ctx) noexcept(invocable_inorder_type::is_nothrow)
                requires(invocable_inorder_type::value)
            {
                return istl::invoke_inorder(callable, ctx, ctx.request, ctx.response);
            }

            template <typename T>
                requires(istl::same_as_cvref<T, Callable>)
            static constexpr auto call(T&&           segment,
                                       context_type& ctx) noexcept(invocable_inorder_type::is_nothrow)
                requires(istl::StringViewifiable<Callable>)
            {
                return ctx.check_segment(stl::forward<T>(segment));
            }


            template <typename R>
            static constexpr bool is_positive(R&& ret) noexcept {
                if constexpr (stl::same_as<return_type, bool>) {
                    return ret;
                } else if constexpr (istl::part_of<return_type, istl::nothing_type>) {
                    return false;
                } else if constexpr (stl::is_void_v<return_type> || HTTPResponse<return_type>) {
                    return true;
                } else if constexpr (stl::is_pointer_v<return_type>) {
                    return ret != nullptr;
                } else if constexpr (istl::Optional<return_type>) {
                    return bool{ret};
                } else {
                    return false;
                }
            }


            template <typename R>
            static constexpr bool is_prerouting_positive(R&& ret) noexcept {
                if constexpr (HTTPResponse<return_type>) {
                    return false;
                } else {
                    return is_positive(ret);
                }
            }

            template <typename R>
            static constexpr bool is_postrouting_positive(R&& ret) noexcept {
                if constexpr (HTTPResponse<return_type>) {
                    return true;
                } else {
                    return is_positive(ret);
                }
            }

            template <typename R>
            static constexpr void set_response(R&& ret, context_type& ctx) {
                using ret_t = stl::remove_cvref_t<R>;
                if constexpr (stl::is_void_v<ret_t> || istl::part_of<ret_t, bool, istl::nothing_type>) {
                    // ignore the result
                } else if constexpr (HTTPResponse<ret_t> || HTTPResponseBody<ret_t>) {
                    ctx.response = stl::forward<R>(ret);
                } else if constexpr (istl::Optional<ret_t>) {
                    if (ret) {
                        set_response(stl::move(*ret), ctx);
                    }
                } else if constexpr (stl::is_pointer_v<ret_t>) {
                    if (ret != nullptr) {
                        set_response(*ret, ctx);
                    }
                } else if constexpr (stl::same_as<ret_t, http::status_code>) {
                    ctx.response = ret;
                } else if constexpr (stl::integral<ret_t>) {
                    ctx.response = static_cast<http::status_code>(ret);
                } else if constexpr (HTTPSerializableBody<ret_t, response_body_type>) {
                    ctx.response = stl::forward<R>(ret);
                } else {
                    static_assert_false(ret_t, "We don't know what to do with your route's return type.");
                }
            }


            template <typename R>
            static constexpr bool set_get_response(R&& ret, context_type& ctx) {
                bool const res = is_positive(ret);
                set_response(stl::forward<R>(ret), ctx);
                return res;
            }


            static constexpr void
            call_and_set(Callable&& segment, context_type& ctx) noexcept(invocable_inorder_type::is_nothrow) {
                if constexpr (stl::is_void_v<return_type>) {
                    call(stl::forward<Callable>(segment), ctx);
                } else {
                    set_response(call(stl::forward<Callable>(segment), ctx), ctx);
                }
            }


            static constexpr bool
            call_set_get(Callable&& segment, context_type& ctx) noexcept(invocable_inorder_type::is_nothrow) {
                if constexpr (stl::is_void_v<return_type>) {
                    call(stl::forward<Callable>(segment), ctx);
                    return true;
                } else {
                    return set_get_response(call(stl::forward<Callable>(segment), ctx), ctx);
                }
            }
        };

        template <typename PreRoute, typename Callable>
        struct pre_route;
        template <typename Callable, typename PostRoute>
        struct post_route;
        template <typename Callable>
        struct not_callable;
        template <typename Callable>
        struct negative_callable;
        template <typename Callable>
        struct positive_callable;
        template <typename LeftCallable, typename RightCallable>
        struct and_callables;
        template <typename LeftCallable, typename RightCallable>
        struct or_callables;
        template <typename... Callables>
        struct forward_callables;
        template <typename... CallableSegments>
        struct segment_callables;




        template <typename C>
        struct route_optimizer {
            using type = C;
        };


        // move pre-routing hooks to the left because they supposed to be called before everything
        template <template <typename, typename> typename CallableTemplate,
                  typename Left,
                  typename Pre,
                  typename Right>
        struct route_optimizer<CallableTemplate<Left, pre_route<Pre, Right>>>
          : route_optimizer<CallableTemplate<
              pre_route<typename route_optimizer<Pre>::type, typename route_optimizer<Left>::type>,
              typename route_optimizer<Right>::type>> {};

        // move post-routing hooks to the right because they supposed to be called after everything
        template <template <typename, typename> typename CallableTemplate,
                  typename Left,
                  typename Post,
                  typename Right>
        struct route_optimizer<CallableTemplate<post_route<Left, Post>, Right>>
          : route_optimizer<CallableTemplate<
              typename route_optimizer<Left>::type,
              post_route<typename route_optimizer<Right>::type, typename route_optimizer<Post>::type>>> {};

        // !!C == C
        template <typename C>
        struct route_optimizer<not_callable<not_callable<C>>> : route_optimizer<C> {};

        // C || !C == +C
        template <typename C>
        struct route_optimizer<or_callables<C, not_callable<C>>> : route_optimizer<positive_callable<C>> {};

        // !C || C == +C
        template <typename C>
        struct route_optimizer<or_callables<not_callable<C>, C>> : route_optimizer<positive_callable<C>> {};

        // C && C == C
        template <typename C>
        struct route_optimizer<and_callables<C, C>> : route_optimizer<C> {};

        // C && !C == -C
        template <typename C>
        struct route_optimizer<and_callables<C, not_callable<C>>> : route_optimizer<negative_callable<C>> {};

        // !C && C == -C
        template <typename C>
        struct route_optimizer<and_callables<not_callable<C>, C>> : route_optimizer<negative_callable<C>> {};

        // remove first voids
        template <typename... C>
        struct route_optimizer<forward_callables<void, C...>> : route_optimizer<forward_callables<C...>> {};


        // C || Positive == ?
        // C && Negative == ?


        template <typename C>
        using route_optimizer_t = typename route_optimizer<C>::type;





        template <typename Self>
        struct route_root {

            template <typename Callable>
            [[nodiscard]] constexpr auto operator>>(Callable&& callable) const {
                if constexpr (stl::is_void_v<Self>) {
                    return forward_callables<stl::remove_cvref_t<Callable>>{stl::forward<Callable>(callable)};
                } else {
                    return route_optimizer_t<forward_callables<Self, stl::remove_cvref_t<Callable>>>{
                      .callables{
                        *static_cast<Self const*>(this), // route 1
                        stl::forward<Callable>(callable) // route 2
                      }};
                }
            }

            template <typename Callable>
            [[nodiscard]] constexpr auto operator&&(Callable&& callable) const {
                if constexpr (stl::is_void_v<Self>) {
                    return operator>>(stl::forward<Callable>(callable));
                } else {
                    return route_optimizer_t<and_callables<Self, stl::remove_cvref_t<Callable>>>{
                      .lhs = *static_cast<Self const*>(this), // left
                      .rhs = stl::forward<Callable>(callable) // right
                    };
                }
            }


            template <typename Callable>
            [[nodiscard]] constexpr auto operator||(Callable&& callable) const {
                if constexpr (stl::is_void_v<Self>) {
                    return operator>>(stl::forward<Callable>(callable));
                } else {
                    return route_optimizer_t<or_callables<Self, stl::remove_cvref_t<Callable>>>{
                      .lhs = *static_cast<Self const*>(this), // left
                      .rhs = stl::forward<Callable>(callable) // right
                    };
                }
            }


            template <typename Callable>
            [[nodiscard]] constexpr auto operator!() const {
                if constexpr (stl::is_void_v<Self>) {
                    return *this;
                } else {
                    return route_optimizer_t<not_callable<Self>>{.callable = *static_cast<Self const*>(this)};
                }
            }


            template <typename Callable>
            [[nodiscard]] constexpr auto operator+(Callable&& callable) const {
                if constexpr (stl::is_void_v<Self>) {
                    return operator>>(stl::forward<Callable>(callable));
                } else {
                    return route_optimizer_t<post_route<Self, stl::remove_cvref_t<Callable>>>{
                      .callable = *static_cast<Self const*>(this), // self
                      .post     = stl::forward<Callable>(callable) // post routing callable
                    };
                }
            }


            template <typename Callable>
            [[nodiscard]] constexpr auto operator-(Callable&& callable) const {
                if constexpr (stl::is_void_v<Self>) {
                    return operator>>(stl::forward<Callable>(callable));
                } else {
                    return route_optimizer_t<pre_route<stl::remove_cvref_t<Callable>, Self>>{
                      .pre      = stl::forward<Callable>(callable), // pre routing callable
                      .callable = *static_cast<Self const*>(this)   // self
                    };
                }
            }



            template <typename CallableSegment>
            [[nodiscard]] constexpr auto operator/(CallableSegment&& inp_segment) const {
                if constexpr (stl::is_void_v<Self>) {
                    return segment_callables<stl::remove_cvref_t<CallableSegment>>{
                      .segment = stl::forward<CallableSegment>(inp_segment)};
                } else {
                    return route_optimizer_t<segment_callables<Self, stl::remove_cvref_t<CallableSegment>>>{
                      .segments{
                        *static_cast<Self const*>(this),           // first segment
                        stl::forward<CallableSegment>(inp_segment) // second segment
                      }};
                }
            }


            template <typename SegT>
                requires(istl::StringView<SegT> || stl::is_array_v<stl::remove_cvref_t<SegT>>)
            [[nodiscard]] constexpr auto operator/(SegT&& inp_segment) const {
                auto const seg_v = istl::string_viewify(stl::forward<SegT>(inp_segment));
                using seg_t      = stl::remove_cvref_t<decltype(seg_v)>;
                if constexpr (stl::is_void_v<Self>) {
                    return segment_callables<seg_t>{.segment = seg_v};
                } else {
                    return route_optimizer_t<segment_callables<Self, seg_t>>{.segments{
                      *static_cast<Self const*>(this), // first segment
                      seg_v                            // second segment
                    }};
                }
            }
        };




        template <typename... Callables>
        struct forward_callables : route_root<forward_callables<Callables...>> {
            stl::tuple<Callables...> callables;

            template <Traits TraitsType>
            constexpr void operator()(basic_context<TraitsType>& ctx) {
                using context_type = basic_context<TraitsType>;

                stl::apply(
                  [&ctx]<typename... T>(T&&... callees) constexpr {
                      (route_traits<T, context_type>::call_and_set(stl::forward<T>(callees), ctx), ...);
                  },
                  callables);
            }
        };

        template <typename Callable>
        struct forward_callables<Callable> : route_root<forward_callables<Callable>> {
            Callable callable;

            template <Traits TraitsType>
            constexpr void operator()(basic_context<TraitsType>& ctx) {
                using context_type    = basic_context<TraitsType>;
                using callable_traits = route_traits<Callable, context_type>;

                callable_traits::set_response(callable_traits::call(callable, ctx), ctx);
            }
        };

        template <>
        struct forward_callables<void> : route_root<void> {
            template <Traits TraitsType>
            constexpr void operator()(basic_context<TraitsType>&) const {}
        };

        template <>
        struct forward_callables<> : route_root<void> {
            template <Traits TraitsType>
            constexpr void operator()(basic_context<TraitsType>&) const {}
        };

        template <typename PreRoute, typename Callable>
        struct pre_route : route_root<pre_route<PreRoute, Callable>> {
            PreRoute pre;
            Callable callable;

            template <Traits TraitsType>
            constexpr void operator()(basic_context<TraitsType>& ctx) {
                using context_type    = basic_context<TraitsType>;
                using pre_traits      = route_traits<PreRoute, context_type>;
                using callable_traits = route_traits<Callable, context_type>;

                auto pre_res = pre_traits::call(pre, ctx);
                if (pre_traits::is_prerouting_positive(pre_res)) {
                    pre_traits::set_response(stl::move(pre_res), ctx);
                    callable_traits::set_response(callable_traits::call(callable, ctx), ctx);
                }
            }
        };

        template <typename PreRoute>
        struct pre_route<PreRoute, void> : route_root<pre_route<PreRoute, void>> {
            PreRoute pre;

            template <Traits TraitsType>
            constexpr void operator()(basic_context<TraitsType>& ctx) {
                using context_type = basic_context<TraitsType>;
                using pre_traits   = route_traits<PreRoute, context_type>;

                pre_traits::call_and_set(pre, ctx);
            }
        };


        template <typename Callable, typename PostRoute>
        struct post_route : route_root<post_route<Callable, PostRoute>> {
            Callable  callable;
            PostRoute post;

            template <Traits TraitsType>
            constexpr void operator()(basic_context<TraitsType>& ctx) {
                using context_type    = basic_context<TraitsType>;
                using callable_traits = route_traits<Callable, context_type>;
                using post_traits     = route_traits<PostRoute, context_type>;

                auto callable_res = callable_traits::call(callable, ctx);
                if (callable_traits::is_postrouting_positive(callable_res)) {
                    callable_traits::set_response(stl::move(callable_res), ctx);
                    post_traits::call_and_set(post, ctx);
                }
            }
        };

        template <typename PostRoute>
        struct post_route<void, PostRoute> : route_root<post_route<void, PostRoute>> {
            PostRoute post;

            template <Traits TraitsType>
            constexpr void operator()(basic_context<TraitsType>& ctx) {
                using context_type = basic_context<TraitsType>;
                using post_traits  = route_traits<PostRoute, context_type>;

                post_traits::call_and_set(post, ctx);
            }
        };


        template <typename Callable>
        struct not_callable : route_root<not_callable<Callable>> {
            using callable_type = Callable;

            callable_type callable;

            template <Traits TraitsType>
            constexpr void operator()(basic_context<TraitsType>& ctx) {
                using context_type = basic_context<TraitsType>;
                using ctraits      = route_traits<callable_type, context_type>;

                auto res = ctraits::call(callable, ctx);
                if (!ctraits::is_positive(res)) {
                    ctraits::set_response(stl::move(res), ctx);
                }
            }
        };

        template <>
        struct not_callable<void> : route_root<not_callable<void>> {
            template <Traits TraitsType>
            constexpr void operator()(basic_context<TraitsType>&) const {}
        };

        template <typename Callable>
        struct negative_callable : route_root<negative_callable<Callable>> {
            using callable_type = Callable;

            callable_type callable;

            template <Traits TraitsType>
            constexpr bool operator()(basic_context<TraitsType>& ctx) {
                using context_type = basic_context<TraitsType>;
                using ctraits      = route_traits<callable_type, context_type>;

                ctraits::call_and_set(callable, ctx);
                return false;
            }
        };

        template <typename Callable>
        struct positive_callable : route_root<positive_callable<Callable>> {
            using callable_type = Callable;

            callable_type callable;

            template <Traits TraitsType>
            constexpr bool operator()(basic_context<TraitsType>& ctx) {
                using context_type = basic_context<TraitsType>;
                using ctraits      = route_traits<callable_type, context_type>;

                ctraits::call_and_set(callable, ctx);
                return true;
            }
        };



        template <typename LeftCallable, typename RightCallable>
        struct and_callables : route_root<and_callables<LeftCallable, RightCallable>> {
            using left_type  = LeftCallable;
            using right_type = RightCallable;

            left_type  lhs;
            right_type rhs;

            template <Traits TraitsType>
            constexpr void operator()(basic_context<TraitsType>& ctx) {
                using context_type = basic_context<TraitsType>;
                using left_traits  = route_traits<left_type, context_type>;
                using right_traits = route_traits<right_type, context_type>;

                auto left_res = left_traits::call(lhs, ctx);
                if (!left_traits::is_positive(left_res)) {
                    left_traits::set_response(stl::move(left_res), ctx);
                    return;
                }
                right_traits::call_and_set(rhs, ctx);
            }
        };

        template <typename RightCallable>
        struct and_callables<void, RightCallable> : forward_callables<RightCallable> {};


        template <typename LeftCallable, typename RightCallable>
        struct or_callables : route_root<or_callables<LeftCallable, RightCallable>> {
            using left_type  = LeftCallable;
            using right_type = RightCallable;

            left_type  lhs;
            right_type rhs;

            template <Traits TraitsType>
            constexpr void operator()(basic_context<TraitsType>& ctx) {
                using context_type = basic_context<TraitsType>;
                using left_traits  = route_traits<left_type, context_type>;
                using right_traits = route_traits<right_type, context_type>;

                auto left_res = left_traits::call(lhs, ctx);
                if (left_traits::is_positive(left_res)) {
                    left_traits::set_response(stl::move(left_res), ctx);
                    return;
                }
                right_traits::call_and_set(rhs, ctx);
            }
        };

        template <typename RightCallable>
        struct or_callables<void, RightCallable> : forward_callables<RightCallable> {};


        template <typename... CallableSegments>
        struct segment_callables : route_root<segment_callables<CallableSegments...>> {
            stl::tuple<CallableSegments...> segments;

            template <Traits TraitsType>
            constexpr void operator()(basic_context<TraitsType>& ctx) {
                using context_type = basic_context<TraitsType>;

                stl::apply(
                  [&ctx]<typename... T>(T&&... callables) constexpr {
                      (route_traits<T, context_type>::call_set_get(stl::forward<T>(callables), ctx) && ...);
                  },
                  segments);
            }
        };


        template <>
        struct segment_callables<void> : route_root<segment_callables<void>> {
            template <Traits TraitsType>
            constexpr void operator()(basic_context<TraitsType>&) const noexcept {}
        };

        template <>
        struct segment_callables<> : route_root<segment_callables<>> {
            template <Traits TraitsType>
            constexpr void operator()(basic_context<TraitsType>&) const noexcept {}
        };


        template <typename CallableSegment>
        struct segment_callables<CallableSegment> : route_root<segment_callables<CallableSegment>> {
            CallableSegment segment;

            template <Traits TraitsType>
            constexpr void operator()(basic_context<TraitsType>& ctx) {
                using context_type   = basic_context<TraitsType>;
                using segment_traits = route_traits<CallableSegment, context_type>;

                segment_traits::set_response(segment_traits::call(segment, ctx), ctx);
            }
        };

    } // namespace details


    template <Traits TraitsType>
    struct dynamic_route
      : istl::function<void(basic_context<TraitsType>&), traits::general_allocator<TraitsType, stl::byte>>,
        details::route_root<dynamic_route<TraitsType>> {
        using traits_type     = TraitsType;
        using string_type     = traits::general_string<traits_type>;
        using route_allocator = traits::general_allocator<traits_type, stl::byte>;
        using context_type    = basic_context<traits_type>;
        using callable_type   = istl::function<void(context_type&), route_allocator>;

        using istl::function<void(basic_context<TraitsType>&),
                             traits::general_allocator<TraitsType, stl::byte>>::function;
    };

    /**
     * @brief A Router that's is fully customizable at runtime
     *
     * This class will be used directly by the developers using this whole library. So be nice and careful and
     * user friendly.
     */
    template <ExtensionList RootExtensions, EnabledTraits TraitsEnabler>
    struct basic_dynamic_router : TraitsEnabler, details::route_root<void> {
        using root_extensions   = RootExtensions;
        using etraits           = TraitsEnabler;
        using traits_type       = typename etraits::traits_type;
        using non_owner_etraits = typename etraits::non_owner_type;
        using route_type        = dynamic_route<traits_type>;
        using vector_allocator  = traits::general_allocator<traits_type, route_type>;
        using string_type       = traits::general_string<traits_type>;
        using string_view_type  = traits::string_view<traits_type>;
        using objects_type      = stl::vector<stl::any, traits::general_allocator<traits_type, stl::any>>;
        using routes_type       = stl::vector<route_type, vector_allocator>;
        using response_type     = basic_response<traits_type>;
        using context_type      = basic_context<traits_type>;
        using request_type      = basic_request<traits_type>;

        static constexpr auto log_cat = "DRouter";

        static_assert(HTTPResponse<response_type>,
                      "For some reason the response type is not a valid match for HTTPResponse concept.");

      private:
        // todo: implement a function_vector that'll require only one allocator not one for each
        routes_type routes;


        /**
         * This method checks the context and see if we have reached the end of the routing or not.
         */
        template <typename CtxT>
        [[nodiscard]] constexpr bool continue_routing(CtxT& ctx) const noexcept {
            return ctx.response.empty();
        }

      public:
        // These are the callable types
        objects_type objects;

        // we're not adding context and response here in router scope because we want the user to be able to
        // take advantage of parallelism


        constexpr basic_dynamic_router() noexcept
            requires(etraits::is_resource_owner)
          : etraits{},
            objects{alloc::general_alloc_for<objects_type>(*this)} {}

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        template <EnabledTraits ET>
            requires(!istl::same_as_cvref<ET, basic_dynamic_router>)
        constexpr basic_dynamic_router(ET&& et)
          : etraits{stl::forward<ET>(et)},
            objects{alloc::general_alloc_for<objects_type>(*this)} {}
        // NOLINTEND(bugprone-forwarding-reference-overload)


        /**
         * @brief Register a member function and it's object; It's the same as using std::mem_fn.
         */
        template <typename T, typename U>
            requires(stl::is_member_function_pointer_v<T>)
        constexpr auto routify(T&& method, U&& obj) noexcept {
            using method_type = member_function_pointer<stl::remove_cvref_t<T>>;
            using type        = typename method_type::type;
            using return_type = typename method_type::return_type;
            static_assert(stl::same_as<type, stl::remove_cvref_t<U>>,
                          "The specified member function is not from the specified object.");

            return routify(
              [callable = obj, method]<typename... Args> requires(
                method_type::template is_same_args_v<Args...>)(
                Args && ... args) constexpr noexcept(method_type::is_noexcept) {
                                                             return stl::invoke_result_t<return_type,
                                                                                         Args...>(
                                                               method,
                                                               callable,
                                                               stl::forward<Args>(args)...);
                                                         });
        }

        /**
         * @brief Register a member function as a route to call
         * You will only need to pass the member function and not the object itself; for this function to
         * work, you have to either:
         *   - make sure object type is default constructible, or
         *   - make sure you have already passed the object of type T to the "objects".
         * If you haven't added the object to the "objects" list, then it tries to default-construct it.
         */
        template <typename T>
            requires(stl::is_member_function_pointer_v<T>)
        constexpr route_type routify(T&& method) noexcept {
            using method_type = member_function_pointer<stl::remove_cvref_t<T>>;
            using type        = typename method_type::type;
            for (auto& obj : objects) {
                if (obj.type() == typeid(type)) {
                    return routify(method, stl::any_cast<type>(obj));
                }
            }

            // default constructing it if it's possible and use that object
            if constexpr (stl::is_default_constructible_v<type>) {
                objects.emplace_back(type{});
                return routify(method, stl::any_cast<type>(objects.back()));
            } else {
                this->logger.error(
                  log_cat,
                  fmt::format("You have not specified an object with typeid of '{}' in your dynamic router,"
                              " but you've tried to register a member function of unknown type for router.",
                              typeid(type).name()));
                return invalid_route(
                  "Unknown member function registered as a route; see the logs for more detail.");
            }
        }

        constexpr route_type invalid_route([[maybe_unused]] string_type const& str) {
            return error(status_code::insufficient_storage, str);
        }

        /**
         * Response with the specified status code.
         */
        constexpr response_type error(status_code code) {
            return response(code);
        }

        constexpr response_type response(status_code code) {
            return {this->get_traits(), code};
        }


        // Append a migration
        template <typename C>
        constexpr basic_dynamic_router& operator+=(C&& callable) {
            routes.emplace_back(details::route_optimizer_t<C>{stl::forward<C>(callable)});
            return *this;
        }


        /**
         * Calling this will get you a response.
         * Don't call this operator for your sub-routers; this will guarantees a response and your
         * parent router will stop processing its next routes because this router returned a 404 response.
         */
        template <HTTPRequest ReqType>
            requires(!Context<ReqType>)
        constexpr response_type operator()(ReqType&& in_req) {
            context_type ctx{in_req};

            // call the router with the specified context, fill the response
            this->operator()<context_type>(ctx);

            // if it didn't fill the response:
            if (ctx.response.empty()) {
                // fill the response with 404 error page
                ctx.response = error(http::status_code::not_found);
            }

            return ctx.response;
        }

        /**
         * Run the router with the specified context;
         * Sets the context's response if necessary
         *
         * This method does not set 404 error message at all, give you a chance to use this router as
         * a sub-router of a parent router and let the parent router to deal with these things.
         */
        template <Context CtxT>
        constexpr void operator()(CtxT& ctx) {
            for (auto& route : routes) {
                route(ctx);
                if (!continue_routing(ctx)) {
                    return;
                }
            }
        }
    };


    using dynamic_router =
      basic_dynamic_router<empty_extension_pack, enable_owner_traits<default_dynamic_traits>>;


    namespace pmr {
        using dynamic_router =
          webpp::http::basic_dynamic_router<empty_extension_pack, enable_owner_traits<std_pmr_traits>>;
    } // namespace pmr

} // namespace webpp::http

#endif // WEBPP_DYNAMIC_ROUTER_HPP
