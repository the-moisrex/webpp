#ifndef WEBPP_STD_MEMORY_HPP
#define WEBPP_STD_MEMORY_HPP

#include "./type_traits.hpp"

#include <boost/smart_ptr/allocate_unique.hpp>
#include <memory>
#include <utility>


namespace webpp::istl {


    // doesn't support copy/move of T with other types of Allocator...
    template <typename T, typename Allocator = stl::allocator<T>>
    struct dynamic {
        // attention: value_type might be an incomplete type at the type of constructing dynamic because it's
        // one of this classes' use cases. so you're not to change this class in a way that'll throw an error
        // for an incomplete type.
        using value_type   = stl::remove_pointer_t<stl::remove_reference_t<T>>;
        using alloc_traits = typename stl::allocator_traits<
          stl::remove_reference_t<Allocator>>::template rebind_traits<value_type>;
        using allocator_type = typename alloc_traits::allocator_type;
        using size_type      = typename alloc_traits::size_type;
        using pointer        = typename alloc_traits::pointer;


      private:
        // alloc needs to be before the ptr because it is required for constructing the ptr
        [[no_unique_address]] allocator_type alloc;
        pointer                              ptr{nullptr};


      public:
        explicit constexpr dynamic(allocator_type const& input_alloc) noexcept : alloc{input_alloc} {}

        // You don't have to pass the allocator if the allocator is default constructible.
        // This will allocate the space, but only constructs the object if it's default constructible
        // constexpr dynamic() noexcept
        //     requires(stl::is_default_constructible_v<allocator_type>)
        // : alloc{},
        //   ptr{allocator_traits::allocate(alloc, 1)} {
        //     if constexpr (stl::is_default_constructible_v<value_type>) {
        //         allocator_traits::construct(alloc, ptr); // default construct
        //     }
        // }

        template <typename... Args>
        // requires(stl::is_constructible_v<value_type, Args...>)
        constexpr dynamic(allocator_type const& input_alloc, Args&&... args)
          : alloc{input_alloc},
            ptr{alloc_traits::allocate(alloc, 1)} {
            static_assert(stl::is_constructible_v<T, Args...>,
                          "The specified type is cannot be initialized with the specified arguments.");
            alloc_traits::construct(alloc, ptr, stl::forward<Args>(args)...);
        }

        template <typename C, typename... Args>
            requires(stl::is_constructible_v<C, Args...>)
        constexpr dynamic(stl::type_identity<C>, allocator_type const& input_alloc, Args&&... args)
          : alloc{input_alloc} {
            emplace<C, Args...>(stl::forward<Args>(args)...);
        }

        // template <typename... Args>
        //     requires(stl::is_default_constructible_v<allocator_type> /*&&
        //              stl::is_constructible_v<value_type, Args...>*/)
        // constexpr dynamic(Args&&... args) : alloc{},
        //                                     ptr{allocator_traits::allocate(alloc, 1)} {
        //     allocator_traits::construct(alloc, ptr, stl::forward<Args>(args)...);
        // }

        constexpr dynamic(dynamic const& other) : alloc(other.alloc), ptr{nullptr} {
            if (other.ptr) {
                ptr = alloc_traits::allocate(alloc, 1);
                // todo: if T is a virtual type, then this will call a copy constructor on a virtual type:
                // https://isocpp.org/wiki/faq/virtual-functions#virtual-ctors
                alloc_traits::construct(alloc, ptr, *other.ptr);
            }
        }

        constexpr dynamic(dynamic&& other) noexcept
          : alloc{other.alloc},
            ptr{stl::exchange(other.ptr, nullptr)} {
            // static_assert(stl::is_move_constructible_v<T>, "The specified type is not move
            // constructible.");
        }


        constexpr dynamic& operator=(value_type const& val) noexcept {
            if (ptr) {
                alloc_traits::destroy(alloc, ptr);
            } else {
                ptr = alloc_traits::allocate(alloc, 1);
            }
            alloc_traits::construct(alloc, ptr, val);
            return *this;
        }

