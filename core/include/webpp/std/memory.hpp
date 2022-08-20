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
        using value_type       = stl::remove_pointer_t<stl::remove_reference_t<T>>;
        using allocator_traits = typename stl::allocator_traits<
          stl::remove_reference_t<Allocator>>::template rebind_traits<value_type>;
        using allocator_type = typename allocator_traits::allocator_type;
        using size_type      = typename allocator_traits::size_type;
        using pointer        = typename allocator_traits::pointer;


      private:
        // alloc needs to be before the ptr because it is required for constructing the ptr
        [[no_unique_address]] allocator_type alloc;
        pointer                              ptr{nullptr};


      public:
        constexpr dynamic(allocator_type const& input_alloc) noexcept : alloc{input_alloc} {}

        // You don't have to pass the allocator if the allocator is default constructible.
        // This will allocate the space, but only constructs the object if it's default constructible
        constexpr dynamic() noexcept
            requires(stl::is_default_constructible_v<allocator_type>)
        : alloc{},
          ptr{allocator_traits::allocate(alloc, 1)} {
            if constexpr (stl::is_default_constructible_v<value_type>) {
                allocator_traits::construct(alloc, ptr); // default construct
            }
        }

        template <typename... Args>
            requires(stl::is_constructible_v<value_type, Args...>)
        constexpr dynamic(allocator_type const& input_alloc, Args&&... args)
          : alloc{input_alloc},
            ptr{allocator_traits::allocate(alloc, 1)} {
            allocator_traits::construct(alloc, ptr, stl::forward<Args>(args)...);
        }

        template <typename... Args>
            requires(stl::is_default_constructible_v<allocator_type> &&
                     stl::is_constructible_v<value_type, Args...>)
        constexpr dynamic(Args&&... args) : alloc{},
                                            ptr{allocator_traits::allocate(alloc, 1)} {
            allocator_traits::construct(alloc, ptr, stl::forward<Args>(args)...);
        }

        constexpr dynamic(dynamic const& other)
          : alloc(other.alloc),
            ptr{allocator_traits::allocate(alloc, 1)} {
            allocator_traits::construct(alloc, ptr, *other.ptr);
        }

        constexpr dynamic(dynamic&& other) noexcept
          : alloc{other.alloc},
            ptr{stl::exchange(other.ptr, nullptr)} {}


        constexpr dynamic& operator=(value_type const& val) noexcept {
            if (ptr) {
                allocator_traits::destroy(alloc, ptr);
            }
            allocator_traits::construct(alloc, ptr, val);
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


      private:
        constexpr inline void destroy() {
            if (ptr) {
                allocator_traits::destroy(alloc, ptr);
                allocator_traits::deallocate(alloc, ptr, 1);
            }
        }
    };



    namespace pmr {

        template <typename T>
        using dynamic = dynamic<T, stl::pmr::polymorphic_allocator<stl::byte>>;

    }


} // namespace webpp::istl

#endif
