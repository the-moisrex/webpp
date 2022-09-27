#ifndef WEBPP_DYNAMIC_ROUTER_HPP
#define WEBPP_DYNAMIC_ROUTER_HPP

#include "../../std/functional.hpp"
#include "../../std/vector.hpp"
#include "../../traits/default_traits.hpp"
#include "../../traits/enable_traits.hpp"
#include "../../utils/functional.hpp"

#include <any>

namespace webpp {

    template <Traits TraitsType = default_traits>
    struct basic_dynamic_context {};

    /**
     * @brief A Router that's is fully customizable at runtime
     *
     * This class will be used directly by the developers using this whole library. So be nice and careful and
     * user friendly.
     */
    template <EnabledTraits TraitsEnabler = enable_owner_traits<default_traits>>
    struct basic_dynamic_router : TraitsEnabler {
        using etraits            = TraitsEnabler;
        using traits_type        = typename etraits::traits_type;
        using response_type      = std::string;
        using caller_type        = response_type();
        using route_allocator    = traits::general_allocator<traits_type, stl::byte>;
        using dynamic_route_type = istl::function<caller_type, route_allocator>;
        using vector_allocator   = traits::general_allocator<traits_type, dynamic_route_type>;
        using context_type       = basic_dynamic_context<traits_type>;

        static constexpr auto log_cat = "DRouter";

      private:
        // todo: implement a function_vector that'll require only one allocator not one for each
        stl::vector<dynamic_route_type, vector_allocator> routes;
        bool                                              is_synced = false;

      public:
        // These are the callable types
        stl::vector<stl::any, traits::general_allocator<traits_type, stl::any>> objects;


        constexpr basic_dynamic_router() noexcept
            requires(etraits::is_resource_owner)
          : etraits{} {}

        template <typename ET>
            requires(EnabledTraits<stl::remove_cvref_t<ET>> &&
                     !stl::same_as<stl::remove_cvref_t<ET>, basic_dynamic_router>)
        constexpr basic_dynamic_router(ET&& et) : etraits{stl::forward<ET>(et)} {}


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
        constexpr basic_dynamic_router& set_route(T&& callable) noexcept {}

        /**
         * @brief Register a member function and it's object; It's the same as using std::mem_fn.
         */
        template <typename T, typename U>
            requires(stl::is_member_function_pointer_v<T>)
        constexpr basic_dynamic_router& set_route(T&& method, U&& obj) noexcept {
            using method_type = member_function_pointer<stl::remove_cvref_t<T>>;
            using type        = typename method_type::type;
            using return_type = typename method_type::return_type;
            static_assert(stl::same_as<type, stl::remove_cvref_t<U>>,
                          "The specified member function is not from the specified object.");

            return set_route(
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
        constexpr basic_dynamic_router& set_route(T&& method) noexcept {
            using method_type = member_function_pointer<stl::remove_cvref_t<T>>;
            using type        = typename method_type::type;
            for (auto& obj : objects) {
                if (obj.type() == typeid(type)) {
                    return set_route(method, stl::any_cast<type>(obj));
                }
            }

            // default constructing it if it's possible and use that object
            if constexpr (stl::is_default_constructible_v<type>) {
                objects.emplace_back(type{});
                return set_route(method, stl::any_cast<type>(objects.back()));
            } else {
                this->logger.error(
                  log_cat,
                  fmt::format("You have not specified an object with typeid of '{}' in your dynamic router,"
                              " but you've tried to register a member function of unknown type for router.",
                              typeid(type).name()));
            }
            return *this;
        }


        // Append a migration
        template <typename C>
        constexpr basic_dynamic_router& operator+=(C&& callable) {
            // todo
            return *this;
        }


        template <typename ReqType>
        constexpr response_type operator()(ReqType&& req) noexcept {
            // todo
            return {};
        }
    };


    template <Traits TraitsType = default_traits>
    using dynamic_router = basic_dynamic_router<enable_owner_traits<TraitsType>>;


    template <Traits TraitsType = default_traits>
    using dynamic_context = basic_dynamic_context<TraitsType>;


    namespace pmr {
        using dynamic_context = webpp::dynamic_context<std_pmr_traits>;
        using dynamic_router  = webpp::dynamic_router<std_pmr_traits>;
    } // namespace pmr

} // namespace webpp

#endif // WEBPP_DYNAMIC_ROUTER_HPP
