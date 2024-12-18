
#include "../webpp/unicode/normalization.hpp"
#include "common/fuzz_common.hpp"

void unicode_fuzz(std::string_view data) {
    using webpp::unicode::toNFC;

    std::string const str8{data.data(), data.size()};
    auto const        res = toNFC<std::string>(str8);
    if (!str8.empty()) {
        ASSERT_NE(str8.size(), 0);
        ASSERT_NE(res.size(), 0);
    }
}

register_fuzz(unicode_fuzz);
