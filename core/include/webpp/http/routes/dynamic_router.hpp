#ifndef WEBPP_DYNAMIC_ROUTER_HPP
#define WEBPP_DYNAMIC_ROUTER_HPP

#include "../../std/functional.hpp"
#include "../../std/vector.hpp"
#include "../../traits/default_traits.hpp"
#include "../../traits/enable_traits.hpp"

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

      private:
        // todo: implement a function_vector that'll require only one allocator not one for each
        stl::vector<dynamic_route_type, vector_allocator> routes;
        bool                                              is_synced = false;

      public:
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
