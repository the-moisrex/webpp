// Created by moisrex on 5/15/20.

#ifndef WEBPP_EXTENSION_HPP
#define WEBPP_EXTENSION_HPP

#include "../std/std.hpp"
#include "../std/tuple.hpp"
#include "../std/type_traits.hpp"
#include "../traits/default_traits.hpp"
#include "../traits/traits.hpp"

#include <cstdint>

namespace webpp {

    // todo: check this, it's old
    template <typename T>
    concept Extension = stl::copy_constructible<T> && !
    stl::is_final_v<T>&& stl::is_default_constructible_v<T>&& stl::is_move_constructible_v<T> &&
      !stl::is_integral_v<T>;

    template <typename TraitsType, typename T>
    concept MotherExtension = Extension<T> && requires { typename T::template type<TraitsType>; };

    template <typename TraitsType, typename Parent, typename T>
    concept ChildExtension = Extension<T> && requires { typename T::template type<TraitsType, Parent>; };


    template <typename ExtensionDescriptorType, typename... Args>
    concept HasMidLevelExtensie =
      requires { typename ExtensionDescriptorType::template mid_level_extensie_type<Args...>; };

    template <typename ExtensionDescriptorType, typename... Args>
    concept HasFinalExtensie =
      requires { typename ExtensionDescriptorType::template final_extensie_type<Args...>; };

    template <typename T>
    concept HasDependencies = requires { typename T::dependencies; };

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
            // template <typename... Args>
            // requires(stl::constructible_from<Parent, Args...>) constexpr vctor(Args&&... args)
            // noexcept
            //   : Parent{stl::forward<Args>(args)...} {}

