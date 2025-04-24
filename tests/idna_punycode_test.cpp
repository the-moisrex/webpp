#include "../webpp/unicode/punycodes.hpp"
#include "./common/tests_common_pch.hpp"

TEST(PunycodeTests, BasicEncoding) {
    using webpp::unicode::idna::punycode_of;
    EXPECT_EQ(punycode_of("ドメイン名例"), "eckwd4c7cu47r2wf");
    EXPECT_EQ(punycode_of("موبايلي"), "mgbb9fbpob");
    EXPECT_EQ(punycode_of("-> $1.00 <-"), "-> $1.00 <--");
    EXPECT_EQ(punycode_of("😉"), "n28h");
    EXPECT_EQ(punycode_of("αβγ"), "mxacd");
    EXPECT_EQ(punycode_of("Lloyd-Atkinson"), "Lloyd-Atkinson-");
    EXPECT_EQ(punycode_of("MajiでKoiする5秒前"), "MajiKoi5-783gue6qz075azm5e");
    EXPECT_EQ(punycode_of("Mnchen-3ya"), "Mnchen-3ya-");
    EXPECT_EQ(punycode_of("「bücher」"), "bcher-kva8445foa");
    EXPECT_EQ(punycode_of("--"), "---");
    EXPECT_EQ(punycode_of("This has spaces"), "This has spaces-");
    EXPECT_EQ(punycode_of("α"), "mxa");
    EXPECT_EQ(punycode_of("London"), "London-");
    EXPECT_EQ(punycode_of("а"), "80a");
    EXPECT_EQ(punycode_of("München"), "Mnchen-3ya");
    EXPECT_EQ(punycode_of("Bahnhof München-Ost"), "Bahnhof Mnchen-Ost-u6b");
    EXPECT_EQ(punycode_of("ยจฆฟคฏข"), "22cdfh1b8fsa");
    EXPECT_EQ(punycode_of("ü"), "tda");
    EXPECT_EQ(punycode_of("团淄"), "3bs854c");
    EXPECT_EQ(punycode_of("例"), "fsq");
    EXPECT_EQ(punycode_of("abæcdöef"), "abcdef-qua4k");
    EXPECT_EQ(punycode_of("München-Ost"), "Mnchen-Ost-9db");
    EXPECT_EQ(punycode_of("a"), "a-");
    EXPECT_EQ(punycode_of("A"), "A-");
    EXPECT_EQ(punycode_of("😉"), "n28h");
    EXPECT_EQ(punycode_of("-> $1.00 <-"), "-> $1.00 <--");
    EXPECT_EQ(punycode_of("ドメイン名例"), "eckwd4c7cu47r2wf");
    EXPECT_EQ(punycode_of("правда"), "80aafi6cg");

    // let's leave the rest of the testing to the other functions
}
