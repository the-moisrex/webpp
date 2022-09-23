#ifndef WEBPP_DYNAMIC_ROUTER_HPP
#define WEBPP_DYNAMIC_ROUTER_HPP

#include "../../traits/enable_traits.hpp"

namespace webpp {

    template <Traits TraitsType, EnabledTraits TraitsEnabler = enable_owner_traits<TraitsType>>
    struct basic_dynamic_router : TraitsEnabler {
        using traits_type = TraitsType;
        using etraits     = TraitsEnabler;

      private:
      public:
        template <typename ET>
            requires(EnabledTraits<stl::remove_cvref_t<ET>> &&
                     !stl::same_as<stl::remove_cvref_t<ET>, basic_dynamic_router>)
        constexpr basic_dynamic_router(ET&& et) : etraits{stl::forward<ET>(et)} {}
    };

} // namespace webpp

#endif // WEBPP_DYNAMIC_ROUTER_HPP
