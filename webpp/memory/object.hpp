// Created by moisrex on 1/11/21.

#ifndef WEBPP_OBJECT_HPP
#define WEBPP_OBJECT_HPP

#include "allocator_pack.hpp"
#include "stack.hpp"

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

    /**
     * An object type which changes the allocator type based on the features you pass to it.
     */
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

      protected:
        /// These are the concepts that's going to be used to choose the best constructors

        template <typename... Args>
        static constexpr bool support_tag_alloc_args =
          requires(allocator_type const& the_alloc, Args... args) {
              super{stl::allocator_arg, the_alloc, stl::forward<Args>(args)...};
          };

        template <typename... Args>
        static constexpr bool support_alloc_args = requires(allocator_type const& the_alloc, Args... args) {
            super{the_alloc, stl::forward<Args>(args)...};
        };

        template <typename... Args>
        static constexpr bool support_args_alloc = requires(allocator_type const& the_alloc, Args... args) {
            super{stl::forward<Args>(args)..., the_alloc};
        };


        template <typename... Args>
        static constexpr bool support_tag_args = requires(Args... args) {
            super{stl::allocator_arg, stl::forward<Args>(args)...};
        };

        template <typename... Args>
        static constexpr bool support_args = requires(Args... args) { super{stl::forward<Args>(args)...}; };

      public:
        // let the user know what's missing with a better error message.
        template <typename... Args>
        explicit constexpr object([[maybe_unused]] alloc_pack_type& alloc_pack,
                                  [[maybe_unused]] Args&&... args) { // NOLINT(*-missing-std-forward)
            static_assert(!has_resource, "This allocator has resources, you need to pass it");
        }

        /// these 2 are for when the object doesn't support allocators at all.

        template <typename... Args>
            requires(support_tag_args<Args...> && !support_tag_alloc_args<Args...>)
        explicit constexpr object([[maybe_unused]] alloc_pack_type& alloc_pack, Args&&... args)
          : super{stl::allocator_arg, stl::forward<Args>(args)...} {}

        template <typename... Args>
            requires(support_args<Args...> && !support_alloc_args<Args...> && !support_tag_args<Args...> &&
                     !support_args_alloc<Args...>)
        constexpr object([[maybe_unused]] alloc_pack_type& alloc_pack,
                         [[maybe_unused]] res_ref          res,
                         Args&&... args)
          : super{stl::forward<Args>(args)...} {}

        /// these 3 are for when we don't have resource and the user is smart enough to not pass one as well

        template <typename... Args>
            requires(!has_resource && support_tag_alloc_args<Args...>)
        explicit constexpr object(alloc_pack_type& alloc_pack, Args&&... args)
          : super{stl::allocator_arg,
                  alloc_pack.template get_allocator<allocator_type, void>(),
                  stl::forward<Args>(args)...} {}

        template <typename... Args>
            requires(!has_resource && support_alloc_args<Args...>)
        explicit constexpr object(alloc_pack_type& alloc_pack, Args&&... args)
          : super{alloc_pack.template get_allocator<allocator_type, void>(), stl::forward<Args>(args)...} {}

        template <typename... Args>
            requires(!has_resource && sizeof...(Args) > 0 && // to resolve some ambiguity with the above
                                                             // version
                     support_args_alloc<Args...>)
        explicit constexpr object(alloc_pack_type& alloc_pack, Args&&... args)
          : super{stl::forward<Args>(args)..., alloc_pack.template get_allocator<allocator_type, void>()} {}

        /// these 3 are for when we don't have a resource but the user of this class passes an empty one

        template <typename... Args>
            requires(!has_resource && support_tag_alloc_args<Args...>)
        explicit constexpr object(alloc_pack_type&                    alloc_pack,
                                  [[maybe_unused]] istl::nothing_type nothing,
                                  Args&&... args)
          : super{stl::allocator_arg,
                  alloc_pack.template get_allocator<allocator_type, void>(),
                  stl::forward<Args>(args)...} {}

        template <typename... Args>
            requires(!has_resource && support_alloc_args<Args...> && !support_tag_alloc_args<Args...>)
        constexpr object(alloc_pack_type&                    alloc_pack,
                         [[maybe_unused]] istl::nothing_type nothing,
                         Args&&... args)
          : super{alloc_pack.template get_allocator<allocator_type, void>(), stl::forward<Args>(args)...} {}

        template <typename... Args>
            requires(!has_resource && sizeof...(Args) > 0 && // to resolve some ambiguity with the above
                                                             // version
                     support_args_alloc<Args...>)
        constexpr object(alloc_pack_type&                    alloc_pack,
                         [[maybe_unused]] istl::nothing_type nothing,
                         Args&&... args)
          : super{stl::forward<Args>(args)..., alloc_pack.template get_allocator<allocator_type, void>()} {}

        /// these 3 are for when you pass a valid resource

        template <typename... Args>
            requires(has_resource && support_tag_alloc_args<Args...>)
        constexpr object(alloc_pack_type& alloc_pack, res_ref res, Args&&... args)
          : super{stl::allocator_arg,
                  alloc_pack.template get_allocator<allocator_type, resource_type>(res),
                  stl::forward<Args>(args)...} {}

        template <typename... Args>
            requires(has_resource && support_alloc_args<Args...> && !support_tag_alloc_args<Args...>)
        constexpr object(alloc_pack_type& alloc_pack, res_ref res, Args&&... args)
          : super{alloc_pack.template get_allocator<allocator_type, resource_type>(res),
                  stl::forward<Args>(args)...} {}

        template <typename... Args>
            requires(has_resource && sizeof...(Args) > 0 && // to resolve some ambiguity with the above
                                                            // version
                     support_args_alloc<Args...>)
        constexpr object(alloc_pack_type& alloc_pack, res_ref res, Args&&... args)
          : super{stl::forward<Args>(args)...,
                  alloc_pack.template get_allocator<allocator_type, resource_type>(res)} {}
    };

    template <typename T, AllocatorDescriptorList AllocDescList>
    using general = object<T, alloc::general_features, AllocDescList>;

    /**
     * Local object.
     * This struct will help you to have an "Stack" and also pass the stack resource to the object's allocator
     *
     * @tparam T
     * @tparam StackType
     * @tparam AllocDescList
     */
    template <typename T, typename StackType, AllocatorDescriptorList AllocDescList>
    struct local
      : public details::resource_holder<
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
        explicit constexpr local(alloc_pack_type& alloc_pack, Args&&... args)
          : res_holder{.resource_holder_data{}, // the stack buffer
                       .resource_holder_res = resource_type{res_holder::resource_holder_data.data(),
                                                            res_holder::resource_holder_data.size(),
                                                            &alloc_pack.general_resource()}},
            super{alloc_pack, res_holder::resource_holder_res, stl::forward<Args>(args)...} {}

        template <typename... Args>
            requires(stl::is_void_v<resource_type>)
        explicit constexpr local(alloc_pack_type& alloc_pack, Args&&... args)
          : super{alloc_pack, stl::forward<Args>(args)...} {}

        // todo
        constexpr auto global_copy() {}
    };

    template <typename T, AllocatorDescriptorList AllocDescType, typename... Args>
    static constexpr auto make_general(alloc::allocator_pack<AllocDescType>& alloc_pack, Args&&... args) {
        return alloc_pack.template general<T, Args...>(stl::forward<Args>(args)...);
    }

    template <typename T, typename AllocHolderType, typename... Args>
    static constexpr auto make_general(AllocHolderType&& holder, Args&&... args) {
        if constexpr (requires { holder.alloc_pack; }) {
            return holder.alloc_pack.template general<T, Args...>(stl::forward<Args>(args)...);
        } else if constexpr (requires { holder.get_allocator(); }) {
            if constexpr (requires {
                              T{stl::allocator_arg, holder.get_allocator(), stl::forward<Args>(args)...};
                          })
            {
                return T{stl::allocator_arg, holder.get_allocator(), stl::forward<Args>(args)...};
            } else if constexpr (requires { T{stl::forward<Args>(args)..., holder.get_allocator()}; }) {
                return T{stl::forward<Args>(args)..., holder.get_allocator()};
            } else {
                // don't know how to use the allocator
                return T{stl::forward<Args>(args)...};
            }
        } else {
            // no allocator
            return T{stl::forward<Args>(args)...};
        }
    }

    template <typename T, AllocatorDescriptorList AllocDescType, typename... Args>
    static constexpr local<T, stack<>, AllocDescType> make_local(
      alloc::allocator_pack<AllocDescType>& alloc_pack,
      Args&&... args) {
        return local<T, stack<>, AllocDescType>{alloc_pack, stl::forward<Args>(args)...};
    }

    template <typename T, typename AllocHolderType, typename... Args>
    static constexpr auto make_local(AllocHolderType&& holder, Args&&... args) {
        if constexpr (requires { holder.alloc_pack; }) {
            return make_local<T>(holder.alloc_pack, stl::forward<Args>(args)...);
        } else {
            // no local allocator
            return make_general<T>(stl::forward<AllocHolderType>(holder), stl::forward<Args>(args)...);
        }
    }

    // This won't change the allocator type
    template <typename T, AllocatorDescriptorList AllocDescType, typename... Args>
    static constexpr T make(alloc::allocator_pack<AllocDescType>& alloc_pack, Args&&... args) {
        using alloc_pack_type = alloc::allocator_pack<AllocDescType>;
        if constexpr (requires { typename T::allocator_type; }) {
            using allocator_type = typename T::allocator_type;
            if constexpr (alloc_pack_type::template has_allocator<allocator_type>) {
                return alloc_pack.template make<T, Args...>(stl::forward<Args>(args)...);
            } else {
                return T{stl::forward<T>(args)...};
            }
        } else {
            return T{stl::forward<T>(args)...};
        }
    }

    // This won't change the allocator type
    template <typename T, typename AllocHolderType, typename... Args>
    static constexpr T make(AllocHolderType&& holder, Args&&... args) {
        using alloc_pack_type = typename stl::remove_cvref_t<AllocHolderType>::allocator_pack_type;
        using alloc_desc_type = typename alloc_pack_type::allocator_descriptors;
        return make<T, alloc_desc_type, Args...>(holder.alloc_pack, stl::forward<Args>(args)...);
    }
} // namespace webpp::object

#endif // WEBPP_OBJECT_HPP
