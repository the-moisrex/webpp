// Created by moisrex on 10/14/20.

#ifndef WEBPP_ALLOCATORS_HPP
#define WEBPP_ALLOCATORS_HPP

#include "../common/meta.hpp"
#include "../std/type_traits.hpp"
#include "./allocator_concepts.hpp"

#include <memory>

namespace webpp {


    /**
     * Default Allocator
     * Specialize this in order to change the default allocator for the whole library.
     * @code
     *   template <typename T>
     *   struct default_allocator<T> {
     *       using type = std::pmr::polymorphic_allocator<T>;
     *   };
     * @endcode
     */
    template <typename T, int priority = 1>
    struct default_allocator {
        using type = default_allocator<T, priority - 1>;
    };

    /// Default, Default Allocator (if the user doesn't specify the default
    /// allocator, this would be used)
    template <typename T>
    struct default_allocator<T, 0> {
        using type = stl::allocator<T>;
    };

    template <typename T>
    using default_allocator_t = typename default_allocator<T>::type;

    /**
     * Allocator CPO (Customization Point Object).
     * This will let us construct an allocator like this:
     *
     * @code
     *   std::string str1{"...", alloc};
     *   std::pmr::string str2{"...", alloc};
     *   std::basic_string str2{"...", alloc.of<char>()};
     * @endcode
     *
     * Also this will let you customize the construction of any allocator that you need, like so:
     * @code
     *   template <typename T>
     *   std::pmr::polymorphic_allocator<T>
     *   tag_invoke(alloc_tag, std::type_identity<std::pmr::polymorphic_allocator<T>>) noexcept {
     *       std::println("pmr allocator constructed.");
     *       return {};
     *   }
     * @endcode
     */
    static constexpr struct [[nodiscard]] alloc_tag {
        /// Customization Point
        template <typename T>
        [[nodiscard]] constexpr decltype(auto) operator()(stl::type_identity<T> ident) const noexcept {
            // rely on ADL to find it
            return tag_invoke(*this, ident);
        }

        /// default impl: default construct
        template <Allocator T>
            requires stl::is_default_constructible_v<T>
        [[nodiscard]] friend constexpr T tag_invoke(alloc_tag, stl::type_identity<T>) noexcept {
            return {};
        }

        template <Allocator T>
        [[nodiscard]] constexpr explicit(false) operator T() const noexcept {
            return operator()(stl::type_identity<T>{});
        }

        // template <typename T>
        // [[nodiscard]] constexpr explicit(false) operator default_allocator_t<T>() const noexcept {
        //     return operator()(stl::type_identity<default_allocator_t<T>>{});
        // }

        template <typename T = stl::byte>
        [[nodiscard]] constexpr decltype(auto) of() const noexcept {
            static_assert(!Allocator<T>, "Don't pass an allocator, pass the value type you need.");
            return operator()(stl::type_identity<default_allocator_t<T>>{});
        }
    } alloc;

    template <typename AllocType, typename NewValueType>
    using rebind_allocator = typename stl::allocator_traits<AllocType>::template rebind_alloc<NewValueType>;

    namespace details {
        template <typename T>
        struct temp_alloc_holder {
            stl::remove_cvref_t<T> alloc;

            [[nodiscard]] constexpr auto operator|([[maybe_unused]] auto&& new_alloc) noexcept {
                // just ignore the new alloc, our order is from right to left
                return *this;
            }
        };

        template <typename AllocType = void>
        struct alloc_finder_type {
            using alloc_type = stl::remove_cvref_t<AllocType>;

            constexpr auto operator()(auto&& arg) const noexcept {
                using U = stl::remove_cvref_t<decltype(arg)>;
                if constexpr (!stl::is_void_v<alloc_type>) {
                    if constexpr (/* stl::uses_allocator_v<U, alloc_type> && */
                                  requires(U n_alloc) { n_alloc.get_allocator(); })
                    {
                        // I have to explicitly use decltype here because of a clang bug
                        return temp_alloc_holder<decltype(arg.get_allocator())>{arg.get_allocator()};
                    } else {
                        return false;
                    }
                } else {
                    if constexpr (requires(U n_alloc) {
                                      // typename U::allocator_type;
                                      n_alloc.get_allocator();
                                  })
                    {
                        // I have to use decltype because of a clang bug
                        return temp_alloc_holder<decltype(arg.get_allocator())>{arg.get_allocator()};
                    } else {
                        return false;
                    }
                }
            }

            constexpr auto operator|(auto&& finder_res) const noexcept { // NOLINT(misc-unconventional-assign-operator)
                using U = stl::remove_cvref_t<decltype(finder_res)>;
                if constexpr (stl::is_integral_v<U>) {
                    return *this;
                } else {
                    return temp_alloc_holder{finder_res};
                }
            }
        };

    } // namespace details

