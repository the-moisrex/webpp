#ifndef WEBPP_DYNAMIC_ROUTER_HPP
#define WEBPP_DYNAMIC_ROUTER_HPP

#include "../../traits/default_traits.hpp"
#include "../response.hpp"

namespace webpp::http {

    struct dynamic_route {};

    template <Traits TraitsType = default_traits>
    struct dynamic_router : public stl::set<dynamic_route> { // fixme: use an allocator for set
        using traits_type   = TraitsType;
        using response_type = response<traits_type>; // fixme: update this type

        using stl::set<dynamic_router>::set; // ctor


        constexpr response_type operator()() {
            // run the router here
        }
    };


} // namespace webpp::http

#endif // WEBPP_DYNAMIC_ROUTER_HPP
