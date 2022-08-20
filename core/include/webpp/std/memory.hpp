#ifndef WEBPP_STD_MEMORY_HPP
#define WEBPP_STD_MEMORY_HPP

#include "std.hpp"

#include <boost/smart_ptr/allocate_unique.hpp>
#include <memory>
#include <utility>


namespace webpp::istl {


    // doesn't support copy/move of T with other types of Allocator...
    template <typename T, typename Allocator = stl::allocator<T>>
    struct dynamic {
        using value_type       = stl::remove_pointer_t<stl::remove_reference_t<T>>;
        using pointer          = stl::add_pointer_t<value_type>;
        using allocator_type   = stl::remove_reference_t<Allocator>;
        using allocator_traits = stl::allocator_traits<allocator_type>;
        using size_type        = typename allocator_traits::size_type;

        static constexpr size_type required_size =
          sizeof(value_type) / sizeof(typename allocator_traits::value_type);

      private:
        // alloc needs to be before the ptr because it is required for constructing the ptr
        [[no_unique_address]] allocator_type alloc{};
        pointer                              ptr{nullptr};


      public:
        constexpr dynamic(allocator_type const& input_alloc) noexcept : alloc{input_alloc} {}

        // value is nullptr,
        // and you don't have to pass the allocator if the allocator is default constructible.
        constexpr dynamic() noexcept {
            static_assert(stl::is_default_constructible_v<allocator_type>);
        }

        template <typename... Args>
        constexpr dynamic(allocator_type const& input_alloc, Args&&... args)
          : alloc{input_alloc},
            ptr(allocator_traits::allocate(alloc, required_size)) {
            allocator_traits::construct(alloc, ptr, stl::forward<Args>(args)...);
        }

        template <typename... Args>
            requires(stl::is_default_constructible_v<allocator_type>)
        constexpr dynamic(Args&&... args) : alloc{},
                                            ptr(allocator_traits::allocate(alloc, 1)) {
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

        // We can't do this because the T might be bool and that would create an ambiguity.
        // constexpr operator T() const noexcept(stl::is_nothrow_copy_constructible_v<value_type>) {
        //     return *ptr;
        // }

        // void reset() noexcept { destroy(); release(); }
        // pointer release() noexcept {  }

        constexpr operator bool() const noexcept {
            return ptr != nullptr;
        }

      private:
        constexpr void destroy() {
            if (ptr) {
                allocator_traits::destroy(alloc, ptr);
                allocator_traits::deallocate(alloc,
                                             reinterpret_cast<typename allocator_traits::pointer>(ptr),
                                             required_size);
            }
        }
    };



    namespace pmr {

        template <typename T>
        using dynamic = dynamic<T, stl::pmr::polymorphic_allocator<stl::byte>>;

    }


} // namespace webpp::istl

#endif
