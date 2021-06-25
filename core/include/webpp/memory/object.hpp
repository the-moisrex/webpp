// Created by moisrex on 1/11/21.

#ifndef WEBPP_OBJECT_HPP
#define WEBPP_OBJECT_HPP

#include "allocator_pack.hpp"
#include "buffer.hpp"

namespace webpp::object {

    namespace details {

        template <typename ResType, typename Data>
        struct resource_holder {
            Data    resource_holder_data;
            ResType resource_holder_res;
        };

        template <typename Data>
        struct resource_holder<void, Data> {};
    } // namespace details

    template <typename T, alloc::feature_pack FPack, AllocatorDescriptorList AllocDescList>
    struct object : public alloc::alloc_finder<T, FPack, AllocDescList>::new_type {
      protected:
        using alloc_details = alloc::alloc_finder<T, FPack, AllocDescList>;
        using super         = typename alloc_details::new_type;

      public:
        using object_type         = super;
        using allocator_type      = typename alloc_details::allocator_type;
        using resource_type       = typename alloc_details::resource_type;
        using resource_type_field = typename alloc_details::resource_type_field;
        using alloc_pack_type     = alloc::allocator_pack<AllocDescList>;

        static constexpr bool has_resource = !stl::is_void_v<resource_type>;
        using res_ref =
          stl::conditional_t<has_resource, stl::add_lvalue_reference_t<resource_type>, istl::nothing_type>;

        /// these 2 are for when the object doesn't support allocators at all.

        template <typename... Args>
        requires(!requires(allocator_type const& the_alloc, Args... args) {
            super{stl::allocator_arg, the_alloc, stl::forward<Args>(args)...};
        }) constexpr object([[maybe_unused]] alloc_pack_type& alloc_pack, Args&&... args)
          : super{stl::allocator_arg, stl::forward<Args>(args)...} {}

        template <typename... Args>
        requires(!requires(allocator_type const& the_alloc, Args... args) {
            super{the_alloc, stl::forward<Args>(args)...};
        }) constexpr object([[maybe_unused]] alloc_pack_type& alloc_pack,
                            [[maybe_unused]] res_ref          res,
                            Args&&... args)
          : super{stl::forward<Args>(args)...} {}

        /// these 3 are for when we don't have resource and the user is smart enough to not pass one as well

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


        /// these 3 are for when we don't have a resource but the user of this class passes an empty one

        template <typename... Args>
        requires(!has_resource &&
                 requires(allocator_type const& the_alloc, Args... args) {
                     super{stl::allocator_arg, the_alloc, stl::forward<Args>(args)...};
                 }) constexpr object(alloc_pack_type& alloc_pack,
                                     [[maybe_unused]] istl::nothing_type,
                                     Args&&... args)
          : super{stl::allocator_arg,
                  alloc_pack.template get_allocator<allocator_type, void>(),
                  stl::forward<Args>(args)...} {}

        template <typename... Args>
        requires(!has_resource &&
                 requires(allocator_type const& the_alloc, Args... args) {
                     super{the_alloc, stl::forward<Args>(args)...};
                 }) constexpr object(alloc_pack_type& alloc_pack,
                                     [[maybe_unused]] istl::nothing_type,
                                     Args&&... args)
          : super{alloc_pack.template get_allocator<allocator_type, void>(), stl::forward<Args>(args)...} {}

        template <typename... Args>
        requires(!has_resource && sizeof...(Args) > 0 && // to resolve some ambiguity with the above version
                 requires(allocator_type const& the_alloc, Args... args) {
                     super{stl::forward<Args>(args)..., the_alloc};
                 }) constexpr object(alloc_pack_type& alloc_pack,
                                     [[maybe_unused]] istl::nothing_type,
                                     Args&&... args)
          : super{stl::forward<Args>(args)..., alloc_pack.template get_allocator<allocator_type, void>()} {}


        /// these 3 are for when you pass a valid resource

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



    /**
     * Local object.
     * This struct will help you to have an "Stack" and also pass the stack resource to the object's allocator
     *
     * @tparam T
     * @tparam StackType
     * @tparam AllocDescList
     */
    template <typename T, typename StackType, AllocatorDescriptorList AllocDescList>
    struct local : public details::resource_holder<
                     typename alloc::alloc_finder<T, alloc::local_features, AllocDescList>::resource_type,
                     StackType>,
                   public object<T, alloc::local_features, AllocDescList> {
      private:
        using super      = object<T, alloc::local_features, AllocDescList>;
        using res_holder = details::resource_holder<typename super::resource_type, StackType>;

      public:
        using stack_type      = StackType;
        using resource_type   = typename super::resource_type;
        using alloc_pack_type = alloc::allocator_pack<AllocDescList>;


        template <typename... Args>
        constexpr local(alloc_pack_type& alloc_pack, Args&&... args)
          : res_holder{.resource_holder_data{}, // the stack buffer
                       .resource_holder_res = resource_type{res_holder::resource_holder_data.data(),
                                                            res_holder::resource_holder_data.size(),
                                                            &alloc_pack.general_resource()}},
            super{alloc_pack, res_holder::resource_holder_res, stl::forward<Args>(args)...} {}

        template <typename... Args>
        requires(stl::is_void_v<resource_type>) constexpr local(alloc_pack_type& alloc_pack, Args&&... args)
          : super{alloc_pack, stl::forward<Args>(args)...} {}

        // todo
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
