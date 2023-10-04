#ifndef WEBPP_STD_MEMORY_HPP
#define WEBPP_STD_MEMORY_HPP

#include "./memory_resource.hpp"
#include "./type_traits.hpp"
#include "./utility.hpp"

#include <memory>


namespace webpp::istl {

    // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays)
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
    constexpr inline T* first_scalar(T* p) noexcept {
        return p;
    }

    template <class T, std::size_t N>
    constexpr inline typename detail::make_scalar<T>::type* first_scalar(T (*p)[N]) noexcept {
        return first_scalar(&(*p)[0]);
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
            typedef T type;
        };

        template <class T, stl::size_t N>
        struct sp_alloc_result<T[N]> {
            using type = T[];
        };

        template <class T>
        struct sp_alloc_value {
            using type = typename stl::remove_cv_t<stl::remove_extent_t<T>>;
        };

        template <class T, class P>
        struct sp_alloc_ptr {
            using element_type = T;

            constexpr sp_alloc_ptr() noexcept = default;

            constexpr sp_alloc_ptr(stl::size_t, P inp_p) noexcept : p(inp_p) {}

            constexpr sp_alloc_ptr(stl::nullptr_t) noexcept {};

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
        class sp_alloc_ptr<T[], P> {
          public:
            typedef T element_type;

            constexpr sp_alloc_ptr() noexcept = default;

            constexpr sp_alloc_ptr(stl::size_t inp_n, P inp_p) noexcept : p(inp_p), n(inp_n) {}

            constexpr sp_alloc_ptr(stl::nullptr_t) noexcept {};

            constexpr T& operator[](stl::size_t i) const {
                return p[i];
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
        class sp_alloc_ptr<T[N], P> {
          public:
            typedef T element_type;

            constexpr sp_alloc_ptr() noexcept = default;

            constexpr sp_alloc_ptr(stl::size_t, P inp_p) noexcept : p(inp_p) {}

            constexpr sp_alloc_ptr(stl::nullptr_t) noexcept {};

            constexpr T& operator[](stl::size_t i) const {
                return p[i];
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
        constexpr bool operator==(const sp_alloc_ptr<T, P>& lhs, const sp_alloc_ptr<T, P>& rhs) {
            return lhs.ptr() == rhs.ptr();
        }

        template <class T, class P>
        constexpr bool operator!=(const sp_alloc_ptr<T, P>& lhs, const sp_alloc_ptr<T, P>& rhs) {
            return !(lhs == rhs);
        }

        template <class T, class P>
        constexpr bool operator==(const sp_alloc_ptr<T, P>& lhs, stl::nullptr_t) noexcept {
            return !lhs.ptr();
        }

        template <class T, class P>
        constexpr bool operator==(stl::nullptr_t, const sp_alloc_ptr<T, P>& rhs) noexcept {
            return !rhs.ptr();
        }

        template <class T, class P>
        constexpr bool operator!=(const sp_alloc_ptr<T, P>& lhs, stl::nullptr_t) noexcept {
            return static_cast<bool>(lhs.ptr());
        }

        template <class T, class P>
        constexpr bool operator!=(stl::nullptr_t, const sp_alloc_ptr<T, P>& rhs) noexcept {
            return static_cast<bool>(rhs.ptr());
        }

        template <class A>
        constexpr void
        sp_alloc_clear(A& a, typename stl::allocator_traits<A>::pointer p, stl::size_t, stl::false_type) {
            stl::destroy(a, stl::to_address(p));
        }

    } // namespace details


    // NOLINTEND(cppcoreguidelines-avoid-c-arrays)

    template <class T, class A>
    class alloc_deleter {
        using allocator =
          typename stl::allocator_traits<A>::template rebind<typename details::sp_alloc_value<T>::type>;

        [[no_unique_address]] allocator alloc;

      public:
        using pointer = details::sp_alloc_ptr<T, typename stl::allocator_traits<allocator>::pointer>;

        constexpr explicit alloc_deleter(const allocator& a) noexcept : alloc{a} {}

        constexpr void operator()(pointer p) {
            details::sp_alloc_clear(alloc, p.ptr(), p.size(), stl::is_array<T>());
            alloc.deallocate(p.ptr(), p.size());
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

            constexpr sp_alloc_make(const A& a, stl::size_t inp_n)
              : alloc(a),
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
                pointer p = ptr;
                ptr       = pointer();
                return type(typename deleter::pointer(n, p), deleter(alloc));
            }

          private:
            allocator   alloc;
            stl::size_t n;
            pointer     ptr;
        };

    } // namespace details

    template <class T, class A>
        requires(!stl::is_array_v<T>)
    constexpr stl::unique_ptr<T, alloc_deleter<T, A>> allocate_unique(const A& alloc) {
        details::sp_alloc_make<T, A> c(alloc, 1);
        stl::allocator_traits<A>::construct(c.state(), c.get());
        return c.release();
    }

    template <class T, class A, class... Args>
        requires(!stl::is_array_v<T>)
    constexpr stl::unique_ptr<T, alloc_deleter<T, A>> allocate_unique(const A& alloc, Args&&... args) {
        details::sp_alloc_make<T, A> c(alloc, 1);
        stl::allocator_traits<A>::construct(c.state(), c.get(), stl::forward<Args>(args)...);
        return c.release();
    }

    template <class T, class A>
        requires(!stl::is_array_v<T>)
    constexpr stl::unique_ptr<T, alloc_deleter<T, A>>
    allocate_unique(const A& alloc, typename stl::type_identity<T>::type&& value) {
        details::sp_alloc_make<T, A> c(alloc, 1);
        stl::allocator_traits<A>::construct(c.state(), c.get(), stl::move(value));
        return c.release();
    }

    template <class T, class A>
        requires(stl::is_unbounded_array_v<T>)
    constexpr stl::unique_ptr<T, alloc_deleter<T, A>> allocate_unique(const A& alloc, stl::size_t size) {
        details::sp_alloc_make<T, A> c(alloc, size);
        stl::allocator_traits<A>::construct_n(c.state(),
                                              first_scalar(c.get()),
                                              size * details::sp_alloc_size<T>::value);
        return c.release();
    }

    template <class T, class A>
        requires(stl::is_unbounded_array_v<T>)
    constexpr stl::unique_ptr<typename details::sp_alloc_result<T>::type, alloc_deleter<T, A>>
    allocate_unique(const A& alloc) {
        details::sp_alloc_make<T, A> c(alloc, stl::extent<T>::value);
        stl::allocator_traits<A>::construct_n(c.state(),
                                              first_scalar(c.get()),
                                              details::sp_alloc_size<T>::value);
        return c.release();
    }





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
            derived_alloc_traits::construct(istl::unmove(derived_alloc_type{alloc}),
                                            static_cast<derived_pointer>(ptr),
                                            stl::forward<InheritedType>(derived_obj));
        }

