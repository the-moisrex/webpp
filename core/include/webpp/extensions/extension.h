// Created by moisrex on 5/15/20.

#ifndef WEBPP_EXTENSION_H
#define WEBPP_EXTENSION_H

#include "../std/std.h"
#include "../std/tuple.h"
#include "../traits/traits_concepts.h"

#include <cstdint>
#include <type_traits>

namespace webpp {

    struct fake_extension {
        template <typename T>
        using type = void;
    };

    // todo: check this, it's old
    template <typename T>
    concept Extension =
      std::copy_constructible<T> && !std::is_final_v<T> &&
      std::is_default_constructible_v<T> && std::is_move_constructible_v<T> &&
      std::is_move_assignable_v<T> && std::is_copy_assignable_v<T> &&
      std::is_class_v<T> && !stl::is_tuple<T>::value && !std::is_integral_v<T>;

    template <typename T>
    concept MotherExtension = Extension<T>&& requires {
        typename T::template type<fake_traits>;
    };

    template <typename T>
    concept ChildExtension = Extension<T>&& requires {
        T::template type<fake_traits, fake_extension>;
    };

    template <Extension... E>
    struct extension_pack {
      private:
        template <typename... F>
        struct prepend {
            using type = extension_pack<F...>;
        };

        template <typename F, typename... L>
        struct prepend<F, extension_pack<L...>> {
            using type = extension_pack<F, L...>;
        };

        template <template <typename> typename IF, typename First = void,
                  typename... EI>
        struct filter {
            using type = std::conditional_t<
              IF<First>::value,
              typename prepend<First, typename filter<IF, EI...>::type>::type,
              typename filter<IF, EI...>::type>;
        };

        template <template <typename> typename IF, typename... EI>
        struct filter<IF, void, EI...> {
            using type = extension_pack<EI...>;
        };

        template <template <typename> typename IF, typename... EI>
        struct filter_epack {};

        template <template <typename> typename IF, typename... EI>
        struct filter_epack<IF, extension_pack<EI...>> {
            using type = typename filter<IF, EI...>::type;
        };

        //        template <typename First = void, typename... U>
        //        struct unique_types {
        //            using type = std::conditional_t<
        //              ((!std::is_same_v<First, U>)&&...),
        //              typename prepend<First, typename extension_pack<
        //                                        U...>::unique_types<E...>::type>::type,
        //              typename
        //              extension_pack<U...>::unique_types<E...>::type>;
        //        };
        //
        //        template <typename... U>
        //        struct unique_types<void, U...> {
        //            using type = extension_pack<U...>;
        //        };


        template <typename T>
        struct mother_type {
            static constexpr bool value = MotherExtension<T>;
        };

        template <typename T>
        struct child_type {
            static constexpr bool value = ChildExtension<T>;
        };
        //
        //        template <typename... E1>
        //        struct flatten_extensions {
        //            using type = extension_pack<>;
        //        };
        //
        //        template <typename... E1>
        //        struct flatten_extensions<extension_pack<E1...>> {
        //            using type = extension_pack<E1...>;
        //        };
        //
        //        template <typename... E1, typename... E2>
        //        struct flatten_extensions<extension_pack<E1...>,
        //                                  extension_pack<E2...>> {
        //            using type = extension_pack<E1..., E2...>;
        //        };
        //
        //        template <typename... E1, typename... E2, typename... ER>
        //        struct flatten_extensions<extension_pack<EI...>,
        //        extension_pack<E2...>,
        //                                  ER...> {
        //            using type = typename flatten_extensions<
        //              typename flatten_extensions<extension_pack<E1...>,
        //                                          extension_pack<E2...>>::type,
        //              ER...>::type;
        //        };


        template <template <typename> typename Extractor, typename... EPack>
        struct epack_miner {};

        template <template <typename> typename Extractor, typename... EPack>
        struct epack_miner<Extractor, extension_pack<EPack...>> {
            using type = extension_pack<Extractor<EPack>...>;
        };

        template <typename... Ex>
        struct inheritable_extension_pack {
            // this should not happen
        };
        template <typename... Ex>
        struct inheritable_extension_pack<extension_pack<Ex...>>
          : public virtual Ex... {
            template <typename... X>
            inheritable_extension_pack(X&&... x)
              : Ex{std::forward<X>(x)...}... {
            }
        };

        using mother_extensions = typename filter<mother_type, E...>::type;
        using child_extensions  = typename filter<child_type, E...>::type;

        template <typename ExtensieDescriptor, typename EPack>
        using merged_extensions = typename epack_miner<
          ExtensieDescriptor::template related_extension_pack_type,
          typename filter_epack<
            ExtensieDescriptor::template has_related_extension_pack,
            EPack>::type>::type;


