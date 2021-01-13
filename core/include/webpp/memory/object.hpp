// Created by moisrex on 1/11/21.

#ifndef WEBPP_OBJECT_HPP
#define WEBPP_OBJECT_HPP

#include "allocator_pack.hpp"
#include "buffer.hpp"

namespace webpp::object {

    template <typename T,
              alloc::feature_pack     FPack,
              AllocatorDescriptorList AllocDescList = stl::pmr::allocator_descriptors>
    struct object : public alloc::alloc_finder<T, FPack, AllocDescList>::new_type {
      private:
        using alloc_details = alloc::alloc_finder<T, FPack, AllocDescList>;
        using super         = typename alloc_details::new_type;

      public:
        using allocator_type = typename alloc_details::allocator_type;
        using resource_type  = typename alloc_details::resource_type;

        template <typename... Args>
        requires(requires(allocator_type const& the_alloc, Args... args) {
            super{stl::allocator_arg, the_alloc, stl::forward<Args>(args)...};
        }) object(alloc::allocator_pack<AllocDescList> const& alloc_pack, resource_type& res, Args&&... args)
          : super{stl::allocator_arg,
                  alloc_pack.template get_allocator<allocator_type, resource_type>(res),
                  stl::forward<Args>(args)...} {}

        template <typename... Args>
        requires(requires(allocator_type const& the_alloc, Args... args) {
            super{the_alloc, stl::forward<Args>(args)...};
        }) object(alloc::allocator_pack<AllocDescList> const& alloc_pack, resource_type& res, Args&&... args)
          : super{alloc_pack.template get_allocator<allocator_type, resource_type>(res),
                  stl::forward<Args>(args)...} {}

        template <typename... Args>
        requires(sizeof...(Args) > 0 && // to resolve some ambiguity with the above version
                 requires(allocator_type const& the_alloc, Args... args) {
                     super{stl::forward<Args>(args)..., the_alloc};
                 })
          object(alloc::allocator_pack<AllocDescList> const& alloc_pack, resource_type& res, Args&&... args)
          : super{stl::forward<Args>(args)...,
                  alloc_pack.template get_allocator<allocator_type, resource_type>(res)} {}


        //        template <typename... Args>
        //        requires(requires(allocator_type const& the_alloc, Args... args) {
        //            super{stl::allocator_arg, the_alloc, stl::forward<Args>(args)...};
        //        }) object(alloc::allocator_pack<AllocDescList> const& alloc_pack, Args&&... args)
        //          : super{stl::allocator_arg,
        //                  alloc_pack.template get_allocator<allocator_type, resource_type>(),
        //                  stl::forward<Args>(args)...} {}
        //
        //        template <typename... Args>
        //        requires(requires(allocator_type const& the_alloc, Args... args) {
        //            super{the_alloc, stl::forward<Args>(args)...};
        //        }) object(alloc::allocator_pack<AllocDescList> const& alloc_pack, Args&&... args)
        //          : super{alloc_pack.template get_allocator<allocator_type, resource_type>(),
        //                  stl::forward<Args>(args)...} {}
        //
        //        template <typename... Args>
        //        requires(requires(allocator_type const& the_alloc, Args... args) {
        //            super{stl::forward<Args>(args)..., the_alloc};
        //        }) object(alloc::allocator_pack<AllocDescList> const& alloc_pack, Args&&... args)
        //          : super{stl::forward<Args>(args)...,
        //                  alloc_pack.template get_allocator<allocator_type, resource_type>()} {}
    };


    template <typename T,
              typename StackType                    = stack<>,
              AllocatorDescriptorList AllocDescList = stl::pmr::allocator_descriptors>
    struct local : object<T, alloc::monotonic_features, AllocDescList> {
      private:
        using super = object<T, alloc::monotonic_features, AllocDescList>;

      public:
        using stack_type    = StackType;
        using resource_type = typename super::resource_type;

        stack_type    buffer;
        resource_type res{buffer.data(), buffer.size(), stl::pmr::new_delete_resource()};

        using super::object;

        template <typename... Args>
        constexpr local(alloc::allocator_pack<AllocDescList> const& alloc_pack, Args&&... args)
          : super{alloc_pack, res, stl::forward<Args>(args)...} {}
    };

} // namespace webpp::object

#endif // WEBPP_OBJECT_HPP
