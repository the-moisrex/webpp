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
#include "../request_view.hpp"
#include "../status_code.hpp"
#include "route.hpp"

#include <any>

namespace webpp::http {


    /**
     * This is a dynamic server which holds a reference to the servers
     */
    template <Traits TraitsType, ExtensionList RootExtensions, typename... ServerTypes>
    struct basic_dynamic_server {
        using servers_variant   = stl::variant<ServerTypes*...>;
        using traits_type       = TraitsType;
        using root_extensions   = RootExtensions;
        using string_view_type  = traits::string_view<traits_type>;
        using supported_servers = stl::tuple<ServerTypes...>;

      private:
        servers_variant svrvar; // server variant

#define call_svr(mem, ...)                                      \
    stl::visit(                                                 \
      [](auto* svr) noexcept(noexcept(svr->mem(__VA_ARGS__))) { \
          return svr->mem(__VA_ARGS__);                         \
      },                                                        \
      svrvar)

      public:
        template <typename ServerType>
            requires(istl::one_of<ServerTypes..., ServerType>)
        basic_dynamic_server(ServerType& inp_server) : svrvar{&inp_server} {}

        // Get the server name that's being used
        [[nodiscard]] string_view_type server_name() const noexcept {
            return call_svr(server_name);
        }

#undef call_svr
    };


    template <ExtensionList ExtensionListType, EnabledTraits TraitsEnabler>
    struct basic_dynamic_router;


    template <ExtensionList ExtensionListType, EnabledTraits TraitsEnabler>
    struct dynamic_route {
        using etraits           = TraitsEnabler;
        using traits_type       = typename etraits::traits_type;
        using string_type       = traits::general_string<traits_type>;
        using non_owner_etraits = typename etraits::non_owner_type;
        using route_allocator   = traits::general_allocator<traits_type, stl::byte>;
        using router_type       = basic_dynamic_router<ExtensionListType, TraitsEnabler>;
        using router_ref        = stl::add_lvalue_reference_t<router_type>;

        enum operator_type { none, and_op, or_op, xor_op };

      private:
        route_type    route;
        operator_type op = none;

        template <typename R>
        constexpr dynamic_route(router_ref inp_router, R&& inp_route) noexcept
          : route{stl::allocator_arg,
                  alloc::general_alloc_for<route_type>(inp_router.get_traits().alloc_pack),
                  stl::forward<R>(inp_route)} {}

      public:
        constexpr dynamic_route(router_ref inp_router) noexcept
          : route{alloc::general_alloc_for<route_type>(inp_router.get_traits().alloc_pack)} {}



        template <Context CtxT, HTTPRequest ReqT>
        constexpr auto operator()(CtxT&& ctx, ReqT&& req) const noexcept {
            switch (op) {
                case none: break;
                case and_op:
            }
            return http::call_route(route, stl::forward<CtxT>(ctx), stl::forward<ReqT>(req));
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
        using extension_list    = RootExtensions;
        using etraits           = TraitsEnabler;
        using traits_type       = typename etraits::traits_type;
        using non_owner_etraits = typename etraits::non_owner_type;
        using route_type        = dynamic_route<extension_list, etraits>;
        using vector_allocator  = traits::general_allocator<traits_type, route_type>;
        using map_allocator =
          traits::general_allocator<traits_type, stl::pair<status_code const, route_type>>;
        using string_type      = traits::general_string<traits_type>;
        using string_view_type = traits::string_view<traits_type>;
        using objects_type     = stl::vector<stl::any, traits::general_allocator<traits_type, stl::any>>;
        using routes_type      = stl::vector<route_type, vector_allocator>;
        using request_type     = simple_request<basic_dynamic_router, dynamic_request>;
        using context_type     = simple_context<request_type, extension_list>;
        using response_type    = simple_response_pack<traits_type, extension_list>;

        static constexpr auto log_cat = "DRouter";

        static_assert(HTTPResponse<response_type>,
                      "For some reason the response type is not a valid match for HTTPResponse concept.");

      private:
        // todo: implement a function_vector that'll require only one allocator not one for each
        routes_type                                                                          routes;
        stl::map<http::status_code, route_type, stl::less<http::status_code>, map_allocator> status_templates;
        bool is_synced = false;

      public:
        // These are the callable types
        objects_type objects;

        // we're not adding context and response here in router scope because we want the user to be able to
        // take advantage of parallelism


        constexpr basic_dynamic_router() noexcept requires(etraits::is_resource_owner)
          : etraits{},
            objects{alloc::general_alloc_for<objects_type>(*this)} {}

        template <typename ET>
            requires(EnabledTraits<stl::remove_cvref_t<ET>> &&
                     !stl::same_as<stl::remove_cvref_t<ET>, basic_dynamic_router>)
        constexpr basic_dynamic_router(ET&& et)
          : etraits{stl::forward<ET>(et)},
            objects{alloc::general_alloc_for<objects_type>(*this)} {}


        /**
         * @brief By enabling it, call to routes are synced and it becomes thread-safe
         * @details Attention: if you've enabled synced inside the server (in beast-server for example), or
         * the protocol that you're using is inherently synced (CGI for example), then enabling this is
         * essentially a useless thing and it'll just slow the process down (not significant enough for apps
         * with low use)
         */
        constexpr basic_dynamic_router& synced(bool enable_synced = true) noexcept {
            is_synced = enable_synced;
            return *this;
        }

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

            return routify([callable = obj, method]<typename... Args> requires(
              method_type::template is_same_args_v<Args...>)(
              Args && ... args) constexpr noexcept(method_type::is_noexcept) {
                return stl::invoke_result_t<return_type, Args...>(method,
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

        template <typename C>
        constexpr basic_dynamic_router& register_subroutes(C&& callable) {
            if constexpr (stl::is_invocable_v<C, subrouter_type&>) {
                auto router = subrouter();
                callable(router);
                apply_subrouter(router);
            } else if constexpr (stl::is_invocable_r_v<subrouter_type, C>) {
                apply_subrouter(callable());
            } else {
                static_assert_false(C, "Unknown type is specified for registering subroutes.");
            }
            return *this;
        }

        /**
         * Get an empty sub-router
         */
        [[nodiscard]] constexpr subrouter_type subrouter() const noexcept {
            return {};
        }


        // Append a migration
        template <typename C>
        constexpr basic_dynamic_router& operator+=(C&& callable) {
            return on<C>(stl::forward<C>(callable));
        }


        template <HTTPRequest ReqType>
        constexpr response_type const& operator()(ReqType&& in_req) noexcept {
            // todo
            return res;
        }
    };


    using dynamic_response = basic_dynamic_response<default_traits>;
    using dynamic_router   = basic_dynamic_router<empty_extension_pack, enable_owner_traits<default_traits>>;
    using dynamic_context  = basic_dynamic_context<default_traits>;


    namespace pmr {
        using dynamic_response = webpp::http::basic_dynamic_response<std_pmr_traits>;
        using dynamic_context  = webpp::http::basic_dynamic_context<std_pmr_traits>;
        using dynamic_router =
          webpp::http::basic_dynamic_router<empty_extension_pack, enable_owner_traits<std_pmr_traits>>;
    } // namespace pmr

} // namespace webpp::http

#endif // WEBPP_DYNAMIC_ROUTER_HPP
