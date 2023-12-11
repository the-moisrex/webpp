#ifndef WEBPP_STD_MEMORY_HPP
#define WEBPP_STD_MEMORY_HPP

#include "./memory_resource.hpp"
#include "./type_traits.hpp"
#include "./utility.hpp"

#include <cassert>
#include <memory>

namespace webpp::istl {

    /// a helper to initialize dynamic
    /// Usage:
    ///   dynamic<int> val{initialize};
    static constexpr struct initialize_tag {
    } initialize;

    // NOLINTBEGIN(*-avoid-c-arrays)
    namespace detail {

        template <class T>
        struct make_scalar {
            using type = T;
        };

        template <class T, std::size_t N>
        struct make_scalar<T[N]> {
            using type = typename make_scalar<T>::type;
        };

    } // namespace detail

    template <class T>
    inline constexpr T* first_scalar(T* ptr) noexcept {
        return ptr;
    }

    template <class T, std::size_t N>
    inline constexpr typename detail::make_scalar<T>::type* first_scalar(T (*ptr)[N]) noexcept {
        return first_scalar(&(*ptr)[0]);
    }

    namespace details {

        template <class T>
        struct sp_alloc_size {
            static constexpr stl::size_t value = 1;
        };

        template <class T>
        struct sp_alloc_size<T[]> {
            static constexpr stl::size_t value = sp_alloc_size<T>::value;
        };

        template <class T, stl::size_t N>
        struct sp_alloc_size<T[N]> {
            static constexpr stl::size_t value = N * sp_alloc_size<T>::value;
        };

        template <class T>
        struct sp_alloc_result {
            using type = T;
        };

        template <class T, stl::size_t N>
        struct sp_alloc_result<T[N]> {
            using type = T[];
        };

        template <class T>
        struct sp_alloc_value {
            using type = stl::remove_cv_t<stl::remove_extent_t<T>>;
        };

        template <class T, class P>
        struct sp_alloc_ptr {
            using element_type = T;

            constexpr sp_alloc_ptr() noexcept = default;

            constexpr sp_alloc_ptr([[maybe_unused]] stl::size_t inp_size, P inp_p) noexcept : p(inp_p) {}

            explicit constexpr sp_alloc_ptr([[maybe_unused]] stl::nullptr_t inp_null) noexcept {};

            constexpr T& operator*() const {
                return *p;
            }

            constexpr T* operator->() const noexcept {
                return stl::to_address(p);
            }

            constexpr explicit operator bool() const noexcept {
                return static_cast<bool>(p);
            }

            constexpr bool operator!() const noexcept {
                return !p;
            }

            constexpr P ptr() const noexcept {
                return p;
            }

            static constexpr stl::size_t size() noexcept {
                return 1;
            }

          private:
            P p{};
        };

        template <class T, class P>
        struct sp_alloc_ptr<T[], P> {
            using element_type = T;

            constexpr sp_alloc_ptr() noexcept = default;

            constexpr sp_alloc_ptr(stl::size_t const inp_n, P inp_p) noexcept : p(inp_p), n(inp_n) {}

            explicit constexpr sp_alloc_ptr(stl::nullptr_t) noexcept {};

            constexpr T& operator[](stl::size_t index) const {
                return p[index];
            }

            constexpr explicit operator bool() const noexcept {
                return static_cast<bool>(p);
            }

            constexpr bool operator!() const noexcept {
                return !p;
            }

            constexpr P ptr() const noexcept {
                return p;
            }

            [[nodiscard]] constexpr stl::size_t size() const noexcept {
                return n;
            }

          private:
            P           p{};
            stl::size_t n{};
        };

        template <class T, stl::size_t N, class P>
        struct sp_alloc_ptr<T[N], P> {
            using element_type = T;

            constexpr sp_alloc_ptr() noexcept = default;

            constexpr sp_alloc_ptr([[maybe_unused]] stl::size_t inp_n, P inp_p) noexcept : p(inp_p) {}

            explicit constexpr sp_alloc_ptr(stl::nullptr_t) noexcept {};

            constexpr T& operator[](stl::size_t index) const {
                return p[index];
            }

