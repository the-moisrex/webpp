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
    static constexpr bool item = true;
};
struct two {
    static constexpr bool item = true;
};
struct three {
    static constexpr bool item = true;
};

template <typename T>
struct has_item {
    static constexpr bool value = T::item;
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
        bool mid_level = true;
    };

    template <typename ExtensionListType, typename TraitsType, typename EList>
    struct final_extensie_type : public EList {
        bool final_level = true;
    };
};

TEST(ExtensionsTests, ExtensionPackStuff) {
    using pack = extension_pack<one, two, three>;
    EXPECT_TRUE(pack::template is_all<has_item>::value);

    using etype = typename pack::template extensie_type<std_traits, fake_descriptor>;
    etype e{};
    EXPECT_TRUE(e.mid_level);
    EXPECT_TRUE(e.final_level);
}
