#include "../webpp/uri/idna/punycodes.hpp"
#include "common/tests_common_pch.hpp"

TEST(PunycodeTests, BasicEncoding) {
    using webpp::uri::idna::punycode_of;
    EXPECT_EQ(punycode_of("ドメイン名例"), "eckwd4c7cu47r2wf");
    EXPECT_EQ(punycode_of("موبايلي"), "mgbb9fbpob");
    EXPECT_EQ(punycode_of("-> $1.00 <-"), "-> $1.00 <--");
    EXPECT_EQ(punycode_of("😉"), "n28h");

    // let's leave the rest of the testing to the other functions
}