            constexpr explicit operator bool() const noexcept {
                return static_cast<bool>(p);
            }

            constexpr bool operator!() const noexcept {
                return !p;
            }

            constexpr P ptr() const noexcept {
                return p;
            }

            static constexpr stl::size_t size() noexcept {
                return N;
            }


          private:
            P p{};
        };

        template <class T, class P>
        constexpr bool operator==(sp_alloc_ptr<T, P> const& lhs, sp_alloc_ptr<T, P> const& rhs) {
            return lhs.ptr() == rhs.ptr();
        }

        template <class T, class P>
        constexpr bool operator!=(sp_alloc_ptr<T, P> const& lhs, sp_alloc_ptr<T, P> const& rhs) {
            return !(lhs == rhs);
        }

        template <class T, class P>
        constexpr bool operator==(sp_alloc_ptr<T, P> const& lhs, stl::nullptr_t) noexcept {
            return !lhs.ptr();
        }

        template <class T, class P>
        constexpr bool operator==(stl::nullptr_t, sp_alloc_ptr<T, P> const& rhs) noexcept {
            return !rhs.ptr();
        }

        template <class T, class P>
        constexpr bool operator!=(sp_alloc_ptr<T, P> const& lhs, stl::nullptr_t) noexcept {
            return static_cast<bool>(lhs.ptr());
        }

        template <class T, class P>
        constexpr bool operator!=(stl::nullptr_t, sp_alloc_ptr<T, P> const& rhs) noexcept {
            return static_cast<bool>(rhs.ptr());
        }

        template <class A>
        constexpr void sp_alloc_clear(
          A&                                         alloc,
          typename stl::allocator_traits<A>::pointer ptr,
          [[maybe_unused]] stl::size_t               inp_size,
          [[maybe_unused]] stl::false_type           not_true) {
            stl::destroy(alloc, stl::to_address(ptr));
        }

    } // namespace details

    // NOLINTEND(*-avoid-c-arrays)

    template <class T, class A>
    class alloc_deleter {
        using allocator =
          typename stl::allocator_traits<A>::template rebind<typename details::sp_alloc_value<T>::type>;

        [[no_unique_address]] allocator alloc;

      public:
        using pointer = details::sp_alloc_ptr<T, typename stl::allocator_traits<allocator>::pointer>;

        constexpr explicit alloc_deleter(allocator const& inp_alloc) noexcept : alloc{inp_alloc} {}

        constexpr void operator()(pointer inp_ptr) {
            details::sp_alloc_clear(alloc, inp_ptr.ptr(), inp_ptr.size(), stl::is_array<T>());
            alloc.deallocate(inp_ptr.ptr(), inp_ptr.size());
        }
    };

    namespace details {
        template <class T, class A>
        struct sp_alloc_make {
            using allocator =
              typename stl::allocator_traits<A>::template rebind<typename sp_alloc_value<T>::type>::type;

          private:
            using deleter = alloc_deleter<T, A>;
            using pointer = typename stl::allocator_traits<allocator>::pointer;

          public:
            using type       = stl::unique_ptr<typename sp_alloc_result<T>::type, deleter>;
            using value_type = typename allocator::value_type;

            sp_alloc_make(sp_alloc_make const&)            = delete;
            sp_alloc_make(sp_alloc_make&&)                 = delete;
            sp_alloc_make& operator=(sp_alloc_make const&) = delete;
            sp_alloc_make& operator=(sp_alloc_make&&)      = delete;

            constexpr sp_alloc_make(A const& inp_alloc, stl::size_t const inp_n)
              : alloc(inp_alloc),
                n(inp_n),
                ptr(stl::allocator_traits<A>::allocate(alloc, n)) {}

            constexpr ~sp_alloc_make() {
                if (ptr) {
                    alloc.deallocate(ptr, n);
                }
            }

            constexpr value_type* get() const noexcept {
                return stl::to_address(ptr);
            }

            constexpr allocator& state() noexcept {
                return alloc;
            }

            constexpr type release() noexcept {
                pointer loc_ptr = ptr;
                ptr             = pointer();
                return type(typename deleter::pointer(n, loc_ptr), deleter(alloc));
            }

