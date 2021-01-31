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

    template <typename TraitsType, typename T>
    concept MotherExtension = Extension<T> && requires {
        typename T::template type<TraitsType>;
    };

    template <typename TraitsType, typename Parent, typename T>
    concept ChildExtension = Extension<T> && requires {
        typename T::template type<TraitsType, Parent>;
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

        template <Traits TraitsType, typename Es>
        struct mother_inherited;

        template <Traits TraitsType, Extension... E>
        struct mother_inherited<TraitsType, extension_pack<E...>>
          : public ctor<typename E::template type<TraitsType>>... {

            template <typename... Args>
            constexpr mother_inherited(Args&&... args) noexcept
              : ctor<typename E::template type<TraitsType>>{stl::forward<Args>(args)...}... {}
        };

        // with 2 or more kids
        template <Traits TraitsType, typename Mother, typename... Kids>
        struct children_inherited {
            struct type : public vctor<typename Kids::template type<TraitsType, vctor<Mother>>>... {
                template <typename... Args>
                constexpr type(Args&&... args) noexcept
                  : vctor<typename Kids::template type<TraitsType, vctor<Mother>>>{
                      stl::forward<Args>(args)...}... {}
            };
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
        struct children_inherited<TraitsType, Mother, extension_pack<Kids...>> {
            using type = typename children_inherited<TraitsType, Mother, Kids...>::type;
        };





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
        struct mother_type {
            template <typename T>
            struct type {
                static constexpr bool value = MotherExtension<TraitsType, T>;
            };
        };

        template <Traits TraitsType, typename Parent>
        struct child_type {
            template <typename T>
            struct type {
                static constexpr bool value = ChildExtension<TraitsType, Parent, T>;
            };
        };


        template <template <typename...> typename PackType,
                  template <typename>
                  typename Extractor,
                  typename... EPack>
        struct epack_miner {};

        template <template <typename...> typename PackType,
                  template <typename>
                  typename Extractor,
                  typename... EPack>
        struct epack_miner<PackType, Extractor, extension_pack<EPack...>> {
            using type = PackType<Extractor<EPack>...>;
        };



    } // namespace details

    template <Extension... E>
    struct extension_pack {

        template <typename... NE>
        using appended = extension_pack<E..., NE...>;

        template <Traits TraitsType>
        using mother_extensions = typename details::
          filter<extension_pack, details::mother_type<TraitsType>::template type, E...>::type;

        template <Traits TraitsType, typename Parent>
        using child_extensions = typename details::
          filter<extension_pack, details::child_type<TraitsType, Parent>::template type, E...>::type;

        using this_epack = extension_pack<E...>;



        template <typename ExtensieDescriptor, template <typename> typename IF>
        using merge_extensions = typename details::unique_types<
          typename details::flatten_epacks<typename details::epack_miner<
            extension_pack,
            ExtensieDescriptor::template related_extension_pack_type,

            // filter the packs that contain the interested packs
            typename details::filter_epack<extension_pack,
                                           ExtensieDescriptor::template has_related_extension_pack,
                                           this_epack>::type

            >::type>::type
          // append the individual lonely extensions in the big epack
          ::template appended<typename details::filter_epack<extension_pack, IF, this_epack>::type>>::type;



        // Mid-Level extensie type
        template <Traits TraitsType, typename ExtensieDescriptor, typename... ExtraArgs>
        using mid_level_extensie_type = typename ExtensieDescriptor::template mid_level_extensie_type<
          this_epack,
          TraitsType,
          details::mother_inherited<
            TraitsType,
            typename details::unique_extensions<
              typename merge_extensions<ExtensieDescriptor, details::mother_type<TraitsType>::template type>::
                template mother_extensions<TraitsType>>::type>,
          ExtraArgs...>;


        // Mid-Level extensie children (will extend the mid-level extensie and will be extended by the final
        // extensie)
        template <Traits TraitsType, typename ExtensieDescriptor, typename... ExtraArgs>
        using mid_level_extensie_children = typename merge_extensions<
          ExtensieDescriptor,
          details::child_type<
            TraitsType,
            mid_level_extensie_type<TraitsType, ExtensieDescriptor, ExtraArgs...>>::template type>::
          template child_extensions<TraitsType,
                                    mid_level_extensie_type<TraitsType, ExtensieDescriptor, ExtraArgs...>>;


        /**
         * Apply extensions into one type
         * todo: first filter based on extensie, then filter based on mother or child
         */
        template <Traits TraitsType, typename ExtensieDescriptor, typename... ExtraArgs>
        using extensie_type = typename ExtensieDescriptor::template final_extensie_type<
          this_epack,
          TraitsType,

          // child extensions + the mid-level extensie + mother extensions
          typename details::children_inherited<
            TraitsType,
            mid_level_extensie_type<TraitsType, ExtensieDescriptor, ExtraArgs...>,
            mid_level_extensie_children<TraitsType, ExtensieDescriptor, ExtraArgs...>>::type,

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
        typename E::template extensie_type<default_traits, fake_extensie_descriptor>;
        typename E::template is_all<fake_extensie_descriptor::template has_related_extension_pack>;
    };

    template <typename E, template <typename> typename IF>
    concept ExtensionListOf = ExtensionList<E> && E::template is_all<IF>::value;


    using empty_extension_pack = extension_pack<>;



    /**
     * This type is used to ease the pain of passing arguments to the constructors of the extensions
     *
     * The type E might not be just one extension and might be an series of extensions chained up in a virtual
     * or non-virtual inheritance.
     */
    template <typename E>
    struct extension_wrapper : public E {
        using E::E;
    };


} // namespace webpp

#endif // WEBPP_EXTENSION_H
