// Created by moisrex on 5/15/20.

#ifndef WEBPP_EXTENSION_H
#define WEBPP_EXTENSION_H

#include "../std/std.hpp"
#include "../std/tuple.hpp"
#include "../traits/default_traits.hpp"
#include "../traits/traits.hpp"

#include <cstdint>
#include <type_traits>

namespace webpp {

    // todo: check this, it's old
    template <typename T>
    concept Extension =
      stl::copy_constructible<T> && !stl::is_final_v<T> && stl::is_default_constructible_v<T> &&
      stl::is_move_constructible_v<T> && stl::is_move_assignable_v<T> && stl::is_copy_assignable_v<T> &&
      stl::is_class_v<T> && !stl::is_integral_v<T>;

    template <typename TraitsType, typename T>
    concept MotherExtension = Extension<T> && requires {
        typename T::template type<TraitsType>;
    };

    template <typename TraitsType, typename Parent, typename T>
    concept ChildExtension = Extension<T> && requires {
        typename T::template type<TraitsType, Parent>;
    };


    template <typename ExtensionDescriptorType, typename TraitsType>
    concept HasMidLevelExtensie = requires {
        ExtensionDescriptorType::template mid_level_extensie_type;
    };

    template <typename ExtensionDescriptorType, typename TraitsType>
    concept HasFinalExtensie = requires {
        ExtensionDescriptorType::template final_extensie_type;
    };

    template <Extension... E>
    struct extension_pack;

    namespace details {

        /**
         * This struct is used to ignore the constructor calls to the base classes that don't have the
         * necessary constructor.
         * This struct requires the base classes to be default constructible.
         * @tparam Parent
         */
        template <typename Parent>
        struct vctor : public virtual Parent {

            using Parent::Parent;
            //            template <typename... Args>
            //            requires(stl::constructible_from<Parent, Args...>) constexpr vctor(Args&&... args)
            //            noexcept
            //              : Parent{stl::forward<Args>(args)...} {}

            template <typename... Args>
            requires(!stl::constructible_from<Parent, Args...> &&
                     stl::is_default_constructible_v<
                       Parent>) constexpr vctor([[maybe_unused]] Args&&... args) noexcept
              : Parent{} {}
        };

        template <typename Parent>
        struct ctor : public Parent {

            using Parent::Parent;
            //            template <typename... Args>
            //            constexpr ctor(Args&&... args) noexcept requires(stl::constructible_from<Parent,
            //            Args...>)
            //              : Parent{stl::forward<Args>(args)...} {}

            template <typename... Args>
            constexpr ctor([[maybe_unused]] Args&&... args) noexcept
              requires(!stl::constructible_from<Parent, Args...> && stl::is_default_constructible_v<Parent>)
              : Parent{} {}
        };


        /// Mother

        template <Traits TraitsType, typename Es>
        struct mother_inherited;

        template <Traits TraitsType, Extension... E>
        struct mother_inherited<TraitsType, extension_pack<E...>> {
            struct type : public ctor<typename E::template type<TraitsType>>... {
                using ctor<typename E::template type<TraitsType>>::ctor...;
            };
        };

        template <Traits TraitsType, Extension E>
        struct mother_inherited<TraitsType, extension_pack<E>> {
            using type = typename E::template type<TraitsType>;
        };


        /// Child

        template <Traits TraitsType, typename Mother, typename... Kids>
        struct children_inherited;

        // with 2 or more kids
        template <Traits TraitsType, typename Mother, typename FirstKid, typename... Kids>
        struct children_inherited<TraitsType, Mother, FirstKid, Kids...> {
            using type = typename FirstKid::
              template type<TraitsType, typename children_inherited<TraitsType, Mother, Kids...>::type>;
            //            struct type : public vctor<typename Kids::template type<TraitsType,
            //            vctor<Mother>>>... {
            //                template <typename... Args>
            //                constexpr type(Args&&... args) noexcept
            //                  : vctor<typename Kids::template type<TraitsType, vctor<Mother>>>{
            //                      stl::forward<Args>(args)...}... {}
            //            };
        };

        // without any kids
        template <Traits TraitsType, typename Mother>
        struct children_inherited<TraitsType, Mother> {
            using type = Mother;
        };

