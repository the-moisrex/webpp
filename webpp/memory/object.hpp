// Created by moisrex on 1/11/21.

#ifndef WEBPP_OBJECT_HPP
#define WEBPP_OBJECT_HPP

#include "../std/memory.hpp"
#include "../traits/traits.hpp"

namespace webpp::object {

    template <typename T, AllocatorHolder HolderT, typename... Args>
    [[nodiscard]] static constexpr T make_object(HolderT&& holder, Args&&... args) {
        using value_type = typename stl::allocator_traits<T>::value_type;
        auto const alloc = stl::forward<HolderT>(holder).template get_allocator<value_type>();
        // return T{alloc, stl::forward<Args>(args)..., alloc};
        return stl::make_obj_using_allocator<T>(alloc, stl::forward<Args>(args)...);
    }

} // namespace webpp::object

#endif // WEBPP_OBJECT_HPP
