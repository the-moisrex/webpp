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
}

TEST(PunycodeTests, BasicDecoding) {
    using webpp::unicode::idna::unicode_of;
    EXPECT_EQ(unicode_of("eckwd4c7cu47r2wf"), "ドメイン名例");
    EXPECT_EQ(unicode_of("mgbb9fbpob"), "موبايلي");
    EXPECT_EQ(unicode_of("-> $1.00 <--"), "-> $1.00 <-");
    EXPECT_EQ(unicode_of("n28h"), "😉");
    EXPECT_EQ(unicode_of("mxacd"), "αβγ");
    EXPECT_EQ(unicode_of("Lloyd-Atkinson-"), "Lloyd-Atkinson");
    EXPECT_EQ(unicode_of("MajiKoi5-783gue6qz075azm5e"), "MajiでKoiする5秒前");
    EXPECT_EQ(unicode_of("Mnchen-3ya-"), "Mnchen-3ya");
    EXPECT_EQ(unicode_of("bcher-kva8445foa"), "「bücher」");
    EXPECT_EQ(unicode_of("---"), "--");
    EXPECT_EQ(unicode_of("This has spaces-"), "This has spaces");
    EXPECT_EQ(unicode_of("mxa"), "α");
    EXPECT_EQ(unicode_of("London-"), "London");
    EXPECT_EQ(unicode_of("80a"), "а");
    EXPECT_EQ(unicode_of("Mnchen-3ya"), "München");
    EXPECT_EQ(unicode_of("Bahnhof Mnchen-Ost-u6b"), "Bahnhof München-Ost");
    EXPECT_EQ(unicode_of("22cdfh1b8fsa"), "ยจฆฟคฏข");
    EXPECT_EQ(unicode_of("tda"), "ü");
    EXPECT_EQ(unicode_of("3bs854c"), "团淄");
    EXPECT_EQ(unicode_of("fsq"), "例");
    EXPECT_EQ(unicode_of("abcdef-qua4k"), "abæcdöef");
    EXPECT_EQ(unicode_of("Mnchen-Ost-9db"), "München-Ost");
    EXPECT_EQ(unicode_of("a-"), "a");
    EXPECT_EQ(unicode_of("A-"), "A");
    EXPECT_EQ(unicode_of("n28h"), "😉");
    EXPECT_EQ(unicode_of("-> $1.00 <--"), "-> $1.00 <-");
    EXPECT_EQ(unicode_of("eckwd4c7cu47r2wf"), "ドメイン名例");
    EXPECT_EQ(unicode_of("80aafi6cg"), "правда");
}

TEST(PunycodeTests, BasicDecodingUTF32) {
    using webpp::stl::u32string;
    using webpp::unicode::idna::unicode_of;
    EXPECT_EQ(unicode_of<u32string>("eckwd4c7cu47r2wf"), U"ドメイン名例");
    EXPECT_EQ(unicode_of<u32string>("mgbb9fbpob"), U"موبايلي");
    EXPECT_EQ(unicode_of<u32string>("-> $1.00 <--"), U"-> $1.00 <-");
    EXPECT_EQ(unicode_of<u32string>("n28h"), U"😉");
    EXPECT_EQ(unicode_of<u32string>("mxacd"), U"αβγ");
    EXPECT_EQ(unicode_of<u32string>("Lloyd-Atkinson-"), U"Lloyd-Atkinson");
    EXPECT_EQ(unicode_of<u32string>("MajiKoi5-783gue6qz075azm5e"), U"MajiでKoiする5秒前");
    EXPECT_EQ(unicode_of<u32string>("Mnchen-3ya-"), U"Mnchen-3ya");
    EXPECT_EQ(unicode_of<u32string>("bcher-kva8445foa"), U"「bücher」");
    EXPECT_EQ(unicode_of<u32string>("---"), U"--");
    EXPECT_EQ(unicode_of<u32string>("This has spaces-"), U"This has spaces");
    EXPECT_EQ(unicode_of<u32string>("mxa"), U"α");
    EXPECT_EQ(unicode_of<u32string>("London-"), U"London");
    EXPECT_EQ(unicode_of<u32string>("80a"), U"а");
    EXPECT_EQ(unicode_of<u32string>("Mnchen-3ya"), U"München");
    EXPECT_EQ(unicode_of<u32string>("Bahnhof Mnchen-Ost-u6b"), U"Bahnhof München-Ost");
    EXPECT_EQ(unicode_of<u32string>("22cdfh1b8fsa"), U"ยจฆฟคฏข");
    EXPECT_EQ(unicode_of<u32string>("tda"), U"ü");
    EXPECT_EQ(unicode_of<u32string>("3bs854c"), U"团淄");
    EXPECT_EQ(unicode_of<u32string>("fsq"), U"例");
    EXPECT_EQ(unicode_of<u32string>("abcdef-qua4k"), U"abæcdöef");
    EXPECT_EQ(unicode_of<u32string>("Mnchen-Ost-9db"), U"München-Ost");
    EXPECT_EQ(unicode_of<u32string>("a-"), U"a");
    EXPECT_EQ(unicode_of<u32string>("A-"), U"A");
    EXPECT_EQ(unicode_of<u32string>("n28h"), U"😉");
    EXPECT_EQ(unicode_of<u32string>("-> $1.00 <--"), U"-> $1.00 <-");
    EXPECT_EQ(unicode_of<u32string>("eckwd4c7cu47r2wf"), U"ドメイン名例");
    EXPECT_EQ(unicode_of<u32string>("80aafi6cg"), U"правда");
}