          private:
            allocator   alloc;
            stl::size_t n;
            pointer     ptr;
        };

    } // namespace details

    template <class T, class A>
        requires(!stl::is_array_v<T>)
    constexpr stl::unique_ptr<T, alloc_deleter<T, A>> allocate_unique(A const& alloc) {
        details::sp_alloc_make<T, A> alloc_maker(alloc, 1);
        stl::allocator_traits<A>::construct(alloc_maker.state(), alloc_maker.get());
        return alloc_maker.release();
    }

    template <class T, class A, class... Args>
        requires(!stl::is_array_v<T>)
    constexpr stl::unique_ptr<T, alloc_deleter<T, A>> allocate_unique(A const& alloc, Args&&... args) {
        details::sp_alloc_make<T, A> alloc_maker(alloc, 1);
        stl::allocator_traits<A>::construct(
          alloc_maker.state(),
          alloc_maker.get(),
          stl::forward<Args>(args)...);
        return alloc_maker.release();
    }

    template <class T, class A>
        requires(!stl::is_array_v<T>)
    constexpr stl::unique_ptr<T, alloc_deleter<T, A>> allocate_unique(
      A const&                  alloc,
      stl::type_identity_t<T>&& value) {
        details::sp_alloc_make<T, A> alloc_maker(alloc, 1);
        stl::allocator_traits<A>::construct(alloc_maker.state(), alloc_maker.get(), stl::move(value));
        return alloc_maker.release();
    }

    template <class T, class A>
        requires(stl::is_unbounded_array_v<T>)
    constexpr stl::unique_ptr<T, alloc_deleter<T, A>> allocate_unique(A const& alloc, stl::size_t size) {
        details::sp_alloc_make<T, A> alloc_maker(alloc, size);
        stl::allocator_traits<A>::construct_n(
          alloc_maker.state(),
          first_scalar(alloc_maker.get()),
          size * details::sp_alloc_size<T>::value);
        return alloc_maker.release();
    }

    template <class T, class A>
        requires(stl::is_unbounded_array_v<T>)
    constexpr stl::unique_ptr<typename details::sp_alloc_result<T>::type, alloc_deleter<T, A>>
    allocate_unique(A const& alloc) {
        details::sp_alloc_make<T, A> alloc_maker(alloc, stl::extent_v<T>);
        stl::allocator_traits<A>::construct_n(
          alloc_maker.state(),
          first_scalar(alloc_maker.get()),
          details::sp_alloc_size<T>::value);
        return alloc_maker.release();
    }

    // doesn't support copy/move of T with other types of Allocator...
    template <typename T, typename AllocT = stl::allocator<T>>
    struct dynamic {
        // attention: value_type might be an incomplete type at the type of constructing dynamic because it's
        // one of this classes' use cases. so you're not to change this class in a way that'll throw an error
        // for an incomplete type.
        using value_type     = stl::remove_pointer_t<stl::remove_reference_t<T>>;
        using alloc_traits   = typename stl::allocator_traits<AllocT>::template rebind_traits<value_type>;
        using allocator_type = typename alloc_traits::allocator_type;
        using size_type      = typename alloc_traits::size_type;
        using pointer        = typename alloc_traits::pointer;
        using const_pointer  = stl::add_const_t<typename alloc_traits::pointer>;


      private:
        template <typename NT>
        using rebind_alloc_traits =
          typename stl::allocator_traits<allocator_type>::template rebind_traits<stl::remove_cvref_t<NT>>;

        template <typename NT>
        using rebind_alloc_type = typename rebind_alloc_traits<NT>::allocator_type;

        // alloc needs to be before the ptr because it is required for constructing the ptr
        [[no_unique_address]] allocator_type alloc;
        pointer                              ptr{nullptr};


      public:
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

