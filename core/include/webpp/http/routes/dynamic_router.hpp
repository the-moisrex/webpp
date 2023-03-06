#ifndef WEBPP_DYNAMIC_ROUTER_HPP
#define WEBPP_DYNAMIC_ROUTER_HPP

#include "../../extensions/extension.hpp"
#include "../../std/functional.hpp"
#include "../../std/map.hpp"
#include "../../std/string.hpp"
#include "../../std/vector.hpp"
#include "../../traits/default_traits.hpp"
#include "../../traits/enable_traits.hpp"
#include "../../utils/functional.hpp"
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
        struct route_input_args {
            using callable_type      = stl::remove_cvref_t<Callable>;
            using context_type       = ContextType;
            using request_type       = typename context_type::request_type;
            using response_type      = typename context_type::response_type;
            using response_body_type = typename response_type::body_type;
            using invocable_inorder_type =
              istl::invocable_inorder<callable_type, context_type&, request_type&, response_type&>;
            using return_type = stl::remove_cvref_t<typename invocable_inorder_type::result>;

            static_assert(invocable_inorder_type::value, "We're not able to call your route.");

            static constexpr auto call(Callable&&    callable,
                                       context_type& ctx) noexcept(invocable_inorder_type::is_nothrow) {
                return istl::invoke_inorder(callable, ctx, ctx.request, ctx.response);
            }

            static constexpr bool is_positive(return_type const& ret) noexcept {
                if constexpr (stl::same_as<return_type, bool>) {
                    return ret;
                } else if constexpr (istl::part_of<return_type, istl::nothing_type>) {
                    return false;
                } else if constexpr (stl::is_void_v<return_type> || HTTPResponse<return_type>) {
                    return true;
                } else {
                    return false;
                }
            }

            static constexpr void set_response(return_type&& ret, context_type& ctx) {
                if constexpr (stl::is_void_v<return_type> ||
                              istl::part_of<return_type, bool, istl::nothing_type>) {
                    // ignore the result
                } else if constexpr (HTTPResponse<return_type> || HTTPResponseBody<return_type>) {
                    ctx.response = stl::move(ret);
                } else if constexpr (istl::Optional<return_type>) {
                    if (ret) {
                        set_response(stl::move(*ret), ctx);
                    }
                } else if constexpr (stl::is_pointer_v<return_type>) {
                    if (ret != nullptr) {
                        set_response(*ret, ctx);
                    }
                } else if constexpr (stl::same_as<return_type, http::status_code>) {
                    ctx.response = ret;
                } else if constexpr (stl::integral<return_type>) {
                    ctx.response = static_cast<http::status_code>(ret);
                } else if constexpr (HTTPSerializableBody<return_type, response_body_type>) {
                    ctx.response = stl::move(ret);
                } else {
                    static_assert_false(return_type,
                                        "We don't know what to do with your route's return type.");
                }
            }
        };



        template <typename LeftCallable, typename RightCallable>
        struct and_callables {
            using left_type  = stl::remove_cvref_t<LeftCallable>;
            using right_type = stl::remove_cvref_t<RightCallable>;

            left_type  lhs;
            right_type rhs;

            template <Traits TraitsType>
            constexpr void operator()(basic_context<TraitsType>& ctx) {
                using context_type     = basic_context<TraitsType>;
                using left_input_args  = route_input_args<left_type, context_type>;
                using right_input_args = route_input_args<right_type, context_type>;

                auto left_res = left_input_args::call(lhs, ctx);
                if (!left_input_args::is_positive(left_res)) {
                    left_input_args::set_response(stl::move(left_res), ctx);
                    return;
                }
                auto right_res = right_input_args::call(rhs, ctx);
                right_input_args::set_response(stl::move(right_res), ctx);
            }
        };
    } // namespace details


    template <Traits TraitsType>
    struct dynamic_route {
        using traits_type     = TraitsType;
        using string_type     = traits::general_string<traits_type>;
        using route_allocator = traits::general_allocator<traits_type, stl::byte>;
        using context_type    = basic_context<traits_type>;
        using callable_type   = istl::function<void(context_type&), route_allocator>;

      private:
        callable_type callable;

        template <typename R, EnabledTraits ET>
        constexpr dynamic_route(ET&& inp_router, R&& inp_route) noexcept
          : callable{stl::allocator_arg,
                     alloc::general_alloc_for<callable_type>(inp_router),
                     stl::forward<R>(inp_route)} {}

      public:
        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        template <EnabledTraits ET>
            requires(!istl::same_as_cvref<ET, dynamic_route>)
        constexpr dynamic_route(ET&& inp_router) noexcept
          : callable{alloc::general_alloc_for<callable_type>(inp_router)} {}
        // NOLINTEND(bugprone-forwarding-reference-overload)

        template <typename CallableType>
        constexpr dynamic_route operator/(CallableType&& new_callable) {
            dynamic_route new_route;
            new_route.callable = [*this, new_callable](context_type& ctx) constexpr {
                // todo: evaluate the path
                ctx.response = call_route(callable, ctx, ctx.request);
                ctx.response = call_route(stl::forward<CallableType>(new_callable), ctx, ctx.request);
                return ctx.response;
            };
            return new_route;
        }

        template <Context CtxT, HTTPRequest ReqT>
        constexpr auto operator()(CtxT&& ctx, ReqT&& req) const noexcept {
            return http::call_route(callable, stl::forward<CtxT>(ctx), stl::forward<ReqT>(req));
        }
    };

    /**
     * @brief A Router that's is fully customizable at runtime
     *
     * This class will be used directly by the developers using this whole library. So be nice and careful and
     * user friendly.
     */
    template <ExtensionList RootExtensions, EnabledTraits TraitsEnabler>
    struct basic_dynamic_router : TraitsEnabler {
        using root_extensions   = RootExtensions;
        using etraits           = TraitsEnabler;
        using traits_type       = typename etraits::traits_type;
        using non_owner_etraits = typename etraits::non_owner_type;
        using route_type        = dynamic_route<traits_type>;
        using vector_allocator  = traits::general_allocator<traits_type, route_type>;
        using map_allocator =
          traits::general_allocator<traits_type, stl::pair<status_code const, route_type>>;
        using string_type      = traits::general_string<traits_type>;
        using string_view_type = traits::string_view<traits_type>;
        using objects_type     = stl::vector<stl::any, traits::general_allocator<traits_type, stl::any>>;
        using routes_type      = stl::vector<route_type, vector_allocator>;
        using response_type    = basic_response<traits_type>;
        using context_type     = basic_context<traits_type>;
        using request_type     = basic_request<traits_type>;

        static constexpr auto log_cat = "DRouter";

        static_assert(HTTPResponse<response_type>,
                      "For some reason the response type is not a valid match for HTTPResponse concept.");

      private:
        // todo: implement a function_vector that'll require only one allocator not one for each
        routes_type                                                                          routes;
        stl::map<http::status_code, route_type, stl::less<http::status_code>, map_allocator> status_templates;


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
        template <typename ET>
            requires(EnabledTraits<ET> && !istl::same_as_cvref<ET, basic_dynamic_router>)
        constexpr basic_dynamic_router(ET&& et)
          : etraits{stl::forward<ET>(et)},
            objects{alloc::general_alloc_for<objects_type>(*this)} {}
        // NOLINTEND(bugprone-forwarding-reference-overload)


        template <typename T>
        constexpr auto routify(T&& callable) noexcept {}

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
        constexpr response_type error(status_code code) const noexcept {
            return response(code);
        }

        template <typename StrT>
            requires(istl::StringifiableOf<string_type, StrT>)
        constexpr response_type error(status_code code, StrT&& reason) const noexcept {
            if (auto const it = status_templates.find(code); it != status_templates.end()) {
                return it; // response type will call this
            }
            auto res = error(code);
            res.body; // todo
            return res;
        }

        constexpr response_type response(status_code code) const noexcept {
            return {this->get_traits(), code};
        }


        template <typename T>
        constexpr basic_dynamic_router& on(status_code code, T&& route) {
            status_templates.emplace(code, route);
            return *this;
        }


        // Append a migration
        template <typename C>
        constexpr basic_dynamic_router& operator+=(C&& callable) {
            return on<C>(stl::forward<C>(callable));
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
         */
        template <Context CtxT>
        constexpr void operator()(CtxT& ctx) {
            for (auto const& route : routes) {
                // todo
            }
        }
    };


    using dynamic_router = basic_dynamic_router<empty_extension_pack, enable_owner_traits<default_traits>>;


    namespace pmr {
        using dynamic_router =
          webpp::http::basic_dynamic_router<empty_extension_pack, enable_owner_traits<std_pmr_traits>>;
    } // namespace pmr

} // namespace webpp::http

#endif // WEBPP_DYNAMIC_ROUTER_HPP