        template <typename ExtendThis, typename... Ex>
        struct extend_to_all {
            // this should not happen
        };

        template <typename ExtendThis, typename... Ex>
        struct extend_to_all<ExtendThis, extension_pack<Ex...>>
          : public virtual Ex... {
            template <typename... Args>
            extend_to_all(Args&&... args) : Ex{std::forward<Args>(args)...}... {
            }
        };


      public:
        /**
         * Get the unique types
         */
        //        using unique = typename unique_types<E...>::type;

        /**
         * Apply extensions into one type
         * todo: first filter based on extensie, then filter based on mother or child
         */
        template <typename TraitsType, typename ExtensieDescriptor>
        using extensie_type = ExtensieDescriptor::template final_extensie_type<
          TraitsType,
          extend_to_all<
            typename ExtensieDescriptor::template mid_level_extensie_type<
              TraitsType, inheritable_extension_pack<merged_extensions<
                            ExtensieDescriptor, mother_extensions>>>::type,
            merged_extensions<ExtensieDescriptor, child_extensions>>>;



        /**
         * Check if all of the extensions are the correct type
         * @tparam IF
         */
        template <template <typename> typename IF>
        struct is_all {
            static constexpr bool value = (IF<E>::value && ...);
        };
    };
    struct fake_extensie_descriptor {
        template <typename ExtensionType>
        struct has_related_extension_pack {
            static constexpr bool value = true;
        };

        template <typename ExtensionType>
        using related_extension_pack_type = typename ExtensionType::something;

        template <typename TraitsType, typename EList>
        using mid_level_extensie_type = void;

        // empty final extensie
        template <typename TraitsType, typename EList>
        struct final_extensie_type final : public EList {};
    };

    template <typename E>
    concept ExtensionList = requires {
        typename E::template extensie_type<fake_traits,
                                           fake_extensie_descriptor>;
        typename E::template is_all<
          fake_extensie_descriptor::template has_related_extension_pack>;
    };

    template <typename E, template <typename> typename IF>
    concept ExtensionListOf = ExtensionList<E>&& E::template is_all<IF>::value;

    //        template <typename... T>
    //        struct is_extension_list {
    //            static constexpr bool value = false;
    //        };
    //
    //        template <Extension T>
    //        struct is_extension_list<T> {
    //            static constexpr bool value = true;
    //        };
    //
    //        template <Extension... T>
    //        struct is_extension_list<std::tuple<T...>> {
    //            static constexpr bool value = true;
    //        };
    //
    //        template <typename T, template <typename...>
    //                              typename IsExtensionList =
    //                              is_extension_list>
    //        concept ExtensionList = IsExtensionList<T>::value ||
    //                                (::stl::Tuple<T> &&
    //                                IsExtensionList<T>::value);
    //

    //    template <typename T>
    //    concept ExtensionWithDependencies = Extension<T>&& requires {
    //        typename T::required_extensions;
    //    }
    //    &&Extension<typename T::required_extensions>;
    //

    template <typename... T>
    struct typelist {
      private:
        template <typename... F>
        struct prepend {
            using type = typelist<F...>;
        };

        template <typename F, typename... L>
        struct prepend<F, typelist<L...>> {
            using type = typelist<F, L...>;
        };

        template <typename First = void, typename... U>
        struct unique_types {
            using type = std::conditional_t<
              ((!std::is_same_v<First, U>)&&...),
              typename prepend<First, typename typelist<U...>::unique>::type,
              typename typelist<U...>::unique>;
        };

        template <typename... U>
        struct unique_types<void, U...> {
            using type = typelist<U...>;
        };

      public:
        using unique = typename unique_types<T...>::type;
    };


