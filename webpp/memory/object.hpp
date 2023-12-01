// Created by moisrex on 1/11/21.

#ifndef WEBPP_OBJECT_HPP
#define WEBPP_OBJECT_HPP

#include "allocator_concepts.hpp"
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

    template <typename T, Allocator AllocT, typename... Args>
    [[nodiscard]] static constexpr T make(AllocT const& alloc, Args&&... args) {
        if constexpr (stl::constructible_from<T, stl::allocator_arg_t, AllocT, Args...>) {
            return T{stl::allocator_arg, alloc, stl::forward<Args>(args)...};
        } else if constexpr (stl::constructible_from<T, stl::allocator_arg_t, AllocT, Args...>) {
            return T{stl::allocator_arg, alloc, stl::forward<Args>(args)...};
        } else if constexpr (stl::constructible_from<T, Args..., AllocT>) {
            return T{stl::forward<Args>(args)..., alloc};
        } else if constexpr (stl::constructible_from<T, AllocT, Args...>) {
            return T{alloc, stl::forward<Args>(args)...};
        } else if constexpr (stl::constructible_from<T, Args...>) {
            return T{stl::forward<Args>(args)...};
        } else {
            static_assert_false(T, "We don't know how to construct the specified type.");
            return T{}; // to get rid of the warning
        }
    }

    /**
     * Local object.
     * This struct will help you to have an "Stack" and also pass the stack resource to the object's allocator
     *
     * @tparam T
     * @tparam StackType
     * @tparam AllocDesc
     */
    template <typename T, typename StackType, AllocatorDescriptor AllocDesc>
    struct local : details::resource_holder<resource_type_of_t<T, AllocDesc>, StackType>, T {
        using resource_type = resource_type_of_t<T, AllocDesc>;

      private:
        using res_holder = details::resource_holder<resource_type, StackType>;

      public:
        using stack_type = StackType;

        template <typename... Args>
        explicit constexpr local(Args&&... args)
          : res_holder{.resource_holder_data{}, // the stack buffer
                       .resource_holder_res = AllocDesc::template construct_allocator<T>(
                         static_cast<void*>(res_holder::resource_holder_data.data()),
                         res_holder::resource_holder_data.size())},
            T{make<T>(construct_allocator_from(this->resource_holder_res), stl::forward<Args>(args)...)} {}

        // todo
        // constexpr auto copy_general(alloc_pack_type& alloc_pack) {
        //     return general<T, AllocDescList>(alloc_pack, *this);
        // }
    };

    template <typename T, AllocatorHolder HolderT, typename... Args>
    [[nodiscard]] static constexpr T make_general(HolderT&& holder, Args&&... args) {
        auto const& alloc = stl::forward<HolderT>(holder).template general_allocator<T>();
        return make<T>(alloc, stl::forward<Args>(args)...);
    }

    template <typename T, MonotonicAllocatorDescriptor AllocDesc, typename... Args>
    [[nodiscard]] static constexpr local<T, stack<>, AllocDesc> make_local(Args&&... args) {
        return local<T, stack<>, AllocDesc>{stl::forward<Args>(args)...};
    }

    // todo: remove this? in older versions, etraits holded the local allocator as well, now we don't!
    template <typename T, EnabledTraits ET, typename... Args>
    [[nodiscard]] static constexpr local<
      T,
      stack<>,
      typename stl::remove_cvref_t<ET>::traits_type::monotonic_allocator_descriptor>
    make_local([[maybe_unused]] ET&& etraits, Args&&... args) {
        using allocator_desc_type =
          typename stl::remove_cvref_t<ET>::traits_type::monotonic_allocator_descriptor;
        return local<T, stack<>, allocator_desc_type>{stl::forward<Args>(args)...};
    }

} // namespace webpp::object

#endif // WEBPP_OBJECT_HPP
