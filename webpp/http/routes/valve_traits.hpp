// Created by moisrex on 4/3/23.

#ifndef WEBPP_VALVE_TRAITS_HPP
#define WEBPP_VALVE_TRAITS_HPP

#include "../../std/function_ref.hpp"
#include "../../std/string.hpp"
#include "../../std/string_view.hpp"
#include "../../std/tag_invoke.hpp"
#include "../../std/types.hpp"
#include "../../traits/default_traits.hpp"
#include "../http_concepts.hpp"
#include "../status_code.hpp"
#include "router_concepts.hpp"

namespace webpp::http {


    template <typename TraitsType>
    using basic_next_route = istl::member_function_ref<void(basic_context<TraitsType>&)>;

    using next_route = basic_next_route<default_dynamic_traits>;

    /**
     * Checks if the type is a valve type.
     */
    template <typename TraitsType, typename T>
    concept ValveOf = Traits<TraitsType> &&
                      (stl::is_invocable_v<T, basic_context<TraitsType>&> || istl::StringViewifiable<T>);

    /// A single valve
    template <typename T>
    concept Valve = ValveOf<default_dynamic_traits, T>;

    template <typename TraitsType, typename T>
    concept Mangler =
      Traits<TraitsType> &&
      stl::is_invocable_v<T,                           // type
                          basic_context<TraitsType>&,  // context
                          basic_next_route<TraitsType> // next valve
                          >;

    template <typename TraitsType, typename T>
    concept RouteSetter = Traits<TraitsType> && stl::is_invocable_v<T, basic_dynamic_router<TraitsType>&>;

    /// Get the string representation of the value and append it to the output
    template <istl::String StrT, typename Callable>
    static constexpr void valve_to_string(StrT& out, Callable& func) {
        using mem_traits = istl::member_function_pointer_traits<Callable>;
        if constexpr (istl::StringViewifiable<Callable>) {
            out.append(" \"");
            out += istl::string_viewify(func);
            out.append("\"");
        } else if constexpr (requires { func.to_string(out); }) {
            func.to_string(out);
        } else if constexpr (mem_traits::value) {
            out.append(" ");
            out.append(istl::type_name<typename mem_traits::type>());
            out.append("::");
            out.append(istl::type_name<Callable>());
        } else {
            out.append(" ");
            StrT name{istl::type_name<Callable>(), out.get_allocator()};
            istl::replace_all(name, "webpp::http::", "");
            istl::replace_all(name, "webpp::", "");
            out.append(name);
        }
    }

    /// General Valvifier Tag, and its default implementation
    /// We use this Customization Point Object to let the users customize their types that they
    /// use as "valves", hence the name "valvify", because you're converting your random object
    /// type into a valve-compliant object.
    inline constexpr struct valvify_tag {
        /// Customization Point
        template <typename T>
            requires stl::tag_invocable<valvify_tag, T>
        [[nodiscard]] constexpr stl::tag_invoke_result_t<valvify_tag, T> operator()(T&& next) const
          noexcept(stl::nothrow_tag_invocable<valvify_tag, T>) {
            return stl::tag_invoke(*this, stl::forward<T>(next));
        }

        /// default impl
        template <typename T>
        [[nodiscard]] friend constexpr decltype(auto) tag_invoke(valvify_tag, T&& next) noexcept {
            return stl::forward<T>(next);
        }
    } valvify;

    /// get the type of the valvified value
    /// We remove constness and references of the returned type
    /// because it'll be the root of a load of evil things if we don't.
    /// Use Pointers if you need lvalue references, we know how to call pointers.
    template <typename T>
    using valvified_type = stl::remove_cvref_t<stl::tag_invoke_result_t<valvify_tag, T>>;

    template <typename T>
    concept Valvifiable = stl::tag_invocable<valvify_tag, T>;

    /**
     * The valve traits
     * Central place for dealing with all valves
     */
    template <typename Callable, typename ContextType = basic_context<default_dynamic_traits>>
    struct valve_traits {
        using callable_type      = stl::remove_cvref_t<stl::remove_pointer_t<stl::remove_cvref_t<Callable>>>;
        using context_type       = ContextType;
        using request_type       = typename context_type::request_type;
        using response_type      = typename context_type::response_type;
        using response_body_type = typename response_type::body_type;
        using invocable_inorder_type =
          istl::invocable_inorder<callable_type, context_type&, request_type&, response_type&>;
        using traits_type = typename context_type::traits_type;
        using return_type = stl::remove_cvref_t<typename invocable_inorder_type::result>;

        static constexpr bool is_nothrow = invocable_inorder_type::is_nothrow;