    //    template <Extension... Extensions>
    //    struct extension_pack : public Extensions... {};
    //
    //
    //    /**
    //     * Unpack std::tuple into extension pack
    //     */
    //    template <Extension... Extensions>
    //    struct extension_pack<std::tuple<Extensions...>>
    //      : public extension_pack<Extensions...> {};
    //
    //    /**
    //     * Unpack typelist into extension pack
    //     */
    //    template <Extension... Extensions>
    //    struct extension_pack<typelist<Extensions...>>
    //      : public extension_pack<Extensions...> {};
    //
    //    /**
    //     * Unpack the extension pack
    //     */
    //    template <Extension... Extensions>
    //    struct extension_pack<extension_pack<Extensions...>>
    //      : public extension_pack<Extensions...> {};
    //
    //    template <typename... E>
    //    struct flattened_extension_pack;
    //
    //    template <Extension... EP1, Extension... EP2>
    //    struct flattened_extension_pack<extension_pack<EP1...>,
    //    typelist<EP2...>> {
    //        using type = extension_pack<EP1..., EP2...>;
    //    };
    //
    //    template <Extension... EP1, Extension... EP2>
    //    struct flattened_extension_pack<extension_pack<EP1...>,
    //                                    std::tuple<EP2...>> {
    //        using type = extension_pack<EP1..., EP2...>;
    //    };
    //
    //    template <Extension... EP1, Extension... EP2>
    //    struct flattened_extension_pack<std::tuple<EP1...>,
    //    typelist<EP2...>> {
    //        using type = extension_pack<EP1..., EP2...>;
    //    };
    //
    //    template <Extension... EP1, Extension... EP2>
    //    struct flattened_extension_pack<std::tuple<EP1...>,
    //                                    extension_pack<EP2...>> {
    //        using type = extension_pack<EP1..., EP2...>;
    //    };
    //
    //    template <Extension... EP1, Extension... EP2>
    //    struct flattened_extension_pack<typelist<EP1...>,
    //    extension_pack<EP2...>> {
    //        using type = extension_pack<EP1..., EP2...>;
    //    };
    //
    //    template <Extension... EP1, Extension... EP2>
    //    struct flattened_extension_pack<typelist<EP1...>,
    //    std::tuple<EP2...>> {
    //        using type = extension_pack<EP1..., EP2...>;
    //    };


    ///////////////////// Extending the is_extension_list ////////////////////

    //    template <Extension... T>
    //    struct is_extension_list<extension_pack<T...>> {
    //        static constexpr bool value = true;
    //    };
    //
    //    template <Extension... T>
    //    struct is_extension_list<typelist<T...>> {
    //        static constexpr bool value = true;
    //    };
    //

    //////////////////////////////////////////////////////////////////////////

    using empty_extension_pack = extension_pack<>;

    //    namespace extensions {
    //        static constexpr auto mother = 0x1;
    //        static constexpr auto child  = 0x2;
    //
    //        template <typename T>
    //        concept MotherExtension = Extension<T> &&
    //                                  ((T::features & mother) == mother);
    //
    //        template <typename T>
    //        concept ChildExtension = Extension<T> &&
    //                                 ((T::features & child) == child);
    //    } // namespace extensions



    //////////////////////////////////////////////////////////////////////////

    //    struct empty_extension {
    //        static constexpr auto features = extensions::mother;
    //    };

    //    template <typename T>
    //    concept HasExtensionSupport = requires {
    //        typename T::template rebind<empty_extension>;
    //        std::copy_constructible<T>; // includes move constructor as well
    //    };

    //    template <typename T>
    //    using rebinder = T;
    //
    //    template <typename T, Extension FirstE>
    //    using rebinder =
    //    std::conditional_t<extensions::MotherExtension<FirstE>,
    //                                          typename T::template
    //                                          rebind<FirstE>, typename
    //                                          FirstE::template rebind<T>>;
    //
    //    template <typename T, Extension FirstE, Extension... Rest>
    //    using rebinder = rebinder<rebinder<T, FirstE>, Rest...>;
    //

    //////////////////////////////////////////////////////////////////////////

    //    template <template <Extension ...> typename T, Extension ...C>
    //    struct extension_holder : public C... {
    //
    //        template<Extension FE>
    //        struct rebind {
    //            using type =
    //            std::conditional_t<extensions::MotherExtension<FE>,
    //                    T<C..., FE>,
    //                    typename FE::template rebind<T>>
    //        };
    //
    //        template <Extension FE, Extension ...E>
    //        struct rebind {
    //            using type = rebind<T<C..., FE>, E...>;
    //        };
    //
    //        template <Extension... ExtensionType>
    //        constexpr auto copy() const noexcept {
    //            using new_type = rebind<ExtensionType...>;
    //            return new_type{*this};
    //        }
    //
    //        template <Extension... ExtensionType>
    //        constexpr auto move() const noexcept {
    //            using new_type = rebind<ExtensionType...>;
    //            return new_type{std::move(*this)};
    //        }
    //
    //    };
    //


    template <typename T, template <typename> typename FE,
              template <typename> typename... E>
    struct apply_extensions {
        using type = apply_extensions<FE<T>, E...>;
    };

    template <typename T, template <typename> typename FE>
    struct apply_extensions<T, FE> {
        using type = FE<T>;
    };


} // namespace webpp

#endif // WEBPP_EXTENSION_H
