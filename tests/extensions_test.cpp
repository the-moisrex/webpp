#include "../core/include/webpp/extensions/extension.h"
#include "../core/include/webpp/std/concepts.h"
#include "../core/include/webpp/traits/std_traits.h"

#include <gtest/gtest.h>

using namespace webpp;

TEST(ExtensionsTests, ExtensionConcepts) {
    EXPECT_FALSE(static_cast<bool>(Extension<int>));

    EXPECT_TRUE(static_cast<bool>(Extension<fake_extension>));
    EXPECT_TRUE(static_cast<bool>(ExtensionList<::std::tuple<fake_extension, fake_extension>>));
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

struct cone {
    static constexpr bool item     = true;
    static constexpr bool item_one = true;
    template <typename tt, typename Daddy>
    struct type : public virtual Daddy {
        static constexpr bool cvalue_one = true;
    };
};
struct ctwo {
    static constexpr bool item     = true;
    static constexpr bool item_two = true;
    template <typename tt, typename Daddy>
    struct type : public virtual Daddy {
        static constexpr bool cvalue_two = true;
    };
};
struct cthree {
    static constexpr bool item       = true;
    static constexpr bool item_three = true;

    template <typename tt, typename Daddy>
    struct type : public virtual Daddy {
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

struct fake_descriptor {
    template <typename ExtensionType>
    struct has_related_extension_pack {
        static constexpr bool value = requires {
            typename ExtensionType::fake_extensions;
        };
    };

    template <typename ExtensionType>
    using related_extension_pack_type = typename ExtensionType::fake_extensions;

    template <typename ExtensionListType, typename TraitsType, typename EList>
    struct mid_level_extensie_type : public EList {
        static constexpr bool mid_level = true;
    };

    template <typename ExtensionListType, typename TraitsType, typename EList>
    struct final_extensie_type : public EList {
        static constexpr bool final_level = true;
    };
};

using pack   = extension_pack<one, two, three>;
using cpack  = extension_pack<cone, ctwo, cthree>;
using expack = extension_pack<exes>;


TEST(ExtensionsTests, ExtensionPackStuff) {
    EXPECT_TRUE(pack::template is_all<has_item>::value);

    static_assert(stl::same_as<typename pack::mother_extensions, pack>,
                  "Extension system is not able to identify the mother extensions");

    static_assert(stl::same_as<typename pack::child_extensions, empty_extension_pack>,
                  "Extension system is not able to identify the child extensions");

    static_assert(stl::same_as<typename extension_pack<exes>::merge_extensions<
                                 fake_descriptor, empty_extension_pack::template mother_type>,
                               pack>,
                  "Cannot merge the extensions");

    static_assert(stl::same_as<typename extension_pack<exes, exes, exes>::merge_extensions<
                                 fake_descriptor, empty_extension_pack::template mother_type>,
                               pack>,
                  "epack is failing at making the extensions unique");

    typename pack::template mother_inherited<std_traits> ipack;

    EXPECT_TRUE(ipack.value_one);
    EXPECT_TRUE(ipack.value_two);
    EXPECT_TRUE(ipack.value_three);

    struct daddy {
        bool daddy_value = true;
    };
    typename cpack::template children_inherited<std_traits, daddy> icpack;

    EXPECT_TRUE(icpack.cvalue_one);
    EXPECT_TRUE(icpack.cvalue_two);
    EXPECT_TRUE(icpack.cvalue_three);
    EXPECT_TRUE(icpack.daddy_value);


    using iexpack = typename expack::template merge_extensions<
      fake_descriptor,
      empty_extension_pack::template mother_type>::mother_extensions::template mother_inherited<std_traits>;
    auto ii = iexpack{};
    EXPECT_TRUE(ii.value_one && ii.value_two && ii.value_three) << "Inherited is not working";

    using etype = typename expack::template extensie_type<std_traits, fake_descriptor>;
    etype e{};
    EXPECT_TRUE(e.mid_level);
    EXPECT_TRUE(e.final_level);
}

struct ctor_one {
    template <typename TraitsType>
    struct type {
        int a;
        type(int a, int b) : a{a + b} {
        }
    };
};

TEST(ExtensionsTests, ExtensionConstructors) {
    using ctor_pack  = extension_pack<ctor_one>;
    using ictor_pack = typename ctor_pack::template mother_inherited<std_traits>;

    EXPECT_EQ((ictor_pack{4, 2}.a), 6);
}