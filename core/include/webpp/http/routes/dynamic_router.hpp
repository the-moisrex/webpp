#ifndef WEBPP_DYNAMIC_ROUTER_HPP
#define WEBPP_DYNAMIC_ROUTER_HPP

#include "../../std/functional.hpp"
#include "../../std/vector.hpp"
#include "../../traits/default_traits.hpp"
#include "../../traits/enable_traits.hpp"

namespace webpp {

    /**
     * @brief A Router that's is fully customizable at runtime
     *
     * This class will be used directly by the developers using this whole library. So be nice and careful and
     * user friendly.
     */
    template <Traits TraitsType, EnabledTraits TraitsEnabler = enable_owner_traits<TraitsType>>
    struct basic_dynamic_router : TraitsEnabler {
        using traits_type        = TraitsType;
        using etraits            = TraitsEnabler;
        using caller_type        = void();
        using route_allocator    = traits::general_allocator<traits_type, stl::byte>;
        using dynamic_route_type = istl::function<caller_type, route_allocator>;
        using vector_allocator   = traits::general_allocator<traits_type, dynamic_route_type>;

      private:
        // todo: implement a function_vector that'll require only one allocator not one for each
        stl::vector<dynamic_route_type, vector_allocator> routes;

      public:
        constexpr basic_dynamic_router() noexcept requires(etraits::is_owner) : etraits{} {}

        template <typename ET>
            requires(EnabledTraits<stl::remove_cvref_t<ET>> &&
                     !stl::same_as<stl::remove_cvref_t<ET>, basic_dynamic_router>)
        constexpr basic_dynamic_router(ET&& et) : etraits{stl::forward<ET>(et)} {}


        template <typename ReqType>
        constexpr auto operator()(ReqType&& req) noexcept {}
    };


    template <Traits TraitsType = default_traits>
    using dynamic_router = basic_dynamic_router<TraitsType, enable_owner_traits<TraitsType>>;

} // namespace webpp

#endif // WEBPP_DYNAMIC_ROUTER_HPP