        template <typename InheritedType>
            requires(!stl::is_constructible_v<value_type, InheritedType> &&
                     stl::is_base_of_v<value_type, stl::remove_cvref_t<InheritedType>>)
        constexpr dynamic(stl::allocator_arg_t,
                          allocator_type const& input_alloc,
                          InheritedType&&       derived_obj)
          : dynamic{input_alloc, stl::forward<InheritedType>(derived_obj)} {}

        template <typename InheritedType>
            requires(!stl::is_constructible_v<value_type, InheritedType> &&
                     stl::is_base_of_v<value_type, stl::remove_cvref_t<InheritedType>>)
        constexpr dynamic(InheritedType&& derived_obj, allocator_type const& input_alloc)
          : dynamic{input_alloc, stl::forward<InheritedType>(derived_obj)} {}


        template <typename... Args>
            requires(stl::is_constructible_v<value_type, Args...>)
        constexpr dynamic(allocator_type const& input_alloc, Args&&... args)
          : alloc{input_alloc},
            ptr{alloc_traits::allocate(alloc, 1)} {
            static_assert(stl::is_constructible_v<T, Args...>,
                          "The specified type is cannot be initialized with the specified arguments.");
            alloc_traits::construct(alloc, ptr, stl::forward<Args>(args)...);
        }

        template <typename... Args>
            requires(stl::is_constructible_v<value_type, Args...>)
        constexpr dynamic(stl::allocator_arg_t, allocator_type const& input_alloc, Args&&... args)
          : dynamic{input_alloc, stl::forward<Args>(args)...} {}

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
        constexpr dynamic(dynamic<DerivedT, NewAllocT> const& other)
          : dynamic{other, other.get_allocator()} {}


        template <typename DerivedT, typename NewAllocT>
            requires(stl::is_base_of_v<value_type, stl::remove_cvref_t<DerivedT>> &&
                     stl::is_constructible_v<allocator_type, NewAllocT>)
        constexpr dynamic(dynamic<DerivedT, NewAllocT>&& other) noexcept
          : alloc{other.get_allocator()},
            ptr{stl::exchange(other.get_pointer(), nullptr)} {}

        template <typename DerivedT, typename NewAllocT>
            requires(stl::is_base_of_v<value_type, stl::remove_cvref_t<DerivedT>>)
        constexpr dynamic(dynamic<DerivedT, NewAllocT>&& other, allocator_type const& new_alloc) noexcept
          : alloc{new_alloc},
            ptr{stl::exchange(other.get_pointer(), nullptr)} {}


        constexpr dynamic(dynamic&& other) noexcept
          : alloc{other.alloc},
            ptr{stl::exchange(other.ptr, nullptr)} {}

        constexpr dynamic(dynamic&& other, allocator_type const& new_alloc) noexcept
          : alloc{new_alloc},
            ptr{stl::exchange(other.ptr, nullptr)} {}


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
        constexpr dynamic& operator=(dynamic<DerivedT, NewAllocT>&& other) noexcept {
            if constexpr (stl::is_assignable_v<allocator_type, NewAllocT>) {
                alloc = other.get_allocator();
            }
            stl::swap(ptr,
                      other.get_pointer()); // so the "other"'s destructor will destroy this object's "ptr"
        }

        template <typename DerivedT, typename NewAllocT>
            requires(stl::is_base_of_v<value_type, stl::remove_cvref_t<DerivedT>>)
        constexpr dynamic& operator=(dynamic<DerivedT, NewAllocT> const& other) noexcept {
            if constexpr (stl::is_assignable_v<allocator_type, NewAllocT>) {
                alloc = other.get_allocator();
            }
            emplace<DerivedT>(*other.pointer());
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
            static_assert(stl::allocator_traits<Allocator>::propagate_on_container_swap::value ||
                            stl::allocator_traits<Allocator>::is_always_equal::value,
                          "Allocator is not swappable in a nothrow manner.");
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
    struct remove_shared_ptr<std::shared_ptr<const T>> {
        using type = const T;
    };

    template <typename T>
    struct remove_shared_ptr<const T> {
        using type = const typename remove_shared_ptr<T>::type;
    };

    template <typename T>
    struct remove_shared_ptr<T&> {
        using type = typename remove_shared_ptr<T>::type;
    };

    template <typename T>
    using remove_shared_ptr_t = typename remove_shared_ptr<T>::type;

} // namespace webpp::istl

#endif