TEST(PunycodeTests, BasicDecodingIterator) {
    using webpp::stl::string_view;
    using webpp::stl::u32string;
    using webpp::unicode::idna::punycode_decode;
    using webpp::unicode::idna::unicode_of;
    u32string out;
    out.resize(100);
    string_view const inp = "MajiKoi5-783gue6qz075azm5e";
    auto              ptr = out.begin();
    EXPECT_EQ(punycode_decode(inp.begin(), inp.end(), ptr), webpp::unicode::idna::punycode_status::success);
    out.resize(ptr - out.begin());
    EXPECT_EQ(out, U"MajiでKoiする5秒前");
}

TEST(PunycodeTests, BasicDecodingIteratorU8) {
    using webpp::stl::string_view;
    using webpp::stl::u8string;
    using webpp::unicode::idna::punycode_decode;
    using webpp::unicode::idna::unicode_of;
    u8string out;
    out.resize(100);
    string_view const inp = "MajiKoi5-783gue6qz075azm5e";
    auto              ptr = out.begin();
    EXPECT_EQ(punycode_decode(inp.begin(), inp.end(), ptr), webpp::unicode::idna::punycode_status::success);
    out.resize(ptr - out.begin());
    EXPECT_EQ(out, u8"MajiでKoiする5秒前");
}

TEST(PunycodeTests, IteratorTest) {
    using webpp::stl::string_view;
    using webpp::stl::u32string;
    using webpp::unicode::checked::insert_at;
    using webpp::unicode::idna::punycode_decode;
    u32string out;
    out.resize(4);
    auto const pos = out.begin();
    insert_at(pos, out.end(), 0, 'a');
    insert_at(pos, out.end(), 1, 'b');
    insert_at(pos, out.end(), 2, 'c');
    insert_at(pos, out.end(), 3, 'd');
    EXPECT_EQ(out, U"abcd");
}

TEST(PunycodeTests, IteratorTestU32) {
    using webpp::stl::string_view;
    using webpp::stl::u32string;
    using webpp::unicode::checked::insert_at;
    using webpp::unicode::idna::punycode_decode;
    u32string out;
    out.resize(4);
    auto const pos = out.begin();
    insert_at(pos, out.end(), 0, U'a');
    insert_at(pos, out.end(), 1, U'b');
    insert_at(pos, out.end(), 2, U'c');
    insert_at(pos, out.end(), 3, U'd');
    EXPECT_EQ(out, U"abcd");
}

TEST(PunycodeTests, IteratorTestU32ToU8) {
    using webpp::stl::string_view;
    using webpp::stl::u8string;
    using webpp::unicode::checked::insert_at;
    using webpp::unicode::idna::punycode_decode;
    u8string out;
    out.resize(4);
    auto const pos = out.begin();
    insert_at(pos, out.end(), 0, U'a');
    insert_at(pos, out.end(), 1, U'b');
    insert_at(pos, out.end(), 2, U'c');
    insert_at(pos, out.end(), 3, U'd');
    EXPECT_EQ(out, u8"abcd");
}

TEST(PunycodeTests, PunycodeEncodeDecode) {
    using webpp::unicode::idna::punycode_of;
    using webpp::unicode::idna::unicode_of;
    EXPECT_EQ(punycode_of("\u05D0\u0308"), "ssa73l");
    EXPECT_EQ(unicode_of("ssa73l"), "\u05D0\u0308");
}
