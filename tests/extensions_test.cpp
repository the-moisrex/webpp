#include "../webpp/extensions/extension.hpp"
#include "../webpp/std/concepts.hpp"
#include "../webpp/traits/std_traits.hpp"
#include "common/tests_common_pch.hpp"



using namespace webpp;

TEST(ExtensionsTests, ExtensionConcepts) {
    EXPECT_FALSE(static_cast<bool>(Extension<int>));

    // EXPECT_TRUE(static_cast<bool>(Extension<fake_extension>));
    // EXPECT_FALSE(static_cast<bool>(ExtensionList<::std::tuple<fake_extension, fake_extension>>));
}

struct one {
    static constexpr bool item     = true;
    static constexpr bool item_one = true;

    template <typename tt>
    struct type {
        static constexpr bool value_one = true;
    };
};

struct two {
    static constexpr bool item     = true;
    static constexpr bool item_two = true;

    template <typename tt>
    struct type {
        static constexpr bool value_two = true;
    };
};

struct three {
    static constexpr bool item       = true;
    static constexpr bool item_three = true;

    template <typename tt>
    struct type {
        static constexpr bool value_three = true;
    };
};

struct child_one {
    static constexpr bool item     = true;
    static constexpr bool item_one = true;

    template <typename tt, typename mommy>
    struct type : mommy {
        static constexpr bool value_one = true;
    };
};

struct child_two {
    static constexpr bool item     = true;
    static constexpr bool item_two = true;

    template <typename tt, typename mommy>
    struct type : mommy {
        static constexpr bool value_two = true;
    };
};

struct cone {
    static constexpr bool item     = true;
    static constexpr bool item_one = true;

    template <typename tt, typename Daddy>
    struct type : public Daddy {
        static constexpr bool cvalue_one = true;
    };
};

struct ctwo {
    static constexpr bool item     = true;
    static constexpr bool item_two = true;

    template <typename tt, typename Daddy>
    struct type : public Daddy {
        static constexpr bool cvalue_two = true;
    };
};

struct cthree {
    static constexpr bool item       = true;
    static constexpr bool item_three = true;

    template <typename tt, typename Daddy>
    struct type : public Daddy {
        static constexpr bool cvalue_three = true;
    };
};

template <typename T>
struct has_item {
    static constexpr bool value = T::item;
};

struct exes {
    using fake_extensions = extension_pack<one, two, three>;
};

struct exes_with_kids {
    using fake_extensions = extension_pack<one, two, three, child_one, child_two>;
};

struct fake_descriptor {
    template <typename ExtensionType>
    using extractor_type = typename ExtensionType::fake_extensions;

    template <typename ExtensionListType, typename TraitsType, typename EList>
    struct mid_level_extensie_type : public EList {
        static constexpr bool mid_level = true;

        template <typename... Args>
        mid_level_extensie_type(Args&&... args) noexcept : EList{std::forward<Args>(args)...} {}
    };

    template <typename ExtensionListType, typename TraitsType, typename EList>
    struct final_extensie_type : public EList {
        static constexpr bool final_level = true;

        template <typename... Args>
        final_extensie_type(Args&&... args) noexcept : EList{std::forward<Args>(args)...} {}
    };
};

using pack   = extension_pack<one, two, three>;
using cpack  = extension_pack<cone, ctwo, cthree>;
using expack = extension_pack<exes>;

