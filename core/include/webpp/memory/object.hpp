// Created by moisrex on 1/11/21.

#ifndef WEBPP_OBJECT_HPP
#define WEBPP_OBJECT_HPP

#include "allocator_pack.hpp"
#include "buffer.hpp"

namespace webpp::object {

    template <typename T,
              stl::size_t             StackSize     = 1024,
              AllocatorDescriptorList AllocDescList = stl::pmr::allocator_descriptors>
    struct local : public T {
        using super                                     = T;
        using buffer_type                               = stack<StackSize>;
        static constexpr stl::size_t         stack_size = StackSize;
        static constexpr alloc::feature_pack features   = alloc::monotonic_features;
        using ranked                                    = alloc::ranker<AllocDescList, features>;
        using resource_type = alloc::descriptors::storage<typename ranked::best_resource_descriptor>;
        using original_allocator_type = typename T::allocator_type;
        using value_type              = typename original_allocator_type::value_type;
        using allocator_type          = typename alloc::descriptors::allocator<
          typename ranked::best_allocator_descriptor>::template type<value_type>;

        buffer_type   buffer;
        resource_type res{buffer.data(), buffer.size(), stl::pmr::new_delete_resource()};

        template <typename... Args>
        requires(requires(allocator_type const& the_alloc, Args... args) {
            super{stl::allocator_arg, the_alloc, stl::forward<Args>(args)...};
        }) local(alloc::allocator_pack<AllocDescList> const& alloc_pack, Args&&... args)
          : super{stl::allocator_arg,
                  alloc_pack.template get_allocator<allocator_type, resource_type>(res),
                  stl::forward<Args>(args)...} {}

        template <typename... Args>
        requires(requires(allocator_type const& the_alloc, Args... args) {
            super{the_alloc, stl::forward<Args>(args)...};
        }) local(alloc::allocator_pack<AllocDescList> const& alloc_pack, Args&&... args)
          : super{alloc_pack.template get_allocator<allocator_type, resource_type>(res),
                  stl::forward<Args>(args)...} {}

        template <typename... Args>
        requires(requires(allocator_type const& the_alloc, Args... args) {
            super{stl::forward<Args>(args)..., the_alloc};
        }) local(alloc::allocator_pack<AllocDescList> const& alloc_pack, Args&&... args)
          : super{stl::forward<Args>(args)...,
                  alloc_pack.template get_allocator<allocator_type, resource_type>(res)} {}
    };

} // namespace webpp::object

#endif // WEBPP_OBJECT_HPP
