#ifndef WEBPP_DYNAMIC_ROUTER_HPP
#define WEBPP_DYNAMIC_ROUTER_HPP

#include "../../extensions/extension.hpp"
#include "../../std/functional.hpp"
#include "../../std/memory.hpp"
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



    template <typename Callable>
    static constexpr void route_to_string(istl::String auto& out, Callable& func) {
        using mem_traits = istl::member_function_pointer_traits<Callable>;
        if constexpr (istl::StringViewifiable<Callable>) {
            out.append(" \"");
            out += istl::string_viewify(func);
            out.append("\"");
        } else if constexpr (requires { func.to_string(out); }) {
            func.to_string(out);
        } else if constexpr (mem_traits::value) {
            out.append(" ");
            out.append(typeid(typename mem_traits::type).name());
            out.append("::");
            out.append(typeid(Callable).name());
        } else {
            out.append(" ");
            out.append(typeid(Callable).name());
        }
    }


    template <Traits TraitsType, typename Callable = void>
    struct dynamic_route final : dynamic_route<TraitsType, void> {

        using traits_type   = TraitsType;
        using string_type   = traits::general_string<traits_type>;
        using context_type  = basic_context<traits_type>;
        using callable_type = stl::remove_cvref_t<Callable>;

      private:
        callable_type callable;

      public:
        dynamic_route(Callable&& new_callable) noexcept : callable(stl::move(new_callable)) {}
        dynamic_route(Callable const& new_callable) noexcept(
          stl::is_nothrow_copy_constructible_v<callable_type>)
          : callable(new_callable) {}
        dynamic_route(dynamic_route const&)                     = delete;
        dynamic_route(dynamic_route&&) noexcept                 = default;
        dynamic_route& operator=(dynamic_route const&) noexcept = delete;
        dynamic_route& operator=(dynamic_route&&) noexcept      = default;
        ~dynamic_route() final                                  = default;

        void operator()(context_type& ctx) final {
            callable(ctx);
        }

        void to_string(string_type& out) const final {
            route_to_string(out, callable);
        }
    };


    template <Traits TraitsType>
    struct dynamic_route<TraitsType, void> {
        using traits_type  = TraitsType;
        using string_type  = traits::general_string<traits_type>;
        using context_type = basic_context<traits_type>;

        dynamic_route()                                         = default;
        dynamic_route(dynamic_route const&)                     = default;
        dynamic_route(dynamic_route&&) noexcept                 = default;
        dynamic_route& operator=(dynamic_route const&) noexcept = default;
        dynamic_route& operator=(dynamic_route&&) noexcept      = default;

        virtual ~dynamic_route() = default;

        virtual void operator()(context_type& ctx)     = 0;
        virtual void to_string(string_type& out) const = 0;
    };


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

        static constexpr auto call(Callable&     callable,
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
            } else if constexpr (HTTPResponse<ret_t> || HTTPResponseBody<ret_t> ||
                                 HTTPSerializableBody<ret_t, response_body_type>) {
                ctx.response = stl::forward<R>(ret);
            } else if constexpr (stl::same_as<ret_t, http::status_code>) {
                ctx.response = ret;
            } else if constexpr (stl::integral<ret_t>) {
                ctx.response = static_cast<http::status_code>(ret);
            } else if constexpr (istl::Optional<ret_t>) {
                if (ret) {
                    set_response(stl::move(*ret), ctx);
                }
            } else if constexpr (stl::is_pointer_v<ret_t>) {
                if (ret != nullptr) {
                    set_response(*ret, ctx);
                }
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


        static constexpr void call_and_set(Callable&&    segment,
                                           context_type& ctx) noexcept(invocable_inorder_type::is_nothrow) {
            if constexpr (stl::is_void_v<return_type>) {
                call(stl::forward<Callable>(segment), ctx);
            } else {
                set_response(call(stl::forward<Callable>(segment), ctx), ctx);
            }
        }


        static constexpr bool call_set_get(Callable&&    segment,
                                           context_type& ctx) noexcept(invocable_inorder_type::is_nothrow) {
            if constexpr (stl::is_void_v<return_type>) {
                call(stl::forward<Callable>(segment), ctx);
                return true;
            } else {
                return set_get_response(call(stl::forward<Callable>(segment), ctx), ctx);
            }
        }
    };



    template <Traits TraitsType, typename PreRoute, typename Callable>
    struct pre_route;
    template <Traits TraitsType, typename Callable, typename PostRoute>
    struct post_route;
    template <Traits TraitsType, typename Callable>
    struct not_callable;
    template <Traits TraitsType, typename Callable>
    struct negative_callable;
    template <Traits TraitsType, typename Callable>
    struct positive_callable;
    template <Traits TraitsType, typename LeftCallable, typename RightCallable>
    struct and_callables;
    template <Traits TraitsType, typename LeftCallable, typename RightCallable>
    struct or_callables;
    template <Traits TraitsType, typename... Callables>
    struct forward_callables;
    template <Traits TraitsType, typename... CallableSegments>
    struct segment_callables;
    template <Traits TraitsType, typename MemPtr>
    struct member_function_callable;


    namespace details {

        template <typename C>
        struct route_optimizer {
            using type = C;
        };


        // move pre-routing hooks to the left because they supposed to be called before everything
        template <template <typename, typename, typename> typename CallableTemplate,
                  typename Left,
                  typename Pre,
                  typename Right,
                  typename TraitsType>
        struct route_optimizer<CallableTemplate<TraitsType, Left, pre_route<TraitsType, Pre, Right>>>
          : route_optimizer<CallableTemplate<TraitsType,
                                             pre_route<TraitsType,
                                                       typename route_optimizer<Pre>::type,
                                                       typename route_optimizer<Left>::type>,
                                             typename route_optimizer<Right>::type>> {};

        // move post-routing hooks to the right because they supposed to be called after everything
        template <template <typename, typename, typename> typename CallableTemplate,
                  typename Left,
                  typename Post,
                  typename Right,
                  typename TraitsType>
        struct route_optimizer<CallableTemplate<TraitsType, post_route<TraitsType, Left, Post>, Right>>
          : route_optimizer<CallableTemplate<TraitsType,
                                             typename route_optimizer<Left>::type,
                                             post_route<TraitsType,
                                                        typename route_optimizer<Right>::type,
                                                        typename route_optimizer<Post>::type>>> {};

        // !!C == C
        template <typename C, typename TraitsType>
        struct route_optimizer<not_callable<TraitsType, not_callable<TraitsType, C>>> : route_optimizer<C> {};

        // C || !C == +C
        template <typename C, typename TraitsType>
        struct route_optimizer<or_callables<TraitsType, C, not_callable<TraitsType, C>>>
          : route_optimizer<positive_callable<TraitsType, C>> {};

        // !C || C == +C
        template <typename C, typename TraitsType>
        struct route_optimizer<or_callables<TraitsType, not_callable<TraitsType, C>, C>>
          : route_optimizer<positive_callable<TraitsType, C>> {};

        // C && C == C
        template <typename C, typename TraitsType>
        struct route_optimizer<and_callables<TraitsType, C, C>> : route_optimizer<C> {};

        // C && !C == -C
        template <typename C, typename TraitsType>
        struct route_optimizer<and_callables<TraitsType, C, not_callable<TraitsType, C>>>
          : route_optimizer<negative_callable<TraitsType, C>> {};

        // !C && C == -C
        template <typename C, typename TraitsType>
        struct route_optimizer<and_callables<TraitsType, not_callable<TraitsType, C>, C>>
          : route_optimizer<negative_callable<TraitsType, C>> {};

        // remove first voids
        template <typename... C, typename TraitsType>
        struct route_optimizer<forward_callables<TraitsType, void, C...>>
          : route_optimizer<forward_callables<TraitsType, C...>> {};

        // remove double forwarding
        template <typename C1, typename... C, typename TraitsType>
        struct route_optimizer<forward_callables<TraitsType, forward_callables<TraitsType, C1>, C...>>
          : route_optimizer<forward_callables<TraitsType, C1, C...>> {};

        // remove double segmenting
        template <typename... C1s, typename... C, typename TraitsType>
        struct route_optimizer<segment_callables<TraitsType, segment_callables<TraitsType, C1s...>, C...>>
          : route_optimizer<segment_callables<TraitsType, C1s..., C...>> {};


        // C || Positive == ?
        // C && Negative == ?


        template <typename C>
        using route_optimizer_t = typename route_optimizer<C>::type;





        template <Traits TraitsType, typename Self>
        struct route_root {
            using traits_type   = TraitsType;
            using context_type  = basic_context<traits_type>;
            using response_type = basic_response<traits_type>;
            using request_type  = basic_request<traits_type>;



#define WEBPP_DEFINE_FUNC_BASE(Ret, ...)                                                                  \
                                                                                                          \
    template <typename T>                                                                                 \
    [[nodiscard]] constexpr auto operator>>(Ret (T::*inp_func)(__VA_ARGS__)) const {                      \
        using mem_ptr_type = Ret (T::*)(__VA_ARGS__);                                                     \
        if constexpr (stl::is_void_v<Self>) {                                                             \
            return forward_callables<traits_type, member_function_callable<traits_type, mem_ptr_type>>{   \
              inp_func};                                                                                  \
        } else {                                                                                          \
            return route_optimizer_t<                                                                     \
              forward_callables<traits_type, Self, member_function_callable<traits_type, mem_ptr_type>>>{ \
              *static_cast<Self const*>(this),                                                            \
              inp_func};                                                                                  \
        }                                                                                                 \
    }                                                                                                     \
                                                                                                          \
    template <typename T>                                                                                 \
    [[nodiscard]] constexpr auto operator>>(Ret (T::*inp_func)(__VA_ARGS__) const) const {                \
        using mem_ptr_type = Ret (T::*)(__VA_ARGS__) const;                                               \
        if constexpr (stl::is_void_v<Self>) {                                                             \
            return forward_callables<traits_type, member_function_callable<traits_type, mem_ptr_type>>{   \
              inp_func};                                                                                  \
        } else {                                                                                          \
            return route_optimizer_t<                                                                     \
              forward_callables<traits_type, Self, member_function_callable<traits_type, mem_ptr_type>>>{ \
              *static_cast<Self const*>(this),                                                            \
              inp_func};                                                                                  \
        }                                                                                                 \
    }

#define WEBPP_DEFINE_FUNC(...)                                \
    WEBPP_DEFINE_FUNC_BASE(response_type, __VA_ARGS__)        \
    WEBPP_DEFINE_FUNC_BASE(response_type const&, __VA_ARGS__) \
    WEBPP_DEFINE_FUNC_BASE(response_type&, __VA_ARGS__)       \
    WEBPP_DEFINE_FUNC_BASE(void, __VA_ARGS__)                 \
    WEBPP_DEFINE_FUNC_BASE(context_type, __VA_ARGS__)         \
    WEBPP_DEFINE_FUNC_BASE(context_type&, __VA_ARGS__)        \
    WEBPP_DEFINE_FUNC_BASE(context_type const&, __VA_ARGS__)


            WEBPP_DEFINE_FUNC(request_type)
            WEBPP_DEFINE_FUNC(request_type&)
            WEBPP_DEFINE_FUNC(request_type const&)
            WEBPP_DEFINE_FUNC(request_type&&)
            WEBPP_DEFINE_FUNC(request_type, response_type)
            WEBPP_DEFINE_FUNC(request_type&, response_type)
            WEBPP_DEFINE_FUNC(request_type const&, response_type)
            WEBPP_DEFINE_FUNC(request_type&&, response_type)
            WEBPP_DEFINE_FUNC(request_type, response_type&)
            WEBPP_DEFINE_FUNC(request_type&, response_type&)
            WEBPP_DEFINE_FUNC(request_type const&, response_type&)
            WEBPP_DEFINE_FUNC(request_type&&, response_type&)
            WEBPP_DEFINE_FUNC(request_type, response_type const&)
            WEBPP_DEFINE_FUNC(request_type&, response_type const&)
            WEBPP_DEFINE_FUNC(request_type const&, response_type const&)
            WEBPP_DEFINE_FUNC(request_type&&, response_type const&)
            WEBPP_DEFINE_FUNC(request_type, context_type)
            WEBPP_DEFINE_FUNC(request_type&, context_type)
            WEBPP_DEFINE_FUNC(request_type const&, context_type)
            WEBPP_DEFINE_FUNC(request_type&&, context_type)
            WEBPP_DEFINE_FUNC(request_type, context_type&)
            WEBPP_DEFINE_FUNC(request_type&, context_type&)
            WEBPP_DEFINE_FUNC(request_type const&, context_type&)
            WEBPP_DEFINE_FUNC(request_type&&, context_type&)
            WEBPP_DEFINE_FUNC(request_type, context_type const&)
            WEBPP_DEFINE_FUNC(request_type&, context_type const&)
            WEBPP_DEFINE_FUNC(request_type const&, context_type const&)
            WEBPP_DEFINE_FUNC(request_type&&, context_type const&)


            WEBPP_DEFINE_FUNC(context_type&)
            WEBPP_DEFINE_FUNC(context_type)
            WEBPP_DEFINE_FUNC(context_type const&)
            WEBPP_DEFINE_FUNC(context_type&, response_type)
            WEBPP_DEFINE_FUNC(context_type, response_type)
            WEBPP_DEFINE_FUNC(context_type const&, response_type)
            WEBPP_DEFINE_FUNC(context_type&, response_type&)
            WEBPP_DEFINE_FUNC(context_type, response_type&)
            WEBPP_DEFINE_FUNC(context_type const&, response_type&)
            WEBPP_DEFINE_FUNC(context_type&, response_type const&)
            WEBPP_DEFINE_FUNC(context_type, response_type const&)
            WEBPP_DEFINE_FUNC(context_type const&, response_type const&)
            WEBPP_DEFINE_FUNC(context_type&, request_type)
            WEBPP_DEFINE_FUNC(context_type, request_type)
            WEBPP_DEFINE_FUNC(context_type const&, request_type)
            WEBPP_DEFINE_FUNC(context_type&, request_type&)
            WEBPP_DEFINE_FUNC(context_type, request_type&)
            WEBPP_DEFINE_FUNC(context_type const&, request_type&)
            WEBPP_DEFINE_FUNC(context_type&, request_type const&)
            WEBPP_DEFINE_FUNC(context_type, request_type const&)
            WEBPP_DEFINE_FUNC(context_type const&, request_type const&)


#undef WEBPP_DEFINE_FUNC
#undef WEBPP_DEFINE_FUNC_BASE


            template <typename Callable>
            [[nodiscard]] constexpr auto operator>>(Callable&& callable) const {
                if constexpr (stl::is_void_v<Self>) {
                    return forward_callables<traits_type, stl::remove_cvref_t<Callable>>{
                      stl::forward<Callable>(callable)};
                } else {
                    return route_optimizer_t<
                      forward_callables<traits_type, Self, stl::remove_cvref_t<Callable>>>{
                      *static_cast<Self const*>(this), // route 1
                      stl::forward<Callable>(callable) // route 2
                    };
                }
            }

            template <typename Callable>
            [[nodiscard]] constexpr auto operator&&(Callable&& callable) const {
                if constexpr (stl::is_void_v<Self>) {
                    return operator>>(stl::forward<Callable>(callable));
                } else {
                    return route_optimizer_t<and_callables<traits_type, Self, stl::remove_cvref_t<Callable>>>{
                      *static_cast<Self const*>(this), // left
                      stl::forward<Callable>(callable) // right
                    };
                }
            }


            template <typename Callable>
            [[nodiscard]] constexpr auto operator||(Callable&& callable) const {
                if constexpr (stl::is_void_v<Self>) {
                    return operator>>(stl::forward<Callable>(callable));
                } else {
                    return route_optimizer_t<or_callables<traits_type, Self, stl::remove_cvref_t<Callable>>>{
                      *static_cast<Self const*>(this), // left
                      stl::forward<Callable>(callable) // right
                    };
                }
            }


            template <typename Callable>
            [[nodiscard]] constexpr auto operator!() const {
                if constexpr (stl::is_void_v<Self>) {
                    return *this;
                } else {
                    return route_optimizer_t<not_callable<traits_type, Self>>{
                      *static_cast<Self const*>(this)};
                }
            }


            template <typename Callable>
            [[nodiscard]] constexpr auto operator+(Callable&& callable) const {
                if constexpr (stl::is_void_v<Self>) {
                    return operator>>(stl::forward<Callable>(callable));
                } else {
                    return route_optimizer_t<post_route<traits_type, Self, stl::remove_cvref_t<Callable>>>{
                      *static_cast<Self const*>(this), // self
                      stl::forward<Callable>(callable) // post routing callable
                    };
                }
            }


            template <typename Callable>
            [[nodiscard]] constexpr auto operator-(Callable&& callable) const {
                if constexpr (stl::is_void_v<Self>) {
                    return operator>>(stl::forward<Callable>(callable));
                } else {
                    return route_optimizer_t<pre_route<traits_type, stl::remove_cvref_t<Callable>, Self>>{
                      stl::forward<Callable>(callable), // pre routing callable
                      *static_cast<Self const*>(this)   // self
                    };
                }
            }



            template <typename CallableSegment>
            [[nodiscard]] constexpr auto operator/(CallableSegment&& inp_segment) const {
                if constexpr (stl::is_void_v<Self>) {
                    return segment_callables<traits_type, stl::remove_cvref_t<CallableSegment>>{
                      stl::forward<CallableSegment>(inp_segment)};
                } else {
                    return route_optimizer_t<
                      segment_callables<traits_type, Self, stl::remove_cvref_t<CallableSegment>>>{
                      *static_cast<Self const*>(this),           // first segment
                      stl::forward<CallableSegment>(inp_segment) // second segment
                    };
                }
            }


            template <typename SegT>
                requires(istl::StringView<SegT> || stl::is_array_v<stl::remove_cvref_t<SegT>>)
            [[nodiscard]] constexpr auto operator/(SegT&& inp_segment) const {
                auto const seg_v = istl::string_viewify(stl::forward<SegT>(inp_segment));
                using seg_t      = stl::remove_cvref_t<decltype(seg_v)>;
                if constexpr (stl::is_void_v<Self>) {
                    return segment_callables<traits_type, seg_t>{seg_v};
                } else {
                    return route_optimizer_t<segment_callables<traits_type, Self, seg_t>>{
                      *static_cast<Self const*>(this), // first segment
                      seg_v                            // second segment
                    };
                }
            }
        };


    } // namespace details


    template <Traits TraitsType, typename... Callables>
    struct forward_callables : details::route_root<TraitsType, forward_callables<TraitsType, Callables...>> {
        using tuple_type = stl::tuple<Callables...>;

        using context_type = basic_context<TraitsType>;

      private:
        tuple_type callables;

      public:
        constexpr forward_callables(forward_callables const&)                     = default;
        constexpr forward_callables(forward_callables&&) noexcept                 = default;
        constexpr forward_callables& operator=(forward_callables&&) noexcept      = default;
        constexpr forward_callables& operator=(forward_callables const&) noexcept = default;
        constexpr ~forward_callables()                                            = default;

        template <typename C1, typename... Cs>
        constexpr forward_callables(forward_callables<C1> const& c1, Cs&&... funcs) noexcept(
          stl::is_nothrow_constructible_v<tuple_type, C1, Cs...>)
          : forward_callables{c1.callable, stl::forward<Cs>(funcs)...} {}

        template <typename... Cs>
        constexpr forward_callables(Cs&&... funcs) noexcept(
          stl::is_nothrow_constructible_v<tuple_type, Cs...>)
          : callables{stl::forward<Cs>(funcs)...} {}

        constexpr void operator()(basic_context<TraitsType>& ctx) {
            stl::apply(
              [&ctx]<typename... T>(T&&... funcs) constexpr {
                  (route_traits<T, context_type>::call_and_set(stl::forward<T>(funcs), ctx), ...);
              },
              callables);
        }

        constexpr void to_string(istl::String auto& out) const {
            stl::apply(
              [&out]<typename... T>(T&&... funcs) constexpr {
                  ((out.append(" >>"), route_to_string(out, funcs)), ...);
              },
              callables);
        }
    };

    template <Traits TraitsType, typename Callable>
    struct forward_callables<TraitsType, Callable>
      : details::route_root<TraitsType, forward_callables<TraitsType, Callable>> {
      private:
        Callable callable;

      public:
        constexpr forward_callables(Callable inp_callable) : callable{stl::move(inp_callable)} {}
        constexpr forward_callables(forward_callables const&)                     = default;
        constexpr forward_callables(forward_callables&&) noexcept                 = default;
        constexpr forward_callables& operator=(forward_callables&&) noexcept      = default;
        constexpr forward_callables& operator=(forward_callables const&) noexcept = default;
        constexpr ~forward_callables()                                            = default;


        constexpr void operator()(basic_context<TraitsType>& ctx) {
            using context_type    = basic_context<TraitsType>;
            using callable_traits = route_traits<Callable, context_type>;

            callable_traits::set_response(callable_traits::call(callable, ctx), ctx);
        }


        constexpr void to_string(istl::String auto& out) const {
            out.append(" >>");
            route_to_string(out, callable);
        }
    };


    template <Traits TraitsType>
    struct forward_callables<TraitsType> : details::route_root<TraitsType, void> {
        constexpr void operator()(basic_context<TraitsType>&) const {}

        constexpr void to_string(istl::String auto& out) const {
            out.append(" >> [empty]");
        }
    };


    template <Traits TraitsType>
    struct forward_callables<TraitsType, void> : forward_callables<TraitsType> {};

    template <Traits TraitsType, typename PreRoute, typename Callable>
    struct pre_route : details::route_root<TraitsType, pre_route<TraitsType, PreRoute, Callable>> {
      private:
        PreRoute pre;
        Callable callable;

      public:
        constexpr pre_route(PreRoute inp_pre, Callable inp_callable)
          : pre{stl::move(inp_pre)},
            callable{stl::move(inp_callable)} {}
        constexpr pre_route(pre_route const&)                     = default;
        constexpr pre_route(pre_route&&) noexcept                 = default;
        constexpr pre_route& operator=(pre_route&&) noexcept      = default;
        constexpr pre_route& operator=(pre_route const&) noexcept = default;
        constexpr ~pre_route()                                    = default;


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


        constexpr void to_string(istl::String auto& out) const {
            out.append(" -");
            route_to_string(out, pre);
            out.append(" >>");
            route_to_string(out, callable);
        }
    };

    template <Traits TraitsType, typename PreRoute>
    struct pre_route<TraitsType, PreRoute, void>
      : details::route_root<TraitsType, pre_route<TraitsType, PreRoute, void>> {
      private:
        PreRoute pre;

      public:
        constexpr pre_route(PreRoute inp_pre) : pre{stl::move(inp_pre)} {}
        constexpr pre_route(pre_route const&)                     = default;
        constexpr pre_route(pre_route&&) noexcept                 = default;
        constexpr pre_route& operator=(pre_route&&) noexcept      = default;
        constexpr pre_route& operator=(pre_route const&) noexcept = default;
        constexpr ~pre_route()                                    = default;

        constexpr void operator()(basic_context<TraitsType>& ctx) {
            using context_type = basic_context<TraitsType>;
            using pre_traits   = route_traits<PreRoute, context_type>;

            pre_traits::call_and_set(pre, ctx);
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" -");
            route_to_string(out, pre);
        }
    };


    template <Traits TraitsType, typename Callable, typename PostRoute>
    struct post_route : details::route_root<TraitsType, post_route<TraitsType, Callable, PostRoute>> {
      private:
        Callable  callable;
        PostRoute post;

      public:
        constexpr post_route(Callable inp_callable, PostRoute inp_post)
          : callable{stl::move(inp_callable)},
            post{stl::move(inp_post)} {}
        constexpr post_route(post_route const&)                     = default;
        constexpr post_route(post_route&&) noexcept                 = default;
        constexpr post_route& operator=(post_route&&) noexcept      = default;
        constexpr post_route& operator=(post_route const&) noexcept = default;
        constexpr ~post_route()                                     = default;


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


        constexpr void to_string(istl::String auto& out) const {
            out.append(" >>");
            route_to_string(out, callable);
            out.append(" +");
            route_to_string(out, post);
        }
    };

    template <Traits TraitsType, typename PostRoute>
    struct post_route<TraitsType, void, PostRoute>
      : details::route_root<TraitsType, post_route<TraitsType, void, PostRoute>> {
      private:
        PostRoute post;

      public:
        constexpr post_route(PostRoute inp_post) : post{stl::move(inp_post)} {}
        constexpr post_route(post_route const&)                     = default;
        constexpr post_route(post_route&&) noexcept                 = default;
        constexpr post_route& operator=(post_route&&) noexcept      = default;
        constexpr post_route& operator=(post_route const&) noexcept = default;
        constexpr ~post_route()                                     = default;


        constexpr void operator()(basic_context<TraitsType>& ctx) {
            using context_type = basic_context<TraitsType>;
            using post_traits  = route_traits<PostRoute, context_type>;

            post_traits::call_and_set(post, ctx);
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" +");
            route_to_string(out, post);
        }
    };


    template <Traits TraitsType, typename Callable>
    struct not_callable : details::route_root<TraitsType, not_callable<TraitsType, Callable>> {
        using callable_type = Callable;

      private:
        callable_type callable;

      public:
        constexpr not_callable(Callable inp_callable) : callable{stl::move(inp_callable)} {}
        constexpr not_callable(not_callable const&)                     = default;
        constexpr not_callable(not_callable&&) noexcept                 = default;
        constexpr not_callable& operator=(not_callable&&) noexcept      = default;
        constexpr not_callable& operator=(not_callable const&) noexcept = default;
        constexpr ~not_callable()                                       = default;


        constexpr void operator()(basic_context<TraitsType>& ctx) {
            using context_type = basic_context<TraitsType>;
            using ctraits      = route_traits<callable_type, context_type>;

            auto res = ctraits::call(callable, ctx);
            if (!ctraits::is_positive(res)) {
                ctraits::set_response(stl::move(res), ctx);
            }
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" !(");
            route_to_string(out, callable);
            out.append(")");
        }
    };

    template <Traits TraitsType>
    struct not_callable<TraitsType, void> : details::route_root<TraitsType, not_callable<TraitsType, void>> {
        constexpr void operator()(basic_context<TraitsType>&) const noexcept {}

        constexpr void to_string(istl::String auto& out) const {
            out.append(" !([empty])");
        }
    };

    template <Traits TraitsType, typename Callable>
    struct negative_callable : details::route_root<TraitsType, negative_callable<TraitsType, Callable>> {
        using callable_type = Callable;

      private:
        callable_type callable;

      public:
        constexpr negative_callable(Callable inp_callable) : callable{stl::move(inp_callable)} {}
        constexpr negative_callable(negative_callable const&)                     = default;
        constexpr negative_callable(negative_callable&&) noexcept                 = default;
        constexpr negative_callable& operator=(negative_callable&&) noexcept      = default;
        constexpr negative_callable& operator=(negative_callable const&) noexcept = default;
        constexpr ~negative_callable()                                            = default;


        constexpr bool operator()(basic_context<TraitsType>& ctx) {
            using context_type = basic_context<TraitsType>;
            using ctraits      = route_traits<callable_type, context_type>;

            ctraits::call_and_set(callable, ctx);
            return false;
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" false(");
            route_to_string(out, callable);
            out.append(")");
        }
    };

    template <Traits TraitsType, typename Callable>
    struct positive_callable : details::route_root<TraitsType, positive_callable<TraitsType, Callable>> {
        using callable_type = Callable;

      private:
        callable_type callable;

      public:
        constexpr positive_callable(Callable inp_callable) : callable{stl::move(inp_callable)} {}
        constexpr positive_callable(positive_callable const&)                     = default;
        constexpr positive_callable(positive_callable&&) noexcept                 = default;
        constexpr positive_callable& operator=(positive_callable&&) noexcept      = default;
        constexpr positive_callable& operator=(positive_callable const&) noexcept = default;
        constexpr ~positive_callable()                                            = default;


        constexpr bool operator()(basic_context<TraitsType>& ctx) {
            using context_type = basic_context<TraitsType>;
            using ctraits      = route_traits<callable_type, context_type>;

            ctraits::call_and_set(callable, ctx);
            return true;
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" true(");
            route_to_string(out, callable);
            out.append(")");
        }
    };



    template <Traits TraitsType, typename LeftCallable, typename RightCallable>
    struct and_callables
      : details::route_root<TraitsType, and_callables<TraitsType, LeftCallable, RightCallable>> {
        using left_type  = LeftCallable;
        using right_type = RightCallable;

      private:
        left_type  lhs;
        right_type rhs;

      public:
        constexpr and_callables(left_type&& inp_lhs, right_type&& inp_rhs) noexcept
          : lhs{stl::move(inp_lhs)},
            rhs{stl::move(inp_rhs)} {}
        constexpr and_callables(left_type const& inp_lhs, right_type&& inp_rhs) noexcept
          : lhs{inp_lhs},
            rhs{stl::move(inp_rhs)} {}
        constexpr and_callables(left_type const& inp_lhs, right_type const& inp_rhs) noexcept
          : lhs{inp_lhs},
            rhs{inp_rhs} {}
        constexpr and_callables(left_type&& inp_lhs, right_type const& inp_rhs) noexcept
          : lhs{stl::move(inp_lhs)},
            rhs{inp_rhs} {}
        constexpr and_callables(and_callables const&) noexcept            = default;
        constexpr and_callables(and_callables&&) noexcept                 = default;
        constexpr and_callables& operator=(and_callables const&) noexcept = default;
        constexpr and_callables& operator=(and_callables&&) noexcept      = default;
        constexpr ~and_callables()                                        = default;

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


        constexpr void to_string(istl::String auto& out) const {
            out.append(" (");
            route_to_string(out, lhs);
            out.append(" && ");
            route_to_string(out, rhs);
            out.append(")");
        }
    };

    template <Traits TraitsType, typename RightCallable>
    struct and_callables<TraitsType, void, RightCallable> : forward_callables<TraitsType, RightCallable> {};


    template <Traits TraitsType, typename LeftCallable, typename RightCallable>
    struct or_callables
      : details::route_root<TraitsType, or_callables<TraitsType, LeftCallable, RightCallable>> {
        using left_type  = LeftCallable;
        using right_type = RightCallable;

      private:
        left_type  lhs;
        right_type rhs;

      public:
        constexpr or_callables(left_type&& inp_lhs, right_type&& inp_rhs) noexcept
          : lhs{stl::move(inp_lhs)},
            rhs{stl::move(inp_rhs)} {}
        constexpr or_callables(left_type const& inp_lhs, right_type&& inp_rhs) noexcept
          : lhs{inp_lhs},
            rhs{stl::move(inp_rhs)} {}
        constexpr or_callables(left_type const& inp_lhs, right_type const& inp_rhs) noexcept
          : lhs{inp_lhs},
            rhs{inp_rhs} {}
        constexpr or_callables(left_type&& inp_lhs, right_type const& inp_rhs) noexcept
          : lhs{stl::move(inp_lhs)},
            rhs{inp_rhs} {}
        constexpr or_callables(or_callables const&) noexcept            = default;
        constexpr or_callables(or_callables&&) noexcept                 = default;
        constexpr or_callables& operator=(or_callables const&) noexcept = default;
        constexpr or_callables& operator=(or_callables&&) noexcept      = default;
        constexpr ~or_callables()                                       = default;

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

        constexpr void to_string(istl::String auto& out) const {
            out.append(" (");
            route_to_string(out, lhs);
            out.append(" || ");
            route_to_string(out, rhs);
            out.append(")");
        }
    };

    template <Traits TraitsType, typename RightCallable>
    struct or_callables<TraitsType, void, RightCallable> : forward_callables<TraitsType, RightCallable> {};


    template <Traits TraitsType, typename... CallableSegments>
    struct segment_callables
      : details::route_root<TraitsType, segment_callables<TraitsType, CallableSegments...>> {
        using tuple_type = stl::tuple<CallableSegments...>;

        using context_type = basic_context<TraitsType>;

      private:
        tuple_type segments;

      public:
        constexpr segment_callables(segment_callables const&)                     = default;
        constexpr segment_callables(segment_callables&&) noexcept                 = default;
        constexpr segment_callables& operator=(segment_callables&&) noexcept      = default;
        constexpr segment_callables& operator=(segment_callables const&) noexcept = default;
        constexpr ~segment_callables()                                            = default;

        template <typename C1, typename... Cs>
        constexpr segment_callables(segment_callables<TraitsType, C1> const& c1, Cs&&... segs) noexcept(
          stl::is_nothrow_constructible_v<tuple_type, C1, Cs...>)
          : segments{c1.get_segment(), stl::forward<Cs>(segs)...} {}

        template <typename... Cs>
        constexpr segment_callables(Cs&&... segs) noexcept(stl::is_nothrow_constructible_v<tuple_type, Cs...>)
          : segments{stl::forward<Cs>(segs)...} {}


        constexpr void operator()(basic_context<TraitsType>& ctx) {
            stl::apply(
              [&ctx]<typename... T>(T&&... callables) constexpr {
                  (route_traits<T, context_type>::call_set_get(stl::forward<T>(callables), ctx) && ...);
              },
              segments);
        }


        constexpr void to_string(istl::String auto& out) const {
            stl::apply(
              [&out]<typename... T>(T&&... callables) constexpr {
                  ((out.append(" /"), route_to_string(out, callables)), ...);
              },
              segments);
        }
    };


    template <Traits TraitsType>
    struct segment_callables<TraitsType> : details::route_root<TraitsType, segment_callables<TraitsType>> {
        constexpr void operator()(basic_context<TraitsType>&) const noexcept {}

        constexpr void to_string(istl::String auto& out) const {
            out.append(" / [empty]");
        }
    };

    template <Traits TraitsType>
    struct segment_callables<TraitsType, void> : segment_callables<TraitsType> {};


    template <Traits TraitsType, typename CallableSegment>
    struct segment_callables<TraitsType, CallableSegment>
      : details::route_root<TraitsType, segment_callables<TraitsType, CallableSegment>> {
      private:
        CallableSegment segment;

      public:
        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        template <typename C>
            requires(!istl::same_as_cvref<C, segment_callables>)
        constexpr segment_callables(C&& seg) noexcept(stl::is_nothrow_constructible_v<CallableSegment, C>)
          : segment{stl::forward<C>(seg)} {}
        // NOLINTEND(bugprone-forwarding-reference-overload)

        constexpr segment_callables(segment_callables const&)                     = default;
        constexpr segment_callables(segment_callables&&) noexcept                 = default;
        constexpr segment_callables& operator=(segment_callables&&) noexcept      = default;
        constexpr segment_callables& operator=(segment_callables const&) noexcept = default;
        constexpr ~segment_callables()                                            = default;

        constexpr CallableSegment const& get_segment() const noexcept {
            return segment;
        }

        constexpr void operator()(basic_context<TraitsType>& ctx) {
            using context_type   = basic_context<TraitsType>;
            using segment_traits = route_traits<CallableSegment, context_type>;

            segment_traits::set_response(segment_traits::call(segment, ctx), ctx);
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" /");
            route_to_string(out, segment);
        }
    };


    template <Traits TraitsType, typename MemPtr>
    struct member_function_callable
      : details::route_root<TraitsType, member_function_callable<TraitsType, MemPtr>> {
        using member_ptr_type = MemPtr;
        using traits_type     = TraitsType;
        using context_type    = basic_context<traits_type>;
        using mem_ref_type    = istl::member_function_pointer_traits<member_ptr_type>;
        using response_type   = basic_response<traits_type>;
        using request_type    = basic_request<traits_type>;
        using object_type     = typename mem_ref_type::type;
        using object_ptr      = object_type*;

      private:
        member_ptr_type mem_ptr;
        object_ptr      obj = nullptr;

      public:
        constexpr member_function_callable(member_function_callable const&) noexcept            = default;
        constexpr member_function_callable(member_function_callable&&) noexcept                 = default;
        constexpr member_function_callable& operator=(member_function_callable const&) noexcept = default;
        constexpr member_function_callable& operator=(member_function_callable&&) noexcept      = default;
        constexpr ~member_function_callable() noexcept                                          = default;

        constexpr member_function_callable(mem_ref_type inp_func) noexcept : mem_ptr{inp_func} {}

        constexpr void operator()(context_type& ctx) {
            using callable_traits = route_traits<member_function_callable, context_type>;
            callable_traits::set_response(callable_traits::call(obj->*mem_ptr, ctx), ctx);
        }

        constexpr void set_object(object_ptr inp_obj) noexcept {
            obj = inp_obj;
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" >>");
            route_to_string(out, mem_ptr);
        }
    };



    /**
     * @brief A Router that's is fully customizable at runtime
     *
     * This class will be used directly by the developers using this whole library. So be nice and careful
     * and user friendly.
     */
    template <ExtensionList RootExtensions, EnabledTraits TraitsEnabler>
    struct basic_dynamic_router : TraitsEnabler,
                                  details::route_root<typename TraitsEnabler::traits_type, void> {
        using root_extensions    = RootExtensions;
        using etraits            = TraitsEnabler;
        using traits_type        = typename etraits::traits_type;
        using non_owner_etraits  = typename etraits::non_owner_type;
        using route_type         = dynamic_route<traits_type>;
        using dynamic_route_type = istl::dynamic<route_type, traits::general_allocator<traits_type>>;
        using vector_allocator   = traits::general_allocator<traits_type, dynamic_route_type>;
        using string_type        = traits::general_string<traits_type>;
        using string_view_type   = traits::string_view<traits_type>;
        using objects_type       = stl::vector<stl::any, traits::general_allocator<traits_type, stl::any>>;
        using routes_type        = stl::vector<dynamic_route_type, vector_allocator>;
        using response_type      = basic_response<traits_type>;
        using context_type       = basic_context<traits_type>;
        using request_type       = basic_request<traits_type>;

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

        // we're not adding context and response here in router scope because we want the user to be able
        // to take advantage of parallelism


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
            using method_type = istl::member_function_pointer_traits<stl::remove_cvref_t<T>>;
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
            using method_type = istl::member_function_pointer_traits<stl::remove_cvref_t<T>>;
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
            }
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
            using callable_type  = details::route_optimizer_t<C>;
            using new_route_type = dynamic_route<traits_type, callable_type>;

            routes.emplace_back(new_route_type{stl::forward<C>(callable)});
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
            this->operator()(ctx);

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
        constexpr void operator()(context_type& ctx) {
            for (auto& route : routes) {
                route->operator()(ctx);
                if (!continue_routing(ctx)) {
                    return;
                }
            }
        }

        constexpr void to_string(istl::String auto& out) const {
            for (auto& route : routes) {
                route->to_string(out);
                out.append("\n");
            }
        }


        template <istl::String StrT = string_type>
        constexpr StrT to_string() {
            StrT out{alloc::general_alloc_for<StrT>(*this)};
            to_string(out);
            return out;
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