            template <typename... Args>
                requires(!stl::constructible_from<Parent, Args...> && stl::is_default_constructible_v<Parent>)
            constexpr vctor([[maybe_unused]] Args&&... args) noexcept : Parent{} {}
        };

        template <typename Parent>
        struct ctor : public Parent {

            using Parent::Parent;
            // template <typename... Args>
            // constexpr ctor(Args&&... args) noexcept requires(stl::constructible_from<Parent,
            // Args...>)
            //   : Parent{stl::forward<Args>(args)...} {}

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

        // Empty extension pack will result in using a nothing_type as the parent since we're not able to
        // inherit from void.
        template <Traits TraitsType>
        struct mother_inherited<TraitsType, extension_pack<>> {
            using type = istl::nothing_type;
        };


        /// Child

        template <Traits TraitsType, typename Mother, typename... Kids>
        struct children_inherited;

        // with 2 or more kids
        template <Traits TraitsType, typename Mother, typename FirstKid, typename... Kids>
        struct children_inherited<TraitsType, Mother, FirstKid, Kids...> {
            using type = typename FirstKid::
              template type<TraitsType, typename children_inherited<TraitsType, Mother, Kids...>::type>;
            //  struct type : public vctor<typename Kids::template type<TraitsType,
            //  vctor<Mother>>>... {
            //      template <typename... Args>
            //      constexpr type(Args&&... args) noexcept
            //        : vctor<typename Kids::template type<TraitsType, vctor<Mother>>>{
            //            stl::forward<Args>(args)...}... {}
            //  };
        };



        /**
         * This TMP will find, flatten, and re-orders the extensions and their dependencies.
         */
        template <typename... E>
        struct dependencies {
            using type = extension_pack<E...>;
        };
        template <typename... E, typename... Es>
        struct dependencies<extension_pack<E...>, Es...> : dependencies<E..., Es...> {};

        template <HasDependencies First, typename... E>
        struct dependencies<First, E...> {
            using deps           = typename First::dependencies;      // extension_pack<...>
            using deps_with_deps = typename dependencies<deps>::type; // extension_pack<...>

            // the "First" needs to go before the "deps" because we order matters here
            // the children_inherited will inherit each of them in reverse order, so ...
            using type = istl::
              merge_parameters<extension_pack<First>, deps_with_deps, typename dependencies<E...>::type>;
        };

        template <typename First, typename... E>
        struct dependencies<First, E...> {
            using type = istl::merge_parameters<extension_pack<First>, typename dependencies<E...>::type>;
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


        template <template <typename> typename Extractor, typename... EPack>
        struct epack_miner;

        template <template <typename> typename Extractor, typename... EPack>
        struct epack_miner<Extractor, extension_pack<EPack...>> {
            using type = extension_pack<Extractor<EPack>...>;
        };

        template <typename ExtensieDescriptor>
        struct has_related_extension_condition {
            template <typename ExtensionType>
            struct type {
                static constexpr bool value =
                  requires { typename ExtensieDescriptor::template extractor_type<ExtensionType>; };
            };
        };

        template <typename RootExtensionPack, typename ExtensieDescriptor, template <typename> typename IF>
        using merge_extensions = typename istl::unique_parameters<
          typename dependencies<typename flatten_epacks<typename epack_miner<
            ExtensieDescriptor::template extractor_type,

            // filter the packs that contain the interested packs
            istl::filter_parameters_t<has_related_extension_condition<ExtensieDescriptor>::template type,
                                      RootExtensionPack>

            >::type>::type
                                // append the individual lonely extensions in the big epack
                                ::template appended<istl::filter_parameters_t<IF, RootExtensionPack>>>::type>;



        template <typename RootExtensionPack,
                  typename TraitsType,
                  typename ExtensieDescriptor,
                  typename... ExtraArgs>
        struct mid_level_extractor {

            using mother_pack = istl::unique_parameters<typename merge_extensions<
              RootExtensionPack,
              ExtensieDescriptor,
              is_mother_condition<TraitsType>::template type>::template mother_extensions<TraitsType>>;

            // these are the applied mother extensions
            using applied_mother_pack = typename mother_inherited<TraitsType, mother_pack>::type;

            template <typename T>
            struct extractor {
                using type = typename T::template mid_level_extensie_type<RootExtensionPack,
                                                                          TraitsType,
                                                                          applied_mother_pack,
                                                                          ExtraArgs...>;
            };

            // if we have a mid-level extensie type:
            //    - Okay, pass the "mother pack" to the "mid-level extensie"
            // if not:
            //    - The just use the "mother pack" as the extensie type
            using type = istl::lazy_conditional_t<HasMidLevelExtensie<ExtensieDescriptor,
                                                                      RootExtensionPack,
                                                                      TraitsType,
                                                                      applied_mother_pack,
                                                                      ExtraArgs...>,
                                                  istl::templated_lazy_type<extractor, ExtensieDescriptor>,
                                                  istl::lazy_type<applied_mother_pack>>;
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
            using mid_level_extensie_children = typename merge_extensions<
              RootExtensionPack,
              ExtensieDescriptor,
              is_child_condition<
                TraitsType,
                mid_level_extensie_type<RootExtensionPack, TraitsType, ExtensieDescriptor, ExtraArgs...>>::
                template type>::
              template child_extensions<
                TraitsType,
                mid_level_extensie_type<RootExtensionPack, TraitsType, ExtensieDescriptor, ExtraArgs...>>;

            // child extensions + the mid-level extensie + mother extensions
            using mother_pack = typename children_inherited<
              TraitsType,
              mid_level_extensie_type<RootExtensionPack, TraitsType, ExtensieDescriptor, ExtraArgs...>,
              mid_level_extensie_children>::type;

            template <typename T>
            struct extractor {
                using type = typename T::
                  template final_extensie_type<RootExtensionPack, TraitsType, mother_pack, ExtraArgs...>;
            };

            // if we have a final extensie type:
            //    - Okay, pass the "mother pack" to the "mid-level extensie"
            // if not:
            //    - The just use the "mother pack" as the extensie type
            using type = istl::lazy_conditional_t<
              HasFinalExtensie<ExtensieDescriptor, RootExtensionPack, TraitsType, mother_pack, ExtraArgs...>,
              istl::templated_lazy_type<extractor, ExtensieDescriptor>,
              istl::lazy_type<mother_pack>>;
        };


    } // namespace details

    template <Extension... E>
    struct extension_pack {

        // these are the dependencies
        using include_dependencies = typename details::dependencies<E...>::type;

        template <typename... NE>
        using appended = extension_pack<E..., NE...>;

        // This type does not handle the dependencies
        template <Traits TraitsType>
        using mother_extensions =
          istl::filter_parameters_t<details::is_mother_condition<TraitsType>::template type,
                                    include_dependencies>;

        // This type does not handle the dependencies
        template <Traits TraitsType, typename Parent>
        using child_extensions =
          istl::filter_parameters_t<details::is_child_condition<TraitsType, Parent>::template type,
                                    include_dependencies>;

        using this_epack = extension_pack<E...>;


        // this will apply only the "Mother Extension" and gives you the result of that.
        // this does not apply the child extensions
        template <Traits TraitsType, typename ExtensieDescriptor>
        using mother_extensie_type = typename details::
          mid_level_extractor<include_dependencies, TraitsType, ExtensieDescriptor>::applied_mother_pack;

        /**
         * Apply extensions into one type
         * todo: first filter based on extensie, then filter based on mother or child
         */
        template <Traits TraitsType, typename ExtensieDescriptor, typename... ExtraArgs>
        using extensie_type = typename details::
          final_extensie_extractor<include_dependencies, TraitsType, ExtensieDescriptor, ExtraArgs...>::type;


        /**
         * Check if all the extensions are the correct type
         * @tparam IF
         */
        template <template <typename> typename IF>
        struct is_all {
            static constexpr bool value = (IF<E>::value && ...);
        };
    };



    struct fake_extensie_descriptor {
        template <typename ExtensionType>
        struct extractor_type {};

        template <typename EPackType, typename TraitsType, typename EList>
        struct mid_level_extensie_type : public EList {};

        // empty final extensie
        template <typename EPackType, typename TraitsType, typename EList>
        struct final_extensie_type final : public EList {};
    };

    template <typename E>
    concept ExtensionList = requires {
                                typename E::template extensie_type<default_traits, fake_extensie_descriptor>;
                                // typename E::template is_all<fake_extensie_descriptor::template
                                // has_related_extension_pack>;
                            };

    template <typename E, template <typename> typename IF>
    concept ExtensionListOf = ExtensionList<E> && E::template
    is_all<IF>::value;


    /**
     * The root extension list,
     * An extension list of extension lists which include extension lists
     *
     * @code
     *   extension_list<
     *     extension_list<
     *       struct {
     *         protocol_extensions = {Extension, ...},
     *         ...
     *       },
     *       ...
     *     >
     *   >
     * @endcode
     */
    template <typename T>
    concept RootExtensionList = ExtensionList<T>;


    using empty_extension_pack = extension_pack<>;



    template <typename T>
    concept ExtensionDescriptor =
      requires { typename T::template related_extension_pack<empty_extension_pack>; };


    namespace details {

        template <ExtensionList EList, ExtensionDescriptor EDesc>
        struct extension_extractor;

    }

    template <ExtensionList EList, ExtensionDescriptor EDesc>
    using extension_extractor = void;



    /**
     * This struct will help the user to create an extension from
     * a templated type and package it in a simple type.
     */
    template <template <typename...> typename TemplatedExtension>
    struct as_extension {
        template <typename... T>
            requires requires { typename TemplatedExtension<T...>; }
        using type = TemplatedExtension<T...>;
    };


    template <RootExtensionList... RootExtensions>
    struct merge_root_extensions;

    template <typename... E1, typename... E2>
    struct merge_root_extensions<extension_pack<E1...>, extension_pack<E2...>> {
        using type = extension_pack<E1..., E2...>;
    };

    template <typename... E1, typename... E2, RootExtensionList... RootExtensions>
    struct merge_root_extensions<extension_pack<E1...>, extension_pack<E2...>, RootExtensions...>
      : merge_root_extensions<extension_pack<E1..., E2...>, RootExtensions...> {};


} // namespace webpp

#endif // WEBPP_EXTENSION_HPP
