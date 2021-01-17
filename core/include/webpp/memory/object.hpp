// Created by moisrex on 1/11/21.

#ifndef WEBPP_OBJECT_HPP
#define WEBPP_OBJECT_HPP

#include "allocator_pack.hpp"
#include "buffer.hpp"

namespace webpp::object {

    template <typename T, alloc::feature_pack FPack, AllocatorDescriptorList AllocDescList>
    struct object : public alloc::alloc_finder<T, FPack, AllocDescList>::new_type {
      private:
        using alloc_details = alloc::alloc_finder<T, FPack, AllocDescList>;
        using super         = typename alloc_details::new_type;

      public:
        using object_type     = super;
        using allocator_type  = typename alloc_details::allocator_type;
        using resource_type   = typename alloc_details::resource_type;
        using alloc_pack_type = alloc::allocator_pack<AllocDescList>;

        static constexpr bool has_resource = !stl::is_void_v<resource_type>;
        using res_ref =
          stl::conditional_t<has_resource, stl::add_lvalue_reference_t<resource_type>, istl::nothing_type>;

        template <typename... Args>
        requires(!has_resource &&
                 requires(allocator_type const& the_alloc, Args... args) {
                     super{stl::allocator_arg, the_alloc, stl::forward<Args>(args)...};
                 }) constexpr object(alloc_pack_type& alloc_pack, Args&&... args)
          : super{stl::allocator_arg,
                  alloc_pack.template get_allocator<allocator_type, void>(),
                  stl::forward<Args>(args)...} {}

        template <typename... Args>
        requires(!has_resource &&
                 requires(allocator_type const& the_alloc, Args... args) {
                     super{the_alloc, stl::forward<Args>(args)...};
                 }) constexpr object(alloc_pack_type& alloc_pack, Args&&... args)
          : super{alloc_pack.template get_allocator<allocator_type, void>(), stl::forward<Args>(args)...} {}

        template <typename... Args>
        requires(!has_resource && sizeof...(Args) > 0 && // to resolve some ambiguity with the above version
                 requires(allocator_type const& the_alloc, Args... args) {
                     super{stl::forward<Args>(args)..., the_alloc};
                 }) constexpr object(alloc_pack_type& alloc_pack, Args&&... args)
          : super{stl::forward<Args>(args)..., alloc_pack.template get_allocator<allocator_type, void>()} {}



        template <typename... Args>
        requires(has_resource&& requires(allocator_type const& the_alloc, Args... args) {
            super{stl::allocator_arg, the_alloc, stl::forward<Args>(args)...};
        }) constexpr object(alloc_pack_type& alloc_pack, res_ref res, Args&&... args)
          : super{stl::allocator_arg,
                  alloc_pack.template get_allocator<allocator_type, resource_type>(res),
                  stl::forward<Args>(args)...} {}

        template <typename... Args>
        requires(has_resource&& requires(allocator_type const& the_alloc, Args... args) {
            super{the_alloc, stl::forward<Args>(args)...};
        }) constexpr object(alloc_pack_type& alloc_pack, res_ref res, Args&&... args)
          : super{alloc_pack.template get_allocator<allocator_type, resource_type>(res),
                  stl::forward<Args>(args)...} {}

        template <typename... Args>
        requires(has_resource && sizeof...(Args) > 0 && // to resolve some ambiguity with the above version
                 requires(allocator_type const& the_alloc, Args... args) {
                     super{stl::forward<Args>(args)..., the_alloc};
                 }) constexpr object(alloc_pack_type& alloc_pack, res_ref res, Args&&... args)
          : super{stl::forward<Args>(args)...,
                  alloc_pack.template get_allocator<allocator_type, resource_type>(res)} {}
    };


    template <typename T, typename StackType, AllocatorDescriptorList AllocDescList>
    struct local : object<T, alloc::local_features, AllocDescList> {
      private:
        using super = object<T, alloc::local_features, AllocDescList>;

      public:
        using stack_type      = StackType;
        using resource_type   = typename super::resource_type;
        using alloc_pack_type = alloc::allocator_pack<AllocDescList>;

        stack_type                                             buffer;
        [[no_unique_address]] istl::void_holder<resource_type> res;

        template <typename... Args>
        constexpr local(alloc_pack_type& alloc_pack, Args&&... args)
          : res{buffer.data(), buffer.size(), &alloc_pack.general_resource()},
            super{alloc_pack, res, stl::forward<Args>(args)...} {}

        template <typename... Args>
        requires(stl::is_void_v<resource_type>) constexpr local(alloc_pack_type& alloc_pack, Args&&... args)
          : super{alloc_pack, stl::forward<Args>(args)...} {}

        constexpr auto global_copy() {}
    };

    template <typename T, AllocatorDescriptorList AllocDescType, typename... Args>
    static constexpr local<T, stack<>, AllocDescType>
    make_local(alloc::allocator_pack<AllocDescType>& alloc_pack, Args&&... args) {
        return {alloc_pack, stl::forward<Args>(args)...};
    }

    template <typename T, AllocatorDescriptorList AllocDescType, typename... Args>
    static constexpr auto make_general(alloc::allocator_pack<AllocDescType>& alloc_pack, Args&&... args) {
        return alloc_pack.template general<T, Args...>(stl::forward<Args>(args)...);
    }

} // namespace webpp::object

#endif // WEBPP_OBJECT_HPP