        constexpr dynamic& operator=(dynamic const& other) noexcept {
            *ptr = *other.ptr;
            if constexpr (stl::is_copy_assignable_v<allocator_type>) {
                alloc = other.alloc;
            }
            return *this;
        }

        constexpr dynamic& operator=(dynamic&& other) noexcept {
            if (&other != this) {
                if constexpr (stl::is_copy_assignable_v<allocator_type>) {
                    alloc = other.alloc;
                }
                ptr       = other.ptr;
                other.ptr = nullptr; // todo: yank out it's life instead of this
            }
            return *this;
        }

        constexpr dynamic& operator=(stl::nullptr_t) noexcept {
            destroy();
            ptr = nullptr;
            return *this;
        }

        constexpr auto operator<=>(value_type const& val) const noexcept {
            return *ptr <=> val;
        }

        template <typename C>
            requires(stl::is_base_of_v<T, C>)
        constexpr dynamic& operator=(C&& new_type) noexcept {
            emplace<C>(stl::forward<C>(new_type));
            return *this;
        }


        // dynamic is not designed to be used for polymorphism. no need for "virtual" keyword
        // virtual ~dynamic() {
        //     destroy();
        // }

        constexpr ~dynamic() {
            destroy();
        }

        constexpr void swap(dynamic& other) noexcept(noexcept(stl::swap(alloc, other.alloc))) {
            stl::swap(ptr, other.ptr);
            stl::swap(alloc, other.alloc);
        }


        constexpr T& operator*() & noexcept {
            return *ptr;
        }

        constexpr const T& operator*() const& noexcept {
            return *ptr;
        }

        constexpr T&& operator*() && noexcept {
            return *ptr;
        }

        constexpr const T&& operator*() const&& noexcept {
            return *ptr;
        }

        constexpr pointer operator->() noexcept {
            return ptr;
        }

        constexpr pointer operator->() const noexcept {
            return ptr;
        }

        constexpr operator T() const noexcept(stl::is_nothrow_copy_constructible_v<value_type>) {
            return *ptr;
        }

        // void reset() noexcept { destroy(); release(); }
        // pointer release() noexcept {  }

        [[nodiscard]] constexpr bool valid() const noexcept {
            return ptr != nullptr;
        }

        void reset() {
            destroy();
            ptr = nullptr;
        }

        template <typename C>
            requires(stl::is_base_of_v<T, C>)
        constexpr C& as() noexcept {
            return *static_cast<C*>(ptr);
        }


        template <typename C, typename... Args>
            requires(stl::is_base_of_v<T, C> && stl::is_constructible_v<C, Args...>)
        constexpr dynamic& emplace(Args&&... args) {
            using new_allocator_traits = typename alloc_traits::template rebind_traits<C>;
            using new_allocator_type   = typename new_allocator_traits::allocator_type;
            using new_pointer          = typename new_allocator_traits::pointer;

            // we will be using the old allocated area if the new type can be constructed inside that size
            constexpr bool should_resize = sizeof(C) > sizeof(T);

            new_allocator_type new_alloc{alloc};
            new_pointer        new_ptr;

            if (ptr) {
                alloc_traits::destroy(alloc, ptr);
                if constexpr (should_resize) {
                    alloc_traits::deallocate(alloc, ptr, 1);
                    new_ptr = new_allocator_traits::allocate(new_alloc, 1);
                } else {
                    new_ptr = static_cast<new_pointer>(ptr);
                }
            } else {
                new_ptr = new_allocator_traits::allocate(new_alloc, 1);
            }
            new_allocator_traits::construct(new_alloc, new_ptr, stl::forward<Args>(args)...);
            ptr = static_cast<pointer>(new_ptr);
            return *this;
        }

      private:
        constexpr inline void destroy() {
            if (ptr) {
                alloc_traits::destroy(alloc, ptr);
                alloc_traits::deallocate(alloc, ptr, 1);
            }
        }
    };



    namespace pmr {

        template <typename T>
        using dynamic = dynamic<T, stl::pmr::polymorphic_allocator<stl::byte>>;

    }


} // namespace webpp::istl

#endif