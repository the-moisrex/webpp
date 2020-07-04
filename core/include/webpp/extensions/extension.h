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

    struct fake_extensie {};

    // todo: check this, it's old
    template <typename T>
    concept Extension =
      stl::copy_constructible<T> && !stl::is_final_v<T> && stl::is_default_constructible_v<T> &&
      stl::is_move_constructible_v<T> && stl::is_move_assignable_v<T> && stl::is_copy_assignable_v<T> &&
      stl::is_class_v<T> && !stl::is_integral_v<T>;

    template <typename T>
    concept MotherExtension = Extension<T>&& requires {
        typename T::template type<fake_traits_type>;
    };

    template <typename T>
    concept ChildExtension = Extension<T>&& requires {
        T::template type<fake_traits_type, fake_extensie>;
    };

    template <Extension... E>
    struct extension_pack {

        template <typename... NE>
        using appended = extension_pack<E..., NE...>;

        template <template <typename...> typename PackType, typename... F>
        struct prepend {
            using type = PackType<F...>;
        };

        template <template <typename...> typename PackType, typename F, typename... L>
        struct prepend<PackType, F, extension_pack<L...>> {
            using type = PackType<F, L...>;
        };

        template <template <typename...> typename PackType, template <typename> typename IF,
                  typename First = void, typename... EI>
        struct filter {
            using type = stl::conditional_t<
              IF<First>::value,
              typename prepend<PackType, First, typename filter<PackType, IF, EI...>::type>::type,
              typename filter<PackType, IF, EI...>::type>;
        };

        template <template <typename...> typename PackType, template <typename> typename IF, typename... EI>
        struct filter<PackType, IF, void, EI...> {
            using type = PackType<EI...>;
        };

        template <template <typename...> typename PackType, template <typename> typename IF, typename... EI>
        struct filter_epack {
            using type = typename filter<PackType, IF, EI...>::type;
        };

        template <template <typename...> typename PackType, template <typename> typename IF, typename... EI>
        struct filter_epack<PackType, IF, extension_pack<EI...>> {
            using type = typename filter<PackType, IF, EI...>::type;
        };

        template <typename First = void, typename... U>
        struct unique_types {
            using type = stl::conditional_t<
              ((!std::is_same_v<First, U>)&&...),
              typename prepend<extension_pack, First, typename unique_types<U...>::type>::type,
              typename unique_types<U...>::type>;
        };

        template <typename... U>
        struct unique_types<void, U...> {
            using type = extension_pack<U...>;
        };

        template <typename... U>
        struct unique_types<extension_pack<U...>> {
            using type = typename unique_types<U...>::type;
        };

        /**
         * Get the unique types
         */
        //        using unique = typename unique_types<E...>::type;

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


        template <template <typename...> typename PackType, template <typename> typename Extractor,
                  typename... EPack>
        struct epack_miner {};

        template <template <typename...> typename PackType, template <typename> typename Extractor,
                  typename... EPack>
        struct epack_miner<PackType, Extractor, extension_pack<EPack...>> {
            using type = PackType<Extractor<EPack>...>;
        };

        //        template <typename... Ex>
        //        struct inheritable_extension_pack {
        //            // this should not happen
        //        };
        //        template <typename... Ex>
        //        struct inheritable_extension_pack<extension_pack<Ex...>> : public virtual Ex... {
        //            template <typename... X>
        //            inheritable_extension_pack(X&&... x) : Ex{stl::forward<X>(x)...}... {
        //            }
        //        };

        using mother_extensions = typename filter<extension_pack, mother_type, E...>::type;
        using child_extensions  = typename filter<extension_pack, child_type, E...>::type;

        using this_epack = extension_pack<E...>;

        template <typename... EPacks>
        struct flatten_epacks {
            using type = extension_pack<EPacks...>;
        };

        template <Extension... Ex>
        struct flatten_epacks<extension_pack<Ex...>> {
            using type = extension_pack<Ex...>;
        };

        template <Extension... Ex1, Extension... Ex2>
        struct flatten_epacks<extension_pack<Ex1...>, extension_pack<Ex2...>> {
            using type = extension_pack<Ex1..., Ex2...>;
        };

        template <Extension... Ex, typename... EPacks>
        struct flatten_epacks<extension_pack<Ex...>, EPacks...> {
            using type = typename flatten_epacks<Ex..., typename flatten_epacks<EPacks...>::type>::type;
        };

        template <Extension... Ex1, typename... EPacks>
        struct flatten_epacks<extension_pack<extension_pack<Ex1...>, EPacks...>> {
            using type =
              typename flatten_epacks<extension_pack<Ex1...>, typename flatten_epacks<EPacks...>::type>::type;
        };


        template <typename ExtensieDescriptor>
        using merge_extensions = typename unique_types<typename flatten_epacks<typename epack_miner<
          extension_pack, ExtensieDescriptor::template related_extension_pack_type,
          typename filter_epack<extension_pack, ExtensieDescriptor::template has_related_extension_pack,
                                this_epack>::type>::type>::type>::type;


        struct inherited : public virtual E... {
            template <typename... Args>
            constexpr inherited(Args&&... args) noexcept : E{stl::forward<Args>(args)...}... {
            }
        };

        template <Traits TraitsType, typename Mother>
        struct joined_extensions : public virtual E::type<TraitsType, Mother>... {};



        /**
         * Apply extensions into one type
         * todo: first filter based on extensie, then filter based on mother or child
         */
        template <Traits TraitsType, typename ExtensieDescriptor, typename... ExtraArgs>
        using extensie_type = typename ExtensieDescriptor::template final_extensie_type<
          this_epack, TraitsType,

          // child extensions + the mid-level extensie + mother extensions
          typename merge_extensions<ExtensieDescriptor>::child_extensions::template joined_extensions<
            TraitsType,
            typename ExtensieDescriptor::template mid_level_extensie_type<
              this_epack, TraitsType,
              typename merge_extensions<ExtensieDescriptor>::mother_extensions::inherited, ExtraArgs...>>,

          ExtraArgs...>;


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
        struct related_extension_pack_type {};

        template <typename EPackType, typename TraitsType, typename EList>
        struct mid_level_extensie_type : public EList {};

        // empty final extensie
        template <typename EPackType, typename TraitsType, typename EList>
        struct final_extensie_type final : public EList {};
    };

    template <typename E>
    concept ExtensionList = requires {
        typename E::template extensie_type<fake_traits_type, fake_extensie_descriptor>;
        typename E::template is_all<fake_extensie_descriptor::template has_related_extension_pack>;
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

    //    template <typename... T>
    //    struct typelist {
    //      private:
    //        template <typename... F>
    //        struct prepend {
    //            using type = typelist<F...>;
    //        };
    //
    //        template <typename F, typename... L>
    //        struct prepend<F, typelist<L...>> {
    //            using type = typelist<F, L...>;
    //        };
    //
    //        template <typename First = void, typename... U>
    //        struct unique_types {
    //            using type = std::conditional_t<
    //              ((!std::is_same_v<First, U>)&&...),
    //              typename prepend<First, typename
    //              typelist<U...>::unique>::type, typename
    //              typelist<U...>::unique>;
    //        };
    //
    //        template <typename... U>
    //        struct unique_types<void, U...> {
    //            using type = typelist<U...>;
    //        };
    //
    //      public:
    //        using unique = typename unique_types<T...>::type;
    //    };


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


    //    template <typename T, template <typename> typename FE,
    //              template <typename> typename... E>
    //    struct apply_extensions {
    //        using type = apply_extensions<FE<T>, E...>;
    //    };
    //
    //    template <typename T, template <typename> typename FE>
    //    struct apply_extensions<T, FE> {
    //        using type = FE<T>;
    //    };
    //

} // namespace webpp

#endif // WEBPP_EXTENSION_H
