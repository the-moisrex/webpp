
#include "../webpp/unicode/normalization.hpp"
#include "common/fuzz_common.hpp"

// NOLINTBEGIN(*-pro-type-reinterpret-cast)
void unicode_fuzz(std::string_view data) {
    using webpp::unicode::toNFC;

    std::string const    str{data.data(), data.size()};
    std::u8string const  str8{reinterpret_cast<char8_t const*>(data.data()), data.size()};
    std::u16string const str16{reinterpret_cast<char16_t const*>(data.data()), data.size() / 2};
    std::u32string const str32{reinterpret_cast<char32_t const*>(data.data()), data.size() / 4};
    auto const           res   = toNFC<std::string>(str);
    auto const           res8  = toNFC<std::u8string>(str8);
    auto const           res16 = toNFC<std::u16string>(str16);
    auto const           res32 = toNFC<std::u32string>(str32);
    if (!str8.empty()) {
        ASSERT_NE(str8.size(), 0);
        ASSERT_NE(str16.size(), 0);
        ASSERT_NE(str32.size(), 0);
        ASSERT_NE(str.size(), 0);
        ASSERT_NE(res.size(), 0);
        ASSERT_NE(res16.size(), 0);
        ASSERT_NE(res8.size(), 0);
        ASSERT_NE(res32.size(), 0);
    }
}

// NOLINTEND(*-pro-type-reinterpret-cast)

register_fuzz(unicode_fuzz);
