#ifndef WEBPP_ROUTER_H
#define WEBPP_ROUTER_H

#include "http/request.h"
#include "http/response.h"
#include "routes/route.h"
#include "std/vector.h"
#include "utils/const_list.h"
#include "utils/containers.h"
#include "utils/functional.h"

#include <functional>
#include <map>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace webpp {




    /**
     * Check if we can convert T to U
     * @tparam T
     * @tparam U
     */
    template <typename T, typename U>
    struct can_convert
      : std::integral_constant<bool,
                               (!std::is_void_v<T> && !std::is_void_v<U>)&&(
                                 std::is_convertible_v<T, U> ||
                                 std::is_constructible_v<T, U> ||
                                 std::is_assignable_v<T, U>)> {};

    template <typename T, typename U>
    constexpr bool can_convert_v = can_convert<T, U>::value;

    /**
     * Check if we can convert T to a string
     * @tparam Traits
     * @tparam U
     */
    template <typename Traits, typename U>
    struct can_convert_to_string
      : std::integral_constant<
          bool, (can_convert_v<Traits, U, typename Traits::string_type> ||
                 can_convert_v<Traits, U, typename Traits::string_view_type>)> {
    };

    template <typename Traits, typename U>
    constexpr bool can_convert_to_string_v =
      can_convert_to_string<Traits, U>::value;





    template <typename HandleExceptionCallable, typename Callable,
              typename... Args>
    constexpr auto
    run_and_catch(HandleExceptionCallable const& handle_exception,
                  Callable const&                c, Args... args) noexcept {
        using RetType = std::invoke_result_t<Callable, Args...>;
        if constexpr (std::is_nothrow_invocable_r_v<RetType, Callable,
                                                    Args...>) {
            // It's noexcept, we call it knowing that.
            return callable(std::forward<Args>(args)...);
        } else if constexpr (std::is_invocable_r_v<RetType, Callable,
                                                   Args...>) {
            try {
                return callable(std::forward<Args>(args)...);
            } catch (...) {
                handle_exception(std::current_exception());
                return false; // todo: check this
            }
        } else {
            throw std::invalid_argument(
              "The specified route is not valid. We're not able to call it.");
        }
    }

    /**
     * Handle the return type of a route::operator()
     * @tparam RetType
     * @param ret
     * @return
     */
    template <typename RetType>
    constexpr auto handle_callable_return_type(RetType&& ret) noexcept {
        if constexpr (std::is_void_v<RetType>) {
            // it's an "Unknown route"
            return;
        } else if constexpr (is_response_v<RetType> ||
                             can_convert_to_string_v<Traits, RetType>) {
            // It was a "Response route"
        } else if constexpr (is_basic_context<RetType>::value) {
            // It's a "Context Switching route"
        } else if constexpr (std::is_convertible_v<RetType, bool>) {
            // It's a "Conditional route"
        }
    }

    template <typename C, typename ContextType>
    inline auto call_it(C& c, ContextType& context) noexcept {
        using req_t     = typename ContextType::request_type const&;
        using res_t     = typename ContextType::response_type&;
        using callable  = std::decay_t<C>;
        using context_t = ContextType&;
        auto callback   = std::forward<C>(c);

        constexpr auto handle_exception = [](auto err) {

        };


        // TODO: add more overrides. You can simulate "dependency injection" here

        if constexpr (std::is_invocable_v<callable, req_t>) {
            return handle_callback_return_type(
              run_and_catch(handle_exception, callback, context.request));
        } else if constexpr (std::is_invocable_v<callable, res_t>) {
            return handle_callback_return_type(
              run_and_catch(handle_exception, callback, context.response));
        } else if constexpr (std::is_invocable_v<callable, req_t, res_t>) {
            return handle_callback_return_type(run_and_catch(
              handle_exception, callback, context.request, context.response));
        } else if constexpr (std::is_invocable_v<callable, res_t, req_t>) {
            return handle_callback_return_type(run_and_catch(
              handle_exception, callback, context.response, context.request));
        } else if constexpr (std::is_invocable_v<callable, context_t>) {
            return handle_callback_return_type(
              run_and_catch(handle_exception, callback, context));
        } else if constexpr (std::is_invocable_v<callable>) {
            return handle_callback_return_type(
              run_and_catch(handle_exception, callback));
        } else {
            throw std::invalid_argument(
              "The specified route cannot be called.");
        }
    }


    /**
     * Router:
     *
     * Router types:
     *   - Dynamic route   : dynamic_route
     *       The routes and the context can be changed at runtime.
     *   - Constexpr route : const_route
     *       No route can be changed at runtime. The initial context cannot be
     *       changed either.
     *
     * Routing table implementation:
     *   - Entry route list:
     *       It's a list of entry routes (which they include some sub-routes of
     *       their own).
     *       We don't need  a Global Routing table, we just need a good
     *       proioritization technique.
     *   - Priorities of the entry route list:
     *       This table will be used to change the priority of the entry route
     *       list. This priority change is done through the context extensions.
     *       So the context extensions need a way to access the router. The best
     *       way to do so is to add this route itself to the base_context. This
     *       is possible because the constext is created initially by the
     *       router itself and the changes in its type should be done inside the
     *       router too.
     *
     */



    //
    // template <typename Interface>
    // using route_sigs = overloaded<
    // std::function<void()>, std::function<void(request_t<Interface> const&)>,
    // std::function<void(response&)>,
    // std::function<void(request_t<Interface> const&, response&)>,
    // std::function<void(response&, request_t<Interface> const&)>,
    // std::function<response()>,
    // std::function<response(request_t<Interface> const&)>,
    // std::function<response(response&)>,
    // std::function<response(request_t<Interface> const&, response&)>,
    // std::function<response(response&, request_t<Interface> const&)>>;
    //
    struct dynamic_route {

      protected:
        // todo: maybe don't use std::function? it's slow a bit (but not that much)
        using callback_t = std::function<void()>;

        callback_t callback = nullptr;

      public:
        // fixme: it gives me error when I put "noexcept" here:
        dynamic_route() = default;
        dynamic_route(callback_t callback) noexcept : callback(callback) {
        }

        template <typename C>
        dynamic_route& operator=(C&& callback) noexcept {
            this->callback = [=](req_t req, res_t res) noexcept {
                return call_it(callback, req, res);
            };
            return *this;
        }

        auto operator()(req_t req, res_t res) noexcept {
            return callback(req, res);
        }

        inline bool is_match(req_t req) noexcept {
            return condition(req);
        }
    };

    template <typename... Route>
    struct const_router {
        const std::tuple<Route...> routes;

        // this madness just fills the array with this: {0, 1, 2, 3, ..., N}
        std::array<std::size_t, sizeof...(Route)> priorities =
          ([]<std::size_t... I>(std::index_sequence<I...>) {
              return std::array<std::size_t, sizeof...(I)>{I...};
          })(std::make_index_sequence<sizeof...(Route)>());


        struct iterator {
            using value_type = const_router<Route...>;

          private:
            using routes_ptr_t     = decltype(routes)*;
            using priorities_ptr_t = decltype(priorities)*;

            routes_ptr_t     routes_ptr;
            priorities_ptr_t priorities_ptr;
            std::size_t      index;

          public:
            constexpr iterator(routes_ptr_t     _routes_ptr     = nullptr,
                               priorities_ptr_t _priorities_ptr = nullptr,
                               std::size_t _index = sizeof...(Route)) noexcept
              : routes_ptr{_routes_ptr},
                priorities_ptr{_priorities_ptr},
                index(_index) {
            }

            constexpr iterator(iterator const& iter) noexcept
              : routes_ptr{iter.routes_ptr},
                priorities_ptr{iter.priorities_ptr},
                index{iter.index} {
            }

            auto& operator=(iterator const& iter) noexcept {
                if (iter != *this) {
                    routes_ptr     = iter.routes_ptr;
                    priorities_ptr = iter.priorities_ptr;
                    index          = iter.index;
                }
                return *this;
            }

            ~iterator() noexcept {
            }

            auto& operator++() noexcept {
                ++index;
                return *this;
            }
            auto& operator--() noexcept {
                --index;
                return *this;
            }
            auto& operator->() noexcept {
                return (*routes_ptr)[(*priorities_ptr)[index]];
            }
            auto& operator*() noexcept {
                return &(*routes_ptr)[(*priorities_ptr)[index]];
            }

            constexpr bool operator==(iterator const& iter) const noexcept {
                return routes_ptr == iter.routes_ptr &&
                       priorities_ptr == iter.priorities_ptr &&
                       index == iter.index;
            }

            constexpr bool operator!=(iterator const& iter) const noexcept {
                return routes_ptr != iter.routes_ptr ||
                       priorities_ptr != iter.priorities_ptr ||
                       index != iter.index;
            }

            constexpr void swap(iterator& iter) noexcept {
                using std::swap;
                swap(routes_ptr, iter.routes_ptr);
                swap(priorities_ptr, iter.priorities_ptr);
                swap(index, iter.index);
            }
        };

        constexpr const_router(Route&&... _route) noexcept
          : routes(std::forward<Route>(_route)...) {
        }

        constexpr auto begin() const noexcept {
            return iterator(routes, priorities, 0);
        }
        constexpr auto end() const noexcept {
            return iterator(routes, priorities);
        }

        constexpr auto size() const noexcept {
            return sizeof...(Route);
        }

        template <std::size_t N = 0>
        constexpr auto& operator[](std::size_t i) const noexcept {
            if (N == i) {
                return std::get<N>(routes);
            }
            if constexpr (N + 1 < size()) {
                return operator[]<N + 1>(i);
            }
            throw std::invalid_argument("The specified index is not valid");
        }



        /**
         * Run the request through the routes and then return the response
         * @param req
         * @return final response
         */
        template <typename RequestType, typename ResponseType>
        ResponseType operator()(RequestType& req) noexcept {
            ResponseType res;
            for (auto& _route : *this) {
                _route();
            }
            return res;
        }

        template <typename RouteType, typename ReternedContext,
                  typename RecievedContext>
        ReternedContext operator()(RouteType&&       _route,
                                   RecievedContext&& ctx) noexcept {
            return _route(ctx);
        }
    };

    /**
     * This is the router; the developers need this class to inject their routes
     * and also add more migrations.
     *
     * @param Interface
     */
    template <typename... Route, typename RouteList = const_list<Route...>>
    struct router_t {
        template <typename... Args>
        constexpr router_t(Args&&... args) noexcept
          : routes(std::forward<Args>(args)...) {
        }


        template <typename Route>
        constexpr auto on(Route&& _route) noexcept {
            static_assert(is_route<Route>::value,
                          "The specified route is not valid.");


            if constexpr (is_specialization_of<RouteList, std::tuple>::value) {
                // when it's a tuple
                auto _tup =
                  std::tuple_cat(routes, std::make_tuple(std::move(_route)));
                return router_t<Interface, decltype(_tup)>{_tup};

            } else if constexpr (is_specialization_of<RouteList,
                                                      const_list>::value) {
                // for const_list (constexpr version)
                auto _the_routes = routes + std::move(_route);
                return router_t<Interface, decltype(_the_routes)>{_the_routes};

            } else if constexpr (is_container_v<RouteList>) {
                // for containers (dynamic)
                static_assert(
                  can_cast<Route, typename RouteList::value_type>::value,
                  "The specified route does not match the router version of "
                  "route.");
                routes.emplace_back(std::forward<Route>(_route));

            } else {
                throw std::invalid_argument(
                  "The container for routes is unknown.");
            }
        }
    };

}; // namespace webpp

#endif // WEBPP_ROUTER_H