        // with only one kid
        template <Traits TraitsType, typename Mother, typename Kid>
        struct children_inherited<TraitsType, Mother, Kid> {
            using type = typename Kid::template type<TraitsType, Mother>;
        };

        // passed with an extension pack
        template <Traits TraitsType, typename Mother, typename... Kids>
        struct children_inherited<TraitsType, Mother, extension_pack<Kids...>>
          : public children_inherited<TraitsType, Mother, Kids...> {};





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





        template <template <typename...> typename PackType, typename... F>
        struct prepend {
            using type = PackType<F...>;
        };

        template <template <typename...> typename PackType, typename F, typename... L>
        struct prepend<PackType, F, extension_pack<L...>> {
            using type = PackType<F, L...>;
        };

        template <template <typename...> typename PackType,
                  template <typename>
                  typename IF,
                  typename First = void,
                  typename... EI>
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
              ((!std::is_same_v<First, U>) &&...),
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

        template <typename E>
        struct unique_extensions;

        template <typename... E>
        struct unique_extensions<extension_pack<E...>> : unique_types<E...> {};


        template <Traits TraitsType>
        struct is_mother_condition {
            template <typename T>
            struct type {
                static constexpr bool value = MotherExtension<TraitsType, T>;
            };
        };

        template <Traits TraitsType, typename Parent>
        struct is_child_condition {
            template <typename T>
            struct type {
                static constexpr bool value = ChildExtension<TraitsType, Parent, T>;
            };
        };


        template <template <typename...> typename PackType,
                  template <typename>
                  typename Extractor,
                  typename... EPack>
        struct epack_miner;

        template <template <typename...> typename PackType,
                  template <typename>
                  typename Extractor,
                  typename... EPack>
        struct epack_miner<PackType, Extractor, extension_pack<EPack...>> {
            using type = PackType<Extractor<EPack>...>;
        };

        template <typename ExtensieDescriptor>
        struct has_related_extension_condition {
            template <typename ExtensionType>
            struct type {
                static constexpr bool value = requires {
                    typename ExtensieDescriptor::template related_extension_pack_type<ExtensionType>;
                };
            };
        };

        template <typename RootExtensionPack, typename ExtensieDescriptor, template <typename> typename IF>
        using merge_extensions = typename details::unique_types<
          typename details::flatten_epacks<typename details::epack_miner<
            extension_pack,
            ExtensieDescriptor::template related_extension_pack_type,

            // filter the packs that contain the interested packs
            typename details::filter_epack<extension_pack,
                                           has_related_extension_condition<ExtensieDescriptor>::template type,
                                           RootExtensionPack>::type

            >::type>::type
          // append the individual lonely extensions in the big epack
          ::template appended<typename details::filter_epack<extension_pack, IF, RootExtensionPack>::type>>::
          type;



        template <typename RootExtensionPack,
                  typename TraitsType,
                  typename ExtensieDescriptor,
                  typename... ExtraArgs>
        struct mid_level_extractor {

            using mother_pack = typename mother_inherited<
              TraitsType,
              typename merge_extensions<RootExtensionPack,
                                        ExtensieDescriptor,
                                        is_mother_condition<TraitsType>::template type>::
                template mother_extensions<TraitsType>>::type;

            template <typename T>
            struct extractor {
                using type = typename T::
                  template mid_level_extensie_type<RootExtensionPack, TraitsType, mother_pack, ExtraArgs...>;
            };

            // if we have a mid-level extensie type:
            //    - Okay, pass the "mother pack" to the "mid-level extensie"
            // if not:
            //    - The just use the "mother pack" as the extensie type
            using type = istl::lazy_conditional_t<HasMidLevelExtensie<ExtensieDescriptor, TraitsType>,
                                                  istl::templated_lazy_type<extractor, ExtensieDescriptor>,
                                                  istl::lazy_type<mother_pack>>;
        };

        // Mid-Level extensie type
        template <typename RootExtensionPack,
                  Traits TraitsType,
                  typename ExtensieDescriptor,
                  typename... ExtraArgs>
        using mid_level_extensie_type =
          typename mid_level_extractor<RootExtensionPack, TraitsType, ExtensieDescriptor, ExtraArgs...>::type;