        /// Call the valve with the specified context
        template <istl::cvref_as<Callable> C>
            requires(!stl::is_pointer_v<stl::remove_cvref_t<C>> && invocable_inorder_type::value)
        static constexpr return_type call(C&& callable, context_type& ctx) noexcept(is_nothrow) {
            return istl::invoke_inorder(stl::forward<C>(callable), ctx, ctx.request, ctx.response);
        }

        /// Call the valve with the specified context
        template <istl::cvref_as<callable_type> C>
            requires(invocable_inorder_type::value)
        static constexpr return_type call(C* callable, context_type& ctx) noexcept(is_nothrow) {
            return istl::invoke_inorder(*callable, ctx, ctx.request, ctx.response);
        }

        /// Should we consider the return type as a positive value or a false value,
        /// You can use this information in your routing decisions
        template <typename R>
        static constexpr bool is_positive(R const& ret) noexcept {
            using ret_type = stl::remove_cvref_t<R>;
            if constexpr (stl::same_as<ret_type, bool>) {
                return ret;
            } else if constexpr (stl::is_void_v<ret_type>) {
                return true;
            } else if constexpr (stl::is_pointer_v<ret_type>) {
                return ret != nullptr;
            } else if constexpr (istl::Optional<ret_type>) {
                return bool{ret};
            } else {
                // istl::nothing_type, HTTPResponse<ret_type>, ...
                return false;
            }
        }

        /// Should we go ahead and run the next route or not
        // template <typename R>
        // static constexpr bool should_continue(R const& ret) noexcept {
        //     using ret_type = stl::remove_cvref_t<R>;
        //     if constexpr (stl::same_as<ret_type, bool>) {
        //         return ret;
        //     } else {
        //         return true;
        //     }
        // }


        template <typename R>
        static constexpr void set_response(R&& ret, context_type& ctx) {
            using ret_t = stl::remove_cvref_t<R>;
            if constexpr (stl::is_void_v<ret_t> || istl::part_of<ret_t, bool, istl::nothing_type>) {
                // ignore the result
            } else if constexpr (HTTPResponse<ret_t> || HTTPResponseBody<ret_t> ||
                                 HTTPGenerallySerializableBody<ret_t, response_body_type>)
            {
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

        /// Call the valve, and then set the response if there's any
        template <typename C>
            requires istl::cvref_as<C, Callable>
        static constexpr void call_set(C&& segment, context_type& ctx) noexcept(is_nothrow) {
            if constexpr (stl::is_void_v<return_type>) {
                call(stl::forward<C>(segment), ctx);
            } else {
                set_response(call(stl::forward<C>(segment), ctx), ctx);
            }
        }

        /// Call the valve, and then set the response if there's any, and then tell me if we should continue
        template <typename C>
            requires istl::cvref_as<C, Callable>
        [[nodiscard]] static constexpr bool call_set_get(C&& segment, context_type& ctx)
          noexcept(is_nothrow) {
            if constexpr (stl::is_void_v<return_type>) {
                call(stl::forward<C>(segment), ctx);
                return true;
            } else {
                auto       ret = call(stl::forward<C>(segment), ctx);
                bool const res = is_positive(ret);
                set_response(stl::move(ret), ctx);
                return res;
            }
        }

        // template <typename C>
        //     requires istl::cvref_as<C, Callable>
        // static constexpr bool call_then(C&& callable, context_type& ctx) noexcept(is_nothrow) {
        //     if constexpr (stl::is_void_v<return_type>) {
        //         call(stl::forward<C>(callable), ctx);
        //         return true;
        //     } else {
        //         auto       ret = call(stl::forward<C>(callable), ctx);
        //         bool const res = should_continue(ret);
        //         set_response(stl::move(ret), ctx);
        //         return res;
        //     }
        // }
    };

    // Get the valves_group's routes_type
    template <typename T>
    struct routes_type_of_valve {
        using type = T;
    };

    template <typename Pres, typename Posts, typename Manglers, typename Routes>
    struct routes_type_of_valve<valves_group<Pres, Posts, Manglers, Routes>> {
        using type = Routes;
    };

    /**
     * Context Chaining
     * @code
     *   // Calla get_data with ctx, and then call add_layout with ctx;
     *   ctx >> get_data >> add_layout;
     * @endcode
     */
    template <Context CtxT, typename Callable>
    constexpr CtxT& operator>>(CtxT& ctx, Callable&& callable) {
        valve_traits<Callable, CtxT>::call_set(stl::forward<Callable>(callable), ctx);
        return ctx;
    }

    /**
     * Call the valve with the specified context
     * You can use "ctx >> route" as well.
     */
    template <typename Callable, Context CtxT>
    constexpr CtxT& call_route(Callable&& callable, CtxT& ctx) {
        valve_traits<Callable, CtxT>::call_set(stl::forward<Callable>(callable), ctx);
        return ctx;
    }

} // namespace webpp::http

#endif // WEBPP_VALVE_TRAITS_HPP