        template <typename InheritedType>
            requires(!stl::is_constructible_v<value_type, InheritedType> &&
                     stl::is_base_of_v<value_type, stl::remove_cvref_t<InheritedType>>)
        constexpr dynamic(allocator_type const& input_alloc, InheritedType&& derived_obj)
          : alloc{input_alloc},
            ptr{static_cast<pointer>(rebind_alloc_traits<InheritedType>::allocate(
              istl::unmove(typename rebind_alloc_traits<InheritedType>::allocator_type{alloc}),
              1))} {
            using derived_alloc_traits = rebind_alloc_traits<InheritedType>;
            using derived_alloc_type   = typename derived_alloc_traits::allocator_type;
            using derived_pointer      = typename derived_alloc_traits::pointer;
            derived_alloc_traits::construct(
              istl::unmove(derived_alloc_type{alloc}),
              static_cast<derived_pointer>(ptr),
              stl::forward<InheritedType>(derived_obj));
        }

        template <typename InheritedType>
            requires(!stl::is_constructible_v<value_type, InheritedType> &&
                     stl::is_base_of_v<value_type, stl::remove_cvref_t<InheritedType>>)
        constexpr dynamic([[maybe_unused]] stl::allocator_arg_t tag,
                          allocator_type const&                 input_alloc,
                          InheritedType&&                       derived_obj)
          : dynamic{input_alloc, stl::forward<InheritedType>(derived_obj)} {}

        template <typename InheritedType>
            requires(!stl::is_constructible_v<value_type, InheritedType> &&
                     stl::is_base_of_v<value_type, stl::remove_cvref_t<InheritedType>>)
        constexpr dynamic(InheritedType&& derived_obj, allocator_type const& input_alloc)
          : dynamic{input_alloc, stl::forward<InheritedType>(derived_obj)} {}

        /// default construct the object
        template <typename... Args>
            requires(stl::is_constructible_v<value_type, Args...>)
        explicit constexpr dynamic([[maybe_unused]] initialize_tag tag,
                                   allocator_type const&           input_alloc,
                                   Args&&... args)
          : alloc{input_alloc},
            ptr{alloc_traits::allocate(alloc, 1)} {
            static_assert(stl::is_constructible_v<T, Args...>,
                          "The specified type is cannot be initialized with the specified arguments.");
            alloc_traits::construct(alloc, ptr, stl::forward<Args>(args)...);
        }

        /// default construct the object
        template <typename... Args>
            requires(stl::is_constructible_v<value_type, Args...>)
        explicit constexpr dynamic(allocator_type const& input_alloc, initialize_tag tag, Args&&... args)
          : dynamic{tag, input_alloc, stl::forward<Args>(args)...} {}

        /// default construct the object
        explicit constexpr dynamic([[maybe_unused]] initialize_tag tag)
            requires(is_complete_v<value_type> && stl::is_default_constructible_v<allocator_type>)
          : dynamic{allocator_type{}} {
            // we're not adding this as a constraint to the constructor in order to let the user use
            // incomplete types
            static_assert(stl::is_default_constructible_v<value_type>,
                          "This type must be default constructible");
        }

        /// construct nothing because the type is not constructible (pure virtual classes), you now have to
        /// use .emplace
        /// we're not adding this as a constraint to the constructor in order to let the user use
        /// incomplete types
        constexpr dynamic() noexcept
            requires(stl::is_default_constructible_v<allocator_type>)
        = default;

        /// construct nothing because the type is not constructible yet (pure virtual classes), you now have
        /// to use .empalce
        explicit constexpr dynamic(allocator_type const& input_alloc) noexcept(
          stl::is_nothrow_copy_constructible_v<allocator_type>)
          : alloc{input_alloc} {
            // we're not adding this as a constraint to the constructor in order to let the user use
            // incomplete types
        }

        template <typename... Args>
            requires(stl::is_constructible_v<value_type, Args...>)
        constexpr dynamic([[maybe_unused]] stl::allocator_arg_t tag,
                          allocator_type const&                 input_alloc,
                          Args&&... args)
          : dynamic{input_alloc, stl::forward<Args>(args)...} {}

        template <typename C, typename... Args>
            requires(stl::is_constructible_v<C, Args...>)
        constexpr dynamic([[maybe_unused]] stl::type_identity<C> type_ident,
                          allocator_type const&                  input_alloc,
                          Args&&... args)
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