    /**
     * Extract an allocator from all the inputs. This is not safe if one of the inputs uses
     *   - allocator_type and
     *   - get_allocator()
     * in a different context as the standard library does.
     */
    template <typename... T>
    [[nodiscard]] static constexpr auto extract_allocator(T&&... args) noexcept {
        webpp_static_constexpr details::alloc_finder_type<> finder;
        auto const                                          res = (finder | ... | finder(stl::forward<T>(args)));
        static_assert(requires { res.alloc; }, "We didn't find any allocator in the inputs.");
        return res.alloc;
    }

    template <typename Default = stl::allocator<void>, typename... T>
    [[nodiscard]] inline auto extract_allocator_or_default(T&&... args) noexcept {
        webpp_static_constexpr details::alloc_finder_type<> finder;
        auto const                                          res = (finder | ... | finder(stl::forward<T>(args)));
        if constexpr (requires { res.alloc; }) {
            return res.alloc;
        } else {
            return Default{};
        }
    }

    /**
     * This version of allocator extractor will help you extract an allocator of an specific type.
     */
    template <typename AllocType, typename... T>
    [[nodiscard]] inline auto extract_allocator_of(T&&... args) noexcept {
        details::alloc_finder_type<AllocType> finder;
        auto const                            res = (finder | ... | finder(stl::forward<T>(args)));
        static_assert(stl::is_same_v<stl::remove_cvref_t<decltype(res)>, details::temp_alloc_holder<AllocType>>,
                      "We didn't find any allocator in the inputs.");
        return res.alloc;
    }

    template <typename AllocType, typename... T>
    [[nodiscard]] inline auto extract_allocator_of_or_default(T&&... args) noexcept {
        details::alloc_finder_type<AllocType> finder;
        auto const                            res = (finder | ... | finder(stl::forward<T>(args)));
        if constexpr (stl::is_same_v<stl::remove_cvref_t<decltype(res)>, details::temp_alloc_holder<AllocType>>) {
            return res.alloc;
        } else {
            // todo: we might be able to find and convert an allocator and not just re-create it
            return AllocType{};
        }
    }

    template <typename AllocType>
    struct allocator_holder {
        using allocator_type       = stl::remove_cvref_t<AllocType>;
        using allocator_value_type = typename allocator_type::value_type;

        template <typename T>
        using allocator_type_as = rebind_allocator<allocator_type, T>;

        template <typename... T>
        explicit constexpr allocator_holder(T&&... alloc_holders) noexcept
          : alloc{extract_allocator_of<AllocType, T...>(stl::forward<T>(alloc_holders)...)} {}

        explicit constexpr allocator_holder(allocator_type const& new_alloc = allocator_type{}) noexcept
          : alloc(new_alloc) {}

        template <typename T = allocator_value_type>
        [[nodiscard]] auto get_allocator_as() const noexcept {
            if constexpr (stl::is_same_v<T, allocator_value_type>) {
                return alloc;
            } else {
                // using copy ctor, so this should work for most allocator types
                return rebind_allocator<allocator_type, T>(alloc);
            }
        }

        [[nodiscard]] auto const& get_allocator() const noexcept {
            return alloc;
        }

      private:
        allocator_type alloc;
    };

    // namespace details {
    //     template <template <typename> typename AllocType>
    //     struct allocator_replacer {
    //         template <typename T>
    //         struct replacer {
    //             static constexpr bool value = false;
    //             using type                  = T;
    //         };
    //
    //         template <Allocator T>
    //         struct replacer<T> {
    //             static constexpr bool value = true;
    //             using value_type            = typename T::value_type;
    //             using type                  = AllocType<value_type>;
    //         };
    //     };
    // } // namespace details
    //
    // template <typename T, template <typename> typename AllocType>
    // using replace_allocators =
    //   istl::recursive_parameter_replacer<T, details::allocator_replacer<AllocType>::template replacer>;

    /**
     * Get allocator of T, if it has an allocator, otherwise, revert back to the DefaultAllocatorType
     */
    template <typename T, typename DefaultAllocatorType = stl::allocator<typename T::value_type>>
    struct allocator_type_from {
        using type = DefaultAllocatorType;
    };

    template <typename T, typename DefAllocT>
        requires requires { typename T::allocator_type; }
    struct allocator_type_from<T, DefAllocT> {
        using type = typename T::allocator_type;
    };

    // todo: use `allocator_from_tag` CPO as the default
    template <typename T, typename DefaultAllocatorType = stl::allocator<typename T::value_type>>
    using allocator_type_from_t = typename allocator_type_from<T, DefaultAllocatorType>::type;

} // namespace webpp

#endif // WEBPP_ALLOCATORS_HPP
