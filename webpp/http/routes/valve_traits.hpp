// Created by moisrex on 4/3/23.

#ifndef WEBPP_VALVE_TRAITS_HPP
#define WEBPP_VALVE_TRAITS_HPP

#include "../../std/function_ref.hpp"
#include "../../std/string.hpp"
#include "../../std/string_view.hpp"
#include "../../std/types.hpp"
#include "../../traits/default_traits.hpp"
#include "../http_concepts.hpp"
#include "../status_code.hpp"

namespace webpp::http {

    template <typename Self = void>
    struct valve;
    template <typename Callable>
    struct not_valve;
    template <typename Callable>
    struct negative_valve;
    template <typename Callable>
    struct positive_valve;
    template <typename LeftCallable, typename RightCallable>
    struct and_valve;
    template <typename LeftCallable, typename RightCallable>
    struct or_valve;
    template <typename... Callables>
    struct forward_valve;
    template <typename... PreRoutes>
    struct prerouting_valve;
    template <typename... PostRoutes>
    struct postrouting_valve;
    template <typename... CallableSegments>
    struct segment_valve;
    template <typename MemPtr>
    struct member_function_valve;
    template <typename... ManglerTypes>
    struct mangler_valve;
    template <typename Pres, typename Posts, typename Manglers, typename Routes>
    struct valves_group;


    template <Traits TraitsType>
    struct basic_context;

    template <Traits>
    struct basic_dynamic_router;

    template <typename TraitsType>
    using next_route = istl::function_ref<bool(basic_context<TraitsType>&)>;

    /**
     * Checks if the type is a valve type.
     */
    template <typename TraitsType, typename T>
    concept ValveOf = Traits<TraitsType> &&
                      (stl::is_invocable_v<T, basic_context<TraitsType>&> || istl::StringViewifiable<T>);

    template <typename T>
    concept Valve = ValveOf<default_dynamic_traits, T>;

    template <typename TraitsType, typename T>
    concept Mangler = Traits<TraitsType> && stl::is_invocable_v<T,                          // type
                                                                basic_context<TraitsType>&, // context
                                                                next_route<TraitsType>      // next valve
                                                                >;

    template <typename TraitsType, typename T>
    concept RouteSetter = Traits<TraitsType> && stl::is_invocable_v<T, basic_dynamic_router<TraitsType>&>;

    template <typename Router, typename T>
    concept ValveRequiresSetup = requires(T callable, Router& router) { callable.setup(router); };



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


    // General Valvifier
    template <typename T>
    struct valvify {

        template <istl::cvref_as<T> TT>
        [[nodiscard]] static constexpr auto call(TT&& next) noexcept {
            return stl::forward<TT>(next);
        }
    };

    template <typename T>
    concept Valvifiable = requires(T obj) { valvify<stl::remove_cvref_t<T>>::call(obj); };

    template <typename T>
    [[nodiscard]] static constexpr decltype(auto) valvify_or(T&& next) noexcept {
        return valvify<stl::remove_cvref_t<T>>::call(stl::forward<T>(next));
    }


    template <typename Callable, typename ContextType = basic_context<default_dynamic_traits>>
    struct valve_traits {
        using callable_type      = stl::remove_cvref_t<Callable>;
        using context_type       = ContextType;
        using request_type       = typename context_type::request_type;
        using response_type      = typename context_type::response_type;
        using response_body_type = typename response_type::body_type;
        using invocable_inorder_type =
          istl::invocable_inorder<callable_type, context_type&, request_type&, response_type&>;
        using traits_type = typename context_type::traits_type;
        using return_type = stl::remove_cvref_t<typename invocable_inorder_type::result>;

        template <istl::cvref_as<Callable> C>
            requires(invocable_inorder_type::value)
        static constexpr return_type
          call(C&& callable, context_type& ctx) noexcept(invocable_inorder_type::is_nothrow) {
            return istl::invoke_inorder(callable, ctx, ctx.request, ctx.response);
        }

        template <typename R>
        static constexpr bool is_positive(R&& ret) noexcept {
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

        template <typename R>
        static constexpr bool should_continue(R&& ret) noexcept {
            using ret_type = stl::remove_cvref_t<R>;
            if constexpr (stl::same_as<ret_type, bool>) {
                return ret;
            } else {
                return true;
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


        template <typename C>
            requires istl::cvref_as<C, Callable>
        static constexpr void call_set(C&&           segment,
                                       context_type& ctx) noexcept(invocable_inorder_type::is_nothrow) {
            if constexpr (stl::is_void_v<return_type>) {
                call(stl::forward<C>(segment), ctx);
            } else {
                set_response(call(stl::forward<C>(segment), ctx), ctx);
            }
        }


        template <typename C>
            requires istl::cvref_as<C, Callable>
        static constexpr bool call_set_get(C&&           segment,
                                           context_type& ctx) noexcept(invocable_inorder_type::is_nothrow) {
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


        template <typename C>
            requires istl::cvref_as<C, Callable>
        static constexpr bool call_then(C&&           callable,
                                        context_type& ctx) noexcept(invocable_inorder_type::is_nothrow) {
            if constexpr (stl::is_void_v<return_type>) {
                call(stl::forward<C>(callable), ctx);
                return true;
            } else {
                auto       ret = call(stl::forward<C>(callable), ctx);
                bool const res = should_continue(ret);
                set_response(stl::move(ret), ctx);
                return res;
            }
        }
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


} // namespace webpp::http

#endif // WEBPP_VALVE_TRAITS_HPP