        constexpr dynamic(dynamic const& other, allocator_type const& new_alloc) : alloc{new_alloc} {
            if (other.ptr) {
                // todo: if T is a virtual type, then this will call a copy constructor on a virtual type:
                // https://isocpp.org/wiki/faq/virtual-functions#virtual-ctors
                init<value_type>(*other.ptr);
            }
        }

        constexpr dynamic(dynamic const& other) : dynamic{other, other.alloc} {}

        template <typename DerivedT, typename NewAllocT>
            requires(stl::is_base_of_v<value_type, stl::remove_cvref_t<DerivedT>>)
        constexpr dynamic(dynamic<DerivedT, NewAllocT> const& other, allocator_type const& new_alloc)
          : alloc{new_alloc} {
            if (other.get_pointer()) {
                init<DerivedT>(*other.get_pointer());
            }
        }

        template <typename DerivedT, typename NewAllocT>
            requires(stl::is_base_of_v<value_type, stl::remove_cvref_t<DerivedT>> &&
                     stl::is_constructible_v<allocator_type, NewAllocT>)
        explicit constexpr dynamic(dynamic<DerivedT, NewAllocT> const& other)
          : dynamic{other, other.get_allocator()} {}

        template <typename DerivedT, typename NewAllocT>
            requires(stl::is_base_of_v<value_type, stl::remove_cvref_t<DerivedT>> &&
                     stl::is_constructible_v<allocator_type, NewAllocT>)
        explicit constexpr dynamic(
          dynamic<DerivedT, NewAllocT>&& other) noexcept // NOLINT(*-rvalue-reference-param-not-moved)
          : alloc{other.get_allocator()},
            ptr{stl::exchange(other.get_pointer(), nullptr)} {}

        template <typename DerivedT, typename NewAllocT>
            requires(stl::is_base_of_v<value_type, stl::remove_cvref_t<DerivedT>>)
        constexpr dynamic(dynamic<DerivedT, NewAllocT>&& other, // NOLINT(*-rvalue-reference-param-not-moved)
                          allocator_type const&          new_alloc) noexcept
          : alloc{new_alloc},
            ptr{stl::exchange(other.get_pointer(), nullptr)} {}

        constexpr dynamic(dynamic&& other) noexcept
          : alloc{other.alloc},
            ptr{stl::exchange(other.ptr, nullptr)} {}

        constexpr dynamic(dynamic&&             other, // NOLINT(*-rvalue-reference-param-not-moved)
                          allocator_type const& new_alloc) noexcept
          : alloc{new_alloc},
            ptr{stl::exchange(other.ptr, nullptr)} {}

        constexpr dynamic& operator=(value_type const& val) noexcept {
            // value_type might be an incomplete type, so we're doing it with a static_assert
            static_assert(stl::is_copy_constructible_v<value_type>, "It must be copy constructible.");
            if constexpr (stl::is_copy_assignable_v<value_type>) {
                if (ptr) {
                    // use copy assignable opereator
                    *ptr = val;
                    return *this;
                }
            }
            // create a new object and destroy the last one
            ptr = alloc_traits::allocate(alloc, 1);
            alloc_traits::construct(alloc, ptr, val);
            return *this;
        }

        constexpr dynamic& operator=(dynamic const& other) noexcept {
            if (&other != this) {
                *ptr = *other.ptr;
                if constexpr (stl::is_copy_assignable_v<allocator_type>) {
                    alloc = other.alloc;
                }
            }
            return *this;
        }

        template <typename DerivedT, typename NewAllocT>
            requires(stl::is_base_of_v<value_type, stl::remove_cvref_t<DerivedT>>)
        constexpr dynamic& operator=(
          dynamic<DerivedT, NewAllocT>&& other) noexcept { // NOLINT(*-rvalue-reference-param-not-moved)
            if constexpr (stl::is_assignable_v<allocator_type, NewAllocT>) {
                alloc = other.get_allocator();
            }
            stl::swap(ptr,
                      other.get_pointer()); // so the "other"'s destructor will destroy this
                                            // object's "ptr"
            return *this;
        }

