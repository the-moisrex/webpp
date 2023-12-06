// Created by moisrex on 12/2/20.

#ifndef WEBPP_ALLOCATOR_CONCEPTS_HPP
#define WEBPP_ALLOCATOR_CONCEPTS_HPP

#include "../std/concepts.hpp"
#include "../std/tag_invoke.hpp"
#include "../std/type_traits.hpp"

#include <memory>

namespace webpp {


    // Name requirement: https://en.cppreference.com/w/cpp/named_req/Allocator
    template <typename A>
    concept AllocatorType = requires {
        typename A::value_type;
        typename stl::allocator_traits<A>::pointer;
        typename stl::allocator_traits<A>::size_type;
        typename stl::allocator_traits<A>::const_pointer;
        typename stl::allocator_traits<A>::void_pointer;
        typename stl::allocator_traits<A>::const_void_pointer;
        requires requires(
          A                                                     alloc,
          A                                                     alloc2,
          typename stl::allocator_traits<A>::size_type          n,
          typename stl::allocator_traits<A>::pointer            ptr,
          typename stl::allocator_traits<A>::const_pointer      const_ptr,
          typename stl::allocator_traits<A>::void_pointer       void_ptr,
          typename stl::allocator_traits<A>::const_void_pointer cvp,
          decltype(*ptr)                                        res) {
            alloc.allocate(n);
            *ptr;
            *const_ptr;
            // {p->m};
            // {cp->m};

            static_cast<typename stl::allocator_traits<A>::pointer>(void_ptr);
            static_cast<typename stl::allocator_traits<A>::const_pointer>(cvp);
            stl::pointer_traits<typename stl::allocator_traits<A>::pointer>::pointer_to(res);
            alloc == alloc2;
            alloc != alloc2;
            A(alloc);
            // {A a1 = a};
            // A a(b)};

            A(stl::move(alloc));
            // {A a1 = stl::move(a)};
            // {A a(stl::move(b))};
        };
    };

    template <typename A>
    concept Allocator = AllocatorType<stl::remove_cvref_t<A>>;

    template <typename T, typename Alloc>
    concept AllocatorOf = Allocator<Alloc> && stl::is_same_v<typename Alloc::value_type, T>;

    // static_assert(Allocator<stl::allocator<int>>, "There's a problem with Allocator concept");

    /**
     * Get Allocator is a CPO (Customization Point Object) that helps to get the allocator out of a "resource"
     * that the allocator returned.
     *
     * For example, in traits type user, instead of returning an allocator, they will return a resource,
     * (std::pmr::monotonic_buffer_resource for example), but then they specialize this CPO and return
     * std::pmr::polymorphic_allocator initialized with that resource.
     *
     * This is designed for the user to easily support both std::pmr-style allocators that have resources,
     * and also std::allocator-style allocators that don't have resources.
     */
    static constexpr struct construct_allocator_from_type {
        /// Customization Point
        template <typename T>
            requires stl::tag_invocable<construct_allocator_from_type, T>
        [[nodiscard]] constexpr stl::tag_invoke_result_t<construct_allocator_from_type, T> operator()(
          T&& resource) const noexcept(stl::nothrow_tag_invocable<construct_allocator_from_type, T>) {
            return stl::tag_invoke(*this, stl::forward<T>(resource));
        }

        /// default impl: return the allocator itself
        template <typename T>
        [[nodiscard]] friend constexpr decltype(auto) tag_invoke(
          [[maybe_unused]] construct_allocator_from_type tag,
          T&&                                            alloc) noexcept {
            return stl::forward<T>(alloc);
        }

        /// handle rvalue reference inputs, the library should not use this, it's just for metaprogamming
        template <typename T>
            requires(!stl::is_lvalue_reference_v<T>)
        [[nodiscard]] friend constexpr decltype(auto) tag_invoke(
          [[maybe_unused]] construct_allocator_from_type tag,
          T&&                                            inp_res) noexcept {                                             // NOLINT(*-missing-std-forward)
            return stl::tag_invoke(construct_allocator_from_type{}, inp_res); // pass as lvalue
        }

        /// void impl: return void
        friend constexpr void tag_invoke([[maybe_unused]] construct_allocator_from_type tag) noexcept {
            // return void;
        }
    } construct_allocator_from;

    /// one single allocator descriptor which describes an allocator and its features and its resources
    template <typename D>
    concept GeneralAllocatorDescriptor = requires {
        typename D::template allocator_type<char>; // get the allocator itself
        D::template construct_allocator<char>();
        {
            construct_allocator_from(D::template construct_allocator<char>())
        } -> istl::cvref_as<typename D::template allocator_type<char>>;
    } || stl::same_as<D, void>;


    /// Allocator Descriptor for a monotonic allocator
    /// A monotonic allocator is an allocator that it's deallocation is a no-op, operates on a buffer,
    /// and mostly is used for as a local allocator that operates on a stack
    ///
    /// It is the same as "std::pmr::monotonic_buffer_resource", but we don't specify an "upstream" idea here,
    /// even though it's perfectly possible to have one.
    ///
    /// Attention: std::pmr::monotonic_buffer_resource is a resource not an allocator, this concept requires
    /// you to return an allocator, that means you have to deal with the resource management yourself
    template <typename D>
    concept MonotonicAllocatorDescriptor = requires {
        typename D::template allocator_type<char>; // get the allocator itself
        requires requires(void* buffer, stl::size_t size) {
            {
                construct_allocator_from(D::template construct_allocator<char>(buffer, size))
            } -> istl::cvref_as<typename D::template allocator_type<char>>;
        };
    } || stl::same_as<D, void>;


    template <typename D>
    concept AllocatorDescriptor = MonotonicAllocatorDescriptor<D> || GeneralAllocatorDescriptor<D>;

    template <typename T, AllocatorDescriptor Desc>
    struct resource_type_of {
        // for a general allocator descriptor, just return the allocator type
        using type = stl::tag_invoke_result_t<construct_allocator_from_type,
                                              decltype(Desc::template construct_allocator<T>())>;
    };

    template <typename T, AllocatorDescriptor Desc>
        requires stl::is_void_v<Desc>
    struct resource_type_of<T, Desc> {
        using type = void;
    };

    template <typename T, AllocatorDescriptor Desc>
        requires MonotonicAllocatorDescriptor<Desc>
    struct resource_type_of<T, Desc> {
        using type = decltype(Desc::template construct_allocator<T>(nullptr, 0));
    };

    /**
     * Get the resource type of the specified Allocator Descriptor according to the specified type
     * The resource MIGHT be the same as allocator type, but that's not the case for std::pmr traits, that's
     * why we need to identify the resource type.
     */
    template <typename T, AllocatorDescriptor Desc>
    using resource_type_of_t = typename resource_type_of<T, Desc>::type;

} // namespace webpp

#endif // WEBPP_ALLOCATOR_CONCEPTS_HPP
