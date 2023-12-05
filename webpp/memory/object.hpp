// Created by moisrex on 1/11/21.

#ifndef WEBPP_OBJECT_HPP
#define WEBPP_OBJECT_HPP

#include "../common/meta.hpp"
#include "../traits/traits.hpp"
#include "allocator_concepts.hpp"
#include "stack.hpp"

namespace webpp::object {

    // use stl::make_obj_using_allocator
    // template <typename T, Allocator AllocT, typename... Args>
    // [[nodiscard]] static constexpr T make(AllocT const& alloc, Args&&... args) {
    //     if constexpr (stl::constructible_from<T, stl::allocator_arg_t, AllocT, Args...>) {
    //         return T{stl::allocator_arg, alloc, stl::forward<Args>(args)...};
    //     } else if constexpr (stl::constructible_from<T, stl::allocator_arg_t, AllocT, Args...>) {
    //         return T{stl::allocator_arg, alloc, stl::forward<Args>(args)...};
    //     } else if constexpr (stl::constructible_from<T, Args..., AllocT>) {
    //         return T{stl::forward<Args>(args)..., alloc};
    //     } else if constexpr (stl::constructible_from<T, AllocT, Args...>) {
    //         return T{alloc, stl::forward<Args>(args)...};
    //     } else if constexpr (stl::constructible_from<T, Args...>) {
    //         return T{stl::forward<Args>(args)...};
    //     } else {
    //         static_assert_false(AllocT, "We don't know how to construct the specified type.");
    //         return T{}; // to get rid of the warning
    //     }
    // }

    /**
     * Local object.
     * This struct will help you to have an "Stack" and also pass the stack resource to the object's allocator
     *
     * @tparam T
     * @tparam StackType
     * @tparam AllocDesc
     */
    template <typename T, typename StackType, AllocatorDescriptor AllocDesc>
        requires stl::uses_allocator_v<T, typename AllocDesc::template allocator_type<T>>
    struct local : T {
        using resource_type = resource_type_of_t<T, AllocDesc>;

      private:
        StackType                           data{};
        [[no_unique_address]] resource_type res;

      public:
        using stack_type = StackType;

        struct local_allocator_for_tag {};

        template <typename... Args>
        explicit constexpr local(Args&&... args)
          : T{stl::make_obj_using_allocator<T>(construct_allocator_from(res), stl::forward<Args>(args)...)},
            res{AllocDesc::template construct_allocator<T>(static_cast<void*>(data.data()), data.size())} {}

        // todo
        // constexpr auto copy_general(alloc_pack_type& alloc_pack) {
        //     return general<T, AllocDescList>(alloc_pack, *this);
        // }
    };

    template <typename T, AllocatorHolder HolderT, typename... Args>
    [[nodiscard]] static constexpr T make_general(HolderT&& holder, Args&&... args) {
        auto const& alloc = stl::forward<HolderT>(holder).template general_allocator<T>();
        return stl::make_obj_using_allocator<T>(alloc, stl::forward<Args>(args)...);
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