        template <typename DerivedT, typename NewAllocT>
            requires(stl::is_base_of_v<value_type, stl::remove_cvref_t<DerivedT>>)
        constexpr dynamic& operator=(dynamic<DerivedT, NewAllocT> const& other) noexcept {
            if constexpr (stl::is_assignable_v<allocator_type, NewAllocT>) {
                alloc = other.get_allocator();
            }
            emplace<DerivedT>(*other.pointer());
            return *this;
        }

        constexpr dynamic& operator=(dynamic&& other) noexcept {
            if (&other != this) {
                if constexpr (stl::is_copy_assignable_v<allocator_type>) {
                    alloc = other.alloc;
                }
                stl::swap(ptr, other.ptr); // so the "other"'s destructor will destroy this object's "ptr"
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

        constexpr ~dynamic() {
            destroy();
        }

        constexpr void swap(dynamic& other) noexcept {
            static_assert(stl::allocator_traits<AllocT>::propagate_on_container_swap::value ||
                            stl::allocator_traits<AllocT>::is_always_equal::value,
                          "Allocator is not swappable in a nothrow manner.");
            stl::swap(ptr, other.ptr);
            stl::swap(alloc, other.alloc);
        }

        constexpr T& operator*() & noexcept {
            assert(ptr != nullptr);
            return *ptr;
        }

        constexpr T const& operator*() const& noexcept {
            assert(ptr != nullptr);
            return *ptr;
        }

        constexpr T&& operator*() && noexcept {
            assert(ptr != nullptr);
            return *ptr;
        }

        constexpr T const&& operator*() const&& noexcept {
            assert(ptr != nullptr);
            return *ptr;
        }

        constexpr pointer operator->() noexcept {
            assert(ptr != nullptr);
            return ptr;
        }

        constexpr pointer operator->() const noexcept {
            assert(ptr != nullptr);
            return ptr;
        }

        explicit constexpr operator T() const noexcept(stl::is_nothrow_copy_constructible_v<value_type>) {
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

        constexpr allocator_type const& get_allocator() const noexcept {
            return alloc;
        }

        constexpr pointer get_pointer() noexcept {
            return ptr;
        }

        constexpr const_pointer get_pointer() const noexcept {
            return ptr;
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
        template <typename C, typename... Args>
            requires(stl::is_base_of_v<T, C> && stl::is_constructible_v<C, Args...>)
        constexpr void init(Args&&... args) {
            using new_allocator_traits = typename alloc_traits::template rebind_traits<C>;
            using new_allocator_type   = typename new_allocator_traits::allocator_type;
            using new_pointer          = typename new_allocator_traits::pointer;

            new_allocator_type new_alloc{alloc};

            new_pointer new_ptr = new_allocator_traits::allocate(new_alloc, 1);
            new_allocator_traits::construct(new_alloc, new_ptr, stl::forward<Args>(args)...);
            ptr = static_cast<pointer>(new_ptr);
        }

        inline constexpr void destroy() {
            if (ptr) {
                alloc_traits::destroy(alloc, ptr);
                alloc_traits::deallocate(alloc, ptr, 1);
            }
        }
    };

    template <typename T>
    dynamic(T&&) -> dynamic<stl::remove_cvref_t<T>>;

    template <typename T, typename AllocT>
    dynamic(T&&, AllocT const&) -> dynamic<stl::remove_cvref_t<T>, AllocT>;

    namespace pmr {

        template <typename T>
        using dynamic = dynamic<T, stl::pmr::polymorphic_allocator<stl::byte>>;

    } // namespace pmr

    /**
     * Like std::remove_pointer for std::shared_ptr.
     */
    template <typename T>
    struct remove_shared_ptr {
        using type = T;
    };

    template <typename T>
    struct remove_shared_ptr<std::shared_ptr<T>> {
        using type = T;
    };

    template <typename T>
    struct remove_shared_ptr<std::shared_ptr<T const>> {
        using type = T const;
    };

    template <typename T>
    struct remove_shared_ptr<T const> {
        using type = typename remove_shared_ptr<T>::type const;
    };

    template <typename T>
    struct remove_shared_ptr<T&> {
        using type = typename remove_shared_ptr<T>::type;
    };

    template <typename T>
    using remove_shared_ptr_t = typename remove_shared_ptr<T>::type;

} // namespace webpp::istl

#endif