TEST(ExtensionsTests, ExtensionPackStuff) {
    EXPECT_TRUE(pack::template is_all<has_item>::value);

    static_assert(stl::same_as<typename pack::template mother_extensions<std_traits>, pack>,
                  "Extension system is not able to identify the mother extensions");

    //    static_assert(stl::same_as<typename pack::child_extensions, empty_extension_pack>,
    //                  "Extension system is not able to identify the child extensions");

    //    static_assert(stl::same_as<typename extension_pack<exes>::merge_extensions<
    //                                 fake_descriptor, empty_extension_pack::template mother_type>,
    //                               pack>,
    //                  "Cannot merge the extensions");

    //    static_assert(stl::same_as<typename extension_pack<exes, exes, exes>::merge_extensions<
    //                                 fake_descriptor, empty_extension_pack::template mother_type>,
    //                               pack>,
    //                  "epack is failing at making the extensions unique");

    typename details::mother_inherited<std_traits, pack>::type ipack;

    EXPECT_TRUE(ipack.value_one);
    EXPECT_TRUE(ipack.value_two);
    EXPECT_TRUE(ipack.value_three);

    struct daddy {
        bool daddy_value = true;
    };

    typename details::children_inherited<std_traits, daddy, cpack>::type icpack;

    EXPECT_TRUE(icpack.cvalue_one);
    EXPECT_TRUE(icpack.cvalue_two);
    EXPECT_TRUE(icpack.cvalue_three);
    EXPECT_TRUE(icpack.daddy_value);

    typename details::children_inherited<std_traits, daddy, extension_pack<cone>>::type icpack2;

    EXPECT_TRUE(icpack2.cvalue_one);
    EXPECT_TRUE(icpack2.daddy_value);


    //    using iexpack = typename expack::template merge_extensions<
    //      fake_descriptor,
    //      empty_extension_pack::template mother_type>::mother_extensions::template
    //      mother_inherited<std_traits>;
    //    auto ii = iexpack{};
    //    EXPECT_TRUE(ii.value_one && ii.value_two && ii.value_three) << "Inherited is not working";

    using etype = typename expack::template extensie_type<std_traits, fake_descriptor>;
    etype e{};
    EXPECT_TRUE(e.mid_level);
    EXPECT_TRUE(e.final_level);



    using mid_type =
      typename details::mid_level_extensie_type<extension_pack<exes>, std_traits, fake_descriptor>;
    mid_type mid;
    EXPECT_TRUE(mid.mid_level);

    //    using mid_kids_type =
    //      typename extension_pack<exes_with_kids>::template mid_level_extensie_children<std_traits,
    //                                                                                    fake_descriptor>;
    //    static_assert(stl::same_as<mid_kids_type, extension_pack<child_one, child_two>>,
    //                  "mid_level_extensie_children doesn't work properly");
}

struct ctor_one {
    template <typename TraitsType>
    struct type {
        int a  = 1;
        type() = default;

        type(int _a, int _b) : a{_a + _b} {}
    };
};

TEST(ExtensionsTests, ExtensionConstructors) {
    using ctor_pack  = extension_pack<ctor_one>;
    using ictor_pack = typename details::mother_inherited<std_traits, ctor_pack>::type;
    using etype      = typename ctor_pack::template extensie_type<std_traits, fake_descriptor>;

    static_assert(std::is_constructible_v<typename ctor_one::template type<std_traits>, int, int>,
                  "We cannot construct the ctor_one::type with two ints");

    static_assert(std::is_constructible_v<ictor_pack, int, int>,
                  "We cannot construct the ctor_one::type with two ints");

    auto d1 = ictor_pack{4, 2};
    EXPECT_EQ(d1.a, 6);
    EXPECT_EQ((etype{4, 2}.a), 6);
}

struct first {
    template <typename TraitsType>
    struct type {
        using first = int;
    };
};

struct second {
    using dependencies = extension_pack<first>;

    template <typename TraitsType, typename Mother>
    struct type : Mother {};
};

struct third {
    using dependencies = extension_pack<second>;

    template <typename TraitsType, typename Mother>
    struct type : Mother {};
};

static_assert(
  stl::same_as<typename details::dependencies<third>::type, extension_pack<third, second, first>>);

struct third_descriptor {
    template <typename ExtensionType>
    using extractor_type = typename ExtensionType::test_extensions;
};

struct third_pack {
    using test_extensions = extension_pack<third>;
};

using third_extensie =
  typename extension_pack<third_pack>::template extensie_type<std_traits, third_descriptor>;
static_assert(stl::same_as<typename third_extensie::first, int>);

struct fake_descriptor_no_final {
    template <typename ExtensionType>
    using extractor_type = typename ExtensionType::willnotexist;

    template <typename RootExtensions, typename TraitsType, typename EList>
    struct mid_level_extensie_type : public EList {
        static constexpr bool mid_level = true;

        template <typename... Args>
        mid_level_extensie_type(Args&&... args) noexcept : EList{std::forward<Args>(args)...} {}
    };

    // mno
};

TEST(ExtensionsTests, EmptyExtesnsionTest) {
    // check if an empty extension will cause the mid-level-extensie to be omitted or not
    using etype = typename empty_extension_pack::template extensie_type<std_traits, fake_descriptor_no_final>;
    EXPECT_TRUE(etype{}.mid_level);
}