        template <typename RootExtensionPack,
                  typename TraitsType,
                  typename ExtensieDescriptor,
                  typename... ExtraArgs>
        struct final_extensie_extractor {

            // Mid-Level extensie children (will extend the mid-level extensie and will be extended by the
            // final extensie)
            using mid_level_extensie_children = typename details::merge_extensions<
              RootExtensionPack,
              ExtensieDescriptor,
              details::is_child_condition<
                TraitsType,
                details::
                  mid_level_extensie_type<RootExtensionPack, TraitsType, ExtensieDescriptor, ExtraArgs...>>::
                template type>::template child_extensions<TraitsType,
                                                          details::mid_level_extensie_type<RootExtensionPack,
                                                                                           TraitsType,
                                                                                           ExtensieDescriptor,
                                                                                           ExtraArgs...>>;

            using mother_pack = typename ExtensieDescriptor::template final_extensie_type<
              RootExtensionPack,
              TraitsType,

              // child extensions + the mid-level extensie + mother extensions
              typename details::children_inherited<
                TraitsType,
                details::
                  mid_level_extensie_type<RootExtensionPack, TraitsType, ExtensieDescriptor, ExtraArgs...>,
                mid_level_extensie_children>::type,
              ExtraArgs...>;

            template <typename T>
            struct extractor {
                using type = typename T::
                  template final_extensie_type<RootExtensionPack, TraitsType, mother_pack, ExtraArgs...>;
            };

            // if we have a final extensie type:
            //    - Okay, pass the "mother pack" to the "mid-level extensie"
            // if not:
            //    - The just use the "mother pack" as the extensie type
            using type = istl::lazy_conditional_t<HasFinalExtensie<ExtensieDescriptor, TraitsType>,
                                                  istl::templated_lazy_type<extractor, ExtensieDescriptor>,
                                                  istl::lazy_type<mother_pack>>;
        };


    } // namespace details

    template <Extension... E>
    struct extension_pack {

        template <typename... NE>
        using appended = extension_pack<E..., NE...>;

        template <Traits TraitsType>
        using mother_extensions = typename details::
          filter<extension_pack, details::is_mother_condition<TraitsType>::template type, E...>::type;

        template <Traits TraitsType, typename Parent>
        using child_extensions = typename details::
          filter<extension_pack, details::is_child_condition<TraitsType, Parent>::template type, E...>::type;

        using this_epack = extension_pack<E...>;



        /**
         * Apply extensions into one type
         * todo: first filter based on extensie, then filter based on mother or child
         */
        template <Traits TraitsType, typename ExtensieDescriptor, typename... ExtraArgs>
        using extensie_type = typename details::
          final_extensie_extractor<this_epack, TraitsType, ExtensieDescriptor, ExtraArgs...>::type;


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
        struct related_extension_pack_type {};

        template <typename EPackType, typename TraitsType, typename EList>
        struct mid_level_extensie_type : public EList {};

        // empty final extensie
        template <typename EPackType, typename TraitsType, typename EList>
        struct final_extensie_type final : public EList {};
    };

    template <typename E>
    concept ExtensionList = requires {
        typename E::template extensie_type<default_traits, fake_extensie_descriptor>;
        // typename E::template is_all<fake_extensie_descriptor::template has_related_extension_pack>;
    };

    template <typename E, template <typename> typename IF>
    concept ExtensionListOf = ExtensionList<E> && E::template is_all<IF>::value;


    template <typename T>
    concept RootExtensionList = sizeof(T) <= 1;


    struct empty_root_extension_lists{};
    using empty_extension_pack = extension_pack<>;



    /**
     * This type is used to ease the pain of passing arguments to the constructors of the extensions
     *
     * The type E might not be just one extension and might be an series of extensions chained up in a virtual
     * or non-virtual inheritance.
     */
    template <typename E>
    using extension_wrapper = E;

    template <typename T>
    concept ExtensionDescriptor = requires {
        typename T::template related_extension_pack<empty_extension_pack>;
    };


    namespace details {

        template <ExtensionList EList, ExtensionDescriptor EDesc>
        struct extension_extractor;

    }

    template <ExtensionList EList, ExtensionDescriptor EDesc>
    using extension_extractor = void;


} // namespace webpp

#endif // WEBPP_EXTENSION_H
