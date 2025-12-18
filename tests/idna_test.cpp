// Created by moisrex on Fri 2024/02/09

#include "../webpp/unicode/idna.hpp"

#include "../webpp/unicode/bidi.hpp"
#include "../webpp/unicode/general_category.hpp"
#include "../webpp/unicode/joiners.hpp"
#include "../webpp/unicode/to_ascii.hpp"
#include "../webpp/unicode/validity_criteria.hpp"
#include "../webpp/uri/uri.hpp"
#include "./common/bidi.hpp"
#include "./common/test.hpp"
#include "./webpp/std/format.hpp"

#include <filesystem>
#include <fstream>
#include <set>

// NOLINTBEGIN(*-magic-numbers, *-pro-bounds-pointer-arithmetic, *-use-designated-initializers)
using namespace webpp;

using Types =
  testing::Types<uri::parsing_uri_context_string<stl::string>,
                 uri::parsing_uri_context_string<stl::string_view>,
                 // uri::parsing_uri_context_string<stl::basic_string_view<char8_t>>,
                 uri::parsing_uri_context_u32,
                 uri::parsing_uri_context_segregated<>,
                 uri::parsing_uri_context_segregated_view<>,
                 uri::parsing_uri_context<stl::string_view, char const*>>;

template <class T>
struct IDNATests : testing::Test {
  private:
    stl::string url_text;

  public:
    template <typename SpecifiedTypeParam, typename StrT = stl::string_view>
    [[nodiscard]] constexpr SpecifiedTypeParam get_context(StrT str) {
        using ctx_type    = SpecifiedTypeParam;
        using string_type = stl::remove_cvref_t<StrT>;
        using str_iter    = typename string_type::const_iterator;
        using iterator    = typename ctx_type::iterator;
        if constexpr (stl::convertible_to<str_iter, iterator>) {
            return SpecifiedTypeParam{.beg = str.begin(), .pos = str.begin(), .end = str.end()};
        } else if constexpr (stl::convertible_to<stl::string::iterator, iterator>) {
            url_text = str;
            return SpecifiedTypeParam{.beg = url_text.begin(), .pos = url_text.begin(), .end = url_text.end()};
        } else {
            return SpecifiedTypeParam{.beg = str.data(), .pos = str.data(), .end = str.data() + str.size()};
        }
    }

    template <typename SpecifiedTypeParam>
    [[nodiscard]] constexpr SpecifiedTypeParam parse_from_string(stl::string_view const str) {
        auto ctx = get_context<SpecifiedTypeParam, stl::string_view>(str);
        uri::parse_uri(ctx);
        return ctx;
    }
};

TYPED_TEST_SUITE(IDNATests, Types);

TYPED_TEST(IDNATests, LabelSeparators) {
    // https://www.unicode.org/reports/tr46/#Notation
    // Label Separators are:
    //   - U+002E ( . ) FULL STOP
    //   - U+FF0E ( ． ) FULLWIDTH FULL STOP
    //   - U+3002 ( 。 ) IDEOGRAPHIC FULL STOP
    //   - U+FF61 ( ｡ ) HALFWIDTH IDEOGRAPHIC FULL STOP

    auto const ctx1 = this->template parse_from_string<TypeParam>("http://example.org");
    EXPECT_TRUE(uri::is_valid(ctx1.status)) << to_string(uri::get_value(ctx1.status));

    auto const ctx2 = this->template parse_from_string<TypeParam>("http://example．org");
    EXPECT_TRUE(uri::is_valid(ctx2.status)) << to_string(uri::get_value(ctx2.status));

    auto const ctx3 = this->template parse_from_string<TypeParam>("http://example。org");
    EXPECT_TRUE(uri::is_valid(ctx3.status)) << to_string(uri::get_value(ctx3.status));

    auto const ctx4 = this->template parse_from_string<TypeParam>("http://example｡org");
    EXPECT_TRUE(uri::is_valid(ctx4.status)) << to_string(uri::get_value(ctx4.status));

    EXPECT_EQ(ctx1.out.get_hostname(), "example.org");
    if constexpr (TypeParam::is_modifiable || TypeParam::is_segregated) {
        EXPECT_EQ(ctx2.out.get_hostname(), "example.org");
        EXPECT_EQ(ctx3.out.get_hostname(), "example.org");
        EXPECT_EQ(ctx4.out.get_hostname(), "example.org");
    }
}

TEST(BasicIDNATests, PerformMappingTest) {
    // 'A' should be mapped to 'a'
    std::string out;
    EXPECT_TRUE(unicode::idna::map(U'A', out));
    EXPECT_EQ(out, "a");
    out.clear();

    std::u32string out32;
    EXPECT_TRUE(unicode::idna::map(U'\x1F244', out32));
    EXPECT_EQ(out32, U"\x3014\x70B9\x3015");

    std::u8string out8;
    EXPECT_TRUE(unicode::idna::map(U'\x1F244', out8));
    EXPECT_EQ(out8, u8"\xE3\x80\x94\xE7\x82\xB9\xE3\x80\x95");
}

TEST(BasicIDNATests, UnicodeMapping) {
    // 'A' should be mapped to 'a'
    std::string out;
    EXPECT_TRUE(unicode::idna::map(U"A", out));
    EXPECT_EQ(out, "a");
    out.clear();

    std::u32string out32;
    EXPECT_TRUE(unicode::idna::map(U"\x1F244", out32));
    EXPECT_EQ(out32, U"\x3014\x70B9\x3015");

    std::u8string out8;
    EXPECT_TRUE(unicode::idna::map(U"\x1F244", out8));
    EXPECT_EQ(out8, u8"\xE3\x80\x94\xE7\x82\xB9\xE3\x80\x95");
}

TEST(BasicIDNATests, MostMappings) {
    using unicode::idna::mapped;
    using unicode::idna::status_of;
    using unicode::idna::details::disallowed;
    using unicode::idna::details::valid;

    // awk -f gen-idna-tests.awk IdnaMappingTable.txt
    // awk -f gen-idna-tests.awk IdnaMappingTable.txt | sort --random-sort | head
    EXPECT_EQ(status_of(0), valid);
    EXPECT_EQ(status_of(4), valid);
    EXPECT_EQ(status_of(8), valid);
    EXPECT_EQ(status_of(12), valid);
    EXPECT_EQ(status_of(16), valid);
    EXPECT_EQ(status_of(20), valid);
    EXPECT_EQ(status_of(24), valid);
    EXPECT_EQ(status_of(28), valid);
    EXPECT_EQ(status_of(32), valid);
    EXPECT_EQ(status_of(36), valid);

    EXPECT_EQ(status_of(128), disallowed);
    EXPECT_EQ(status_of(131), disallowed);
    EXPECT_EQ(status_of(134), disallowed);
    EXPECT_EQ(status_of(137), disallowed);
    EXPECT_EQ(status_of(140), disallowed);
    EXPECT_EQ(status_of(143), disallowed);
    EXPECT_EQ(status_of(146), disallowed);
    EXPECT_EQ(status_of(149), disallowed);
    EXPECT_EQ(status_of(152), disallowed);
    EXPECT_EQ(status_of(155), disallowed);

    EXPECT_EQ(mapped<std::u32string>(319), U"\x006C\x00B7"); // mapped
    EXPECT_EQ(mapped<std::u32string>(320), U"\x006C\x00B7"); // mapped
    EXPECT_EQ(status_of(446), valid);
    EXPECT_EQ(status_of(447), valid);
    EXPECT_EQ(status_of(450), valid);
    EXPECT_EQ(status_of(575), valid);
    EXPECT_EQ(status_of(576), valid);

    EXPECT_EQ(status_of(1480), disallowed);
    EXPECT_EQ(status_of(1481), disallowed);
    EXPECT_EQ(status_of(1482), disallowed);
    EXPECT_EQ(status_of(1483), disallowed);
    EXPECT_EQ(status_of(1484), disallowed);
    EXPECT_EQ(status_of(1485), disallowed);
    EXPECT_EQ(status_of(1486), disallowed);
    EXPECT_EQ(status_of(1487), disallowed);
    EXPECT_EQ(status_of(1488), valid);
    EXPECT_EQ(status_of(1490), valid);
    EXPECT_EQ(status_of(1492), valid);
    EXPECT_EQ(status_of(1494), valid);

    EXPECT_EQ(status_of(1512), valid);
    EXPECT_EQ(status_of(1514), valid);
    EXPECT_EQ(status_of(1515), disallowed);
    EXPECT_EQ(status_of(1516), disallowed);
    EXPECT_EQ(status_of(1517), disallowed);
    EXPECT_EQ(status_of(1518), disallowed);
    EXPECT_EQ(status_of(1519), valid);
    EXPECT_EQ(status_of(1520), valid);

    EXPECT_EQ(status_of(205'744), valid);                 // Unicode 17.0 made it valid "323B0"
    EXPECT_EQ(status_of(205'742), valid);

    EXPECT_EQ(mapped<std::u32string>(173), U"");          // ignored
    EXPECT_EQ(mapped<std::u32string>(847), U"");          // ignored
    EXPECT_EQ(mapped<std::u32string>(4447), U"");         // ignored
    EXPECT_EQ(mapped<std::u32string>(4448), U"");         // ignored
    EXPECT_EQ(mapped<std::u32string>(6068), U"");         // ignored
    EXPECT_EQ(mapped<std::u32string>(6069), U"");         // ignored
    EXPECT_EQ(mapped<std::u32string>(6155), U"");         // ignored
    EXPECT_EQ(mapped<std::u32string>(6156), U"");         // ignored
    EXPECT_EQ(mapped<std::u32string>(6157), U"");         // ignored
    EXPECT_EQ(mapped<std::u32string>(6158), U"");         // ignored
    EXPECT_EQ(mapped<std::u32string>(6159), U"");         // ignored
    EXPECT_EQ(mapped<std::u32string>(8203), U"");         // ignored
    EXPECT_EQ(mapped<std::u32string>(8288), U"");         // ignored
    EXPECT_EQ(mapped<std::u32string>(8289), U"");         // ignored
    EXPECT_EQ(mapped<std::u32string>(8290), U"");         // ignored
    EXPECT_EQ(mapped<std::u32string>(8291), U"");         // ignored
    EXPECT_EQ(mapped<std::u32string>(8292), U"");         // ignored
    EXPECT_EQ(mapped<std::u32string>(8298), U"");         // ignored
    EXPECT_EQ(mapped<std::u32string>(8299), U"");         // ignored
    EXPECT_EQ(mapped<std::u32string>(8300), U"");         // ignored
    EXPECT_EQ(mapped<std::u32string>(8301), U"");         // ignored
    EXPECT_EQ(mapped<std::u32string>(8302), U"");         // ignored
    EXPECT_EQ(mapped<std::u32string>(8303), U"");         // ignored
    EXPECT_EQ(mapped<std::u32string>(12'644), U"");       // ignored
    EXPECT_EQ(mapped<std::u32string>(65'024), U"");       // ignored
    EXPECT_EQ(mapped<std::u32string>(65'025), U"");       // ignored
    EXPECT_EQ(mapped<std::u32string>(65'026), U"");       // ignored
    EXPECT_EQ(mapped<std::u32string>(65'027), U"");       // ignored
    EXPECT_EQ(mapped<std::u32string>(65'028), U"");       // ignored
    EXPECT_EQ(mapped<std::u32string>(65'029), U"");       // ignored
    EXPECT_EQ(mapped<std::u32string>(65'030), U"");       // ignored
    EXPECT_EQ(mapped<std::u32string>(65'031), U"");       // ignored
    EXPECT_EQ(mapped<std::u32string>(65'032), U"");       // ignored
    EXPECT_EQ(mapped<std::u32string>(65'033), U"");       // ignored
    EXPECT_EQ(mapped<std::u32string>(65'034), U"");       // ignored
    EXPECT_EQ(mapped<std::u32string>(65'035), U"");       // ignored
    EXPECT_EQ(mapped<std::u32string>(65'036), U"");       // ignored
    EXPECT_EQ(mapped<std::u32string>(65'037), U"");       // ignored
    EXPECT_EQ(mapped<std::u32string>(65'038), U"");       // ignored
    EXPECT_EQ(mapped<std::u32string>(65'039), U"");       // ignored
    EXPECT_EQ(mapped<std::u32string>(65'279), U"");       // ignored
    EXPECT_EQ(mapped<std::u32string>(65'440), U"");       // ignored
    EXPECT_EQ(mapped<std::u32string>(113'824), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(113'825), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(113'826), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(113'827), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(119'155), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(119'156), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(119'157), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(119'158), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(119'159), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(119'160), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(119'161), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(119'162), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(917'760), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(917'783), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(917'806), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(917'829), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(917'852), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(917'875), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(917'898), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(917'921), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(917'944), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(917'967), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(917'990), U"");      // ignored

    EXPECT_EQ(mapped<std::u32string>(917'925), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(917'926), U"");      // ignored
    EXPECT_EQ(mapped<std::u32string>(917'925 + 48), U""); // ignored

    EXPECT_EQ(status_of(223), valid);                     // deviation
    EXPECT_EQ(status_of(962), valid);                     // deviation
    EXPECT_EQ(status_of(8204), valid);                    // deviation
    EXPECT_EQ(status_of(8205), valid);                    // deviation

    // awk -f gen-idna-tests.awk IdnaMappingTable.txt | grep map -B 3 --no-group-separator | head
    EXPECT_EQ(status_of(62), valid);
    EXPECT_EQ(status_of(63), valid);
    EXPECT_EQ(status_of(64), valid);
    EXPECT_EQ(mapped<std::u32string>(65), U"\x0061"); // mapped
    EXPECT_EQ(mapped<std::u32string>(66), U"\x0062"); // mapped
    EXPECT_EQ(mapped<std::u32string>(67), U"\x0063"); // mapped
    EXPECT_EQ(mapped<std::u32string>(68), U"\x0064"); // mapped
    EXPECT_EQ(mapped<std::u32string>(69), U"\x0065"); // mapped
    EXPECT_EQ(mapped<std::u32string>(70), U"\x0066"); // mapped
    EXPECT_EQ(mapped<std::u32string>(71), U"\x0067"); // mapped


    EXPECT_EQ(status_of(67'454), disallowed);
    EXPECT_EQ(status_of(119'751), disallowed);
    EXPECT_EQ(status_of(69'594), disallowed);
    EXPECT_EQ(status_of(72'361), disallowed);
    EXPECT_EQ(status_of(40'892), valid);
    EXPECT_EQ(status_of(71'812), disallowed);
    EXPECT_EQ(status_of(4021), valid);
    EXPECT_EQ(mapped<std::u32string>(8315), U"\x2212");               // mapped
    EXPECT_EQ(status_of(11'676), disallowed);
    EXPECT_EQ(status_of(6851), valid);
    EXPECT_EQ(mapped<std::u32string>(404), U"\x0263");                // mapped
    EXPECT_EQ(status_of(43'130), disallowed);
    EXPECT_EQ(status_of(70'046), valid);
    EXPECT_EQ(mapped<std::u32string>(63'972), U"\x7406");             // mapped
    EXPECT_EQ(mapped<std::u32string>(7586), U"\x0261");               // mapped
    EXPECT_EQ(status_of(2974), valid);
    EXPECT_EQ(status_of(64'221), disallowed);
    EXPECT_EQ(mapped<std::u32string>(120'540), U"\x03B5");            // mapped
    EXPECT_EQ(status_of(68'926), disallowed);
    EXPECT_EQ(mapped<std::u32string>(12'808), U"\x0028\x110C\x0029"); // mapped
    EXPECT_EQ(status_of(43'230), disallowed);
    EXPECT_EQ(mapped<std::u32string>(7690), U"\x1E0B");               // mapped
    EXPECT_EQ(mapped<std::u32string>(12'089), U"\x5F50");             // mapped
    EXPECT_EQ(status_of(68'942), valid);
    EXPECT_EQ(status_of(3400), valid);
    EXPECT_EQ(status_of(11'247), valid);
    EXPECT_EQ(status_of(1569), valid);
    EXPECT_EQ(mapped<std::u32string>(12'644), U""); // ignored
    EXPECT_EQ(status_of(66'289), valid);

    // 11B60..11B67  ; valid                      # 17.0 SHARADA VOWEL SIGN OE..SHARADA VOWEL SIGN CANDRA O
    EXPECT_EQ(status_of(72'548), valid);                   // 11B64; Unicode 17.0 made it valid

    EXPECT_EQ(status_of(127'128), disallowed);
    EXPECT_EQ(mapped<std::u32string>(12'614), U"\x110A");  // mapped
    EXPECT_EQ(mapped<std::u32string>(42'824), U"\xA749");  // mapped
    EXPECT_EQ(status_of(11'944), valid);
    EXPECT_EQ(mapped<std::u32string>(127'538), U"\x7981"); // mapped
    EXPECT_EQ(status_of(129'205), valid);
    EXPECT_EQ(status_of(67'386), disallowed);
    EXPECT_EQ(status_of(6903), disallowed);
    EXPECT_EQ(status_of(185'478), valid);
    EXPECT_EQ(status_of(2950), valid);
    EXPECT_EQ(status_of(7707), valid);
    EXPECT_EQ(status_of(4814), valid);
    EXPECT_EQ(status_of(66'008), valid);
    EXPECT_EQ(status_of(125'274), disallowed);
    EXPECT_EQ(status_of(2228), valid);
    EXPECT_EQ(status_of(71'892), valid);
    EXPECT_EQ(status_of(9932), valid);
    EXPECT_EQ(status_of(73'902), valid);
    EXPECT_EQ(status_of(123'211), disallowed);
    EXPECT_EQ(status_of(481), valid);
}

TEST(BasicIDNATests, BidiMost) {
    using webpp::unicode::direction;
    using webpp::unicode::direction_of;

    EXPECT_EQ(direction_of(U'\x0041'), direction::L);
    EXPECT_EQ(direction_of(U'\x0600'), direction::AN);
    EXPECT_EQ(direction_of(U'\x0610'), direction::NSM);

    // 1734;HANUNOO SIGN PAMUDPOD;Mc;9;L;;;;;N;;;;;
    EXPECT_EQ(direction_of(U'\x1734'), direction::L);

    // 10101;AEGEAN WORD SEPARATOR DOT;Po;0;ON;;;;;N;;;;;
    EXPECT_EQ(direction_of(U'\x10101'), direction::ON);

    // 1171E;AHOM CONSONANT SIGN MEDIAL RA;Mc;0;L;;;;;N;;;;;
    EXPECT_EQ(direction_of(U'\x1171E'), direction::L);

    // 1D6C1;MATHEMATICAL BOLD NABLA;Sm;0;ON;<font> 2207;;;;N;;;;;
    EXPECT_EQ(direction_of(U'\x1D6C1'), direction::ON);

    // 1D6FB;MATHEMATICAL ITALIC NABLA;Sm;0;ON;<font> 2207;;;;N;;;;;
    EXPECT_EQ(direction_of(U'\x1D6FB'), direction::ON);

    // 1D735;MATHEMATICAL BOLD ITALIC NABLA;Sm;0;ON;<font> 2207;;;;N;;;;;
    EXPECT_EQ(direction_of(U'\x1D735'), direction::ON);

    // 1D76F;MATHEMATICAL SANS-SERIF BOLD NABLA;Sm;0;ON;<font> 2207;;;;N;;;;;
    EXPECT_EQ(direction_of(U'\x1D76F'), direction::ON);

    // 1D7A9;MATHEMATICAL SANS-SERIF BOLD ITALIC NABLA;Sm;0;ON;<font> 2207;;;;N;;;;;
    EXPECT_EQ(direction_of(U'\x1D7A9'), direction::ON);

    // 1F4A9;PILE OF POO;So;0;ON;;;;;N;;;;;
    EXPECT_EQ(direction_of(U'\x1F4A9'), direction::ON);

    // 061D;ARABIC END OF TEXT MARK;Po;0;AL;;;;;N;;;;;
    EXPECT_EQ(direction_of(U'\x61D'), direction::AL);

    // E01EF;VARIATION SELECTOR-256;Mn;0;NSM;;;;;N;;;;;
    EXPECT_EQ(direction_of(U'\xE01EF'), direction::NSM);

    for (char32_t cp = 0; cp < 0x10'fffdU + 10; cp += 1) {
        auto const lhs = direction_of(cp);
        auto const rhs = webpp::unicode::tests::find_direction(cp);

        // if these fail, that might be because the test table might be a few versions behind;
        // that's why we're ignoring some of the tests here:
        if (lhs != rhs && rhs == direction::NONE) {
            continue;
        }

        switch (cp) {
            case 0x1734:
            case 0x1'171E:
            case 0x1'D6C1:
            case 0x1'D6FB:
            case 0x1'D735:
            case 0x1'D76F:
            case 0x1'D7A9: continue;
            default: break;
        }

        EXPECT_EQ(lhs, rhs) << "Code Point: " << static_cast<int>(cp) << "\n    " << to_abbr(lhs) << " ("
                            << to_string(lhs) << ")  !==  " << to_abbr(rhs) << " (" << to_string(rhs) << ")";
    }
}

TEST(BasicIDNATests, BidiRules) {
    using webpp::stl::u32string_view;
    using webpp::unicode::validate_bidi_rule;

    // https://www.rfc-editor.org/rfc/rfc5893#section-4.1
    u32string_view const computer_word =
      U"\u0786"  // THAANA LETTER KAAFU (AL)
      U"\u07AE"  // THAANA OBOFILI (NSM)
      U"\u0782"  // THAANA LETTER NOONU (AL)
      U"\u07B0"  // THAANA SUKUN (NSM)
      U"\u0795"  // THAANA LETTER PAVIYANI (AL)
      U"\u07A9"  // THAANA LETTER EEBEEFILI (AL)
      U"\u0793"  // THAANA LETTER TAVIYANI (AL)
      U"\u07A6"  // THAANA ABAFILI (NSM)
      U"\u0783"  // THAANA LETTER RAA (AL)
      U"\u07AA"; // THAANA UBUFILI (NSM)

    // https://www.rfc-editor.org/rfc/rfc5893#section-4.2
    u32string_view const yivo_acronym =
      U"\u05D9"  // HEBREW LETTER YOD (R)
      U"\u05B4"  // HEBREW POINT HIRIQ (NSM)
      U"\u05D5"  // HEBREW LETTER VAV (R)
      U"\u05D0"  // HEBREW LETTER ALEF (R)
      U"\u05B8"; // HEBREW POINT QAMATS (NSM)


    EXPECT_TRUE(validate_bidi_rule(computer_word.begin(), computer_word.end()));
    EXPECT_TRUE(validate_bidi_rule(yivo_acronym.begin(), yivo_acronym.end()));


    // 3.  In an RTL label, the end of the label must be a character with
    //     Bidi property R, AL, EN, or AN, followed by zero or more
    //     characters with Bidi property NSM.
    u32string_view const invalid_computer_word =
      U"\u0786" // THAANA LETTER KAAFU (AL)
      U"\u07AE" // THAANA OBOFILI (NSM)
      U"\u0782" // THAANA LETTER NOONU (AL)
      U"\u07B0" // THAANA SUKUN (NSM)
      U"\u0795" // THAANA LETTER PAVIYANI (AL)
      U"\u07A9" // THAANA LETTER EEBEEFILI (AL)
      U"\u0793" // THAANA LETTER TAVIYANI (AL)
      U"\u07A6" // THAANA ABAFILI (NSM)
      U"\u0783" // THAANA LETTER RAA (AL)
      U"\u07AA" // THAANA UBUFILI (NSM)
      U"\u0294" // 0294          ; L # Lo       LATIN LETTER GLOTTAL STOP
      U"\u07AA" // THAANA UBUFILI (NSM)
      ;

    EXPECT_FALSE(validate_bidi_rule(invalid_computer_word.begin(), invalid_computer_word.end()));


    u32string_view const invalid_yivo_acronym =
      U"\u05D9"  // HEBREW LETTER YOD (R)
      U"\u05B4"  // HEBREW POINT HIRIQ (NSM)
      U"\u05D5"  // HEBREW LETTER VAV (R)
      U"\u05D0"  // HEBREW LETTER ALEF (R)
      U"\u0378"  // Should be (L), but this makes it invalid
      U"\u05B8"; // HEBREW POINT QAMATS (NSM)

    EXPECT_FALSE(validate_bidi_rule(invalid_yivo_acronym.begin(), invalid_yivo_acronym.end()));

    u32string_view const empty{};
    EXPECT_TRUE(validate_bidi_rule(empty.begin(), empty.end()));
}

TEST(BasicIDNATests, JoinerTypesTest) {
    using webpp::unicode::joiner_type;
    using webpp::unicode::joiner_type_of;

    EXPECT_EQ(joiner_type_of(U'\x0'), joiner_type::non_joining);
    EXPECT_EQ(joiner_type_of(U'\x1'), joiner_type::non_joining);

    // Generated using:
    // ./gen-joiners-tests.awk DerivedJoiningType.txt | grep -v transparent | grep -v join_causing | sort
    //     --random-sort | head -n 50
    EXPECT_EQ(joiner_type_of(U'\x84E'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x8A8'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x6BD'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x10AD9'), joiner_type::dual_joining);  // D
    EXPECT_EQ(joiner_type_of(U'\x1E92F'), joiner_type::dual_joining);  // D
    EXPECT_EQ(joiner_type_of(U'\x67C'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x8BC'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x10F33'), joiner_type::right_joining); // R
    EXPECT_EQ(joiner_type_of(U'\x634'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x1E921'), joiner_type::dual_joining);  // D
    EXPECT_EQ(joiner_type_of(U'\x0627'), joiner_type::right_joining);  // R
    EXPECT_EQ(joiner_type_of(U'\x67D'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x1E905'), joiner_type::dual_joining);  // D
    EXPECT_EQ(joiner_type_of(U'\x10F76'), joiner_type::dual_joining);  // D
    EXPECT_EQ(joiner_type_of(U'\xA858'), joiner_type::dual_joining);   // D
    EXPECT_EQ(joiner_type_of(U'\x188A'), joiner_type::dual_joining);   // D
    EXPECT_EQ(joiner_type_of(U'\x758'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x62A'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\xA85A'), joiner_type::dual_joining);   // D
    EXPECT_EQ(joiner_type_of(U'\x06C0'), joiner_type::right_joining);  // R
    EXPECT_EQ(joiner_type_of(U'\x872'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x696'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x693'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x877'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\xA84A'), joiner_type::dual_joining);   // D
    EXPECT_EQ(joiner_type_of(U'\x699'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x76E'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\xA84F'), joiner_type::dual_joining);   // D
    EXPECT_EQ(joiner_type_of(U'\x10FBB'), joiner_type::dual_joining);  // D
    EXPECT_EQ(joiner_type_of(U'\x10F73'), joiner_type::dual_joining);  // D
    EXPECT_EQ(joiner_type_of(U'\x841'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x871'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x768'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x87B'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x10EC3'), joiner_type::dual_joining);  // D
    EXPECT_EQ(joiner_type_of(U'\x7D8'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x842'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\xA856'), joiner_type::dual_joining);   // D
    EXPECT_EQ(joiner_type_of(U'\x10BAB'), joiner_type::right_joining); // R
    EXPECT_EQ(joiner_type_of(U'\x7CE'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x6B8'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x720'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x63C'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x10FB5'), joiner_type::right_joining); // R
    EXPECT_EQ(joiner_type_of(U'\x1E927'), joiner_type::dual_joining);  // D
    EXPECT_EQ(joiner_type_of(U'\x775'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x7DA'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x8A9'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x184A'), joiner_type::dual_joining);   // D
    EXPECT_EQ(joiner_type_of(U'\x183F'), joiner_type::dual_joining);   // D

    EXPECT_EQ(joiner_type_of(U'\x11180'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x1CF1A'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x10FB2'), joiner_type::dual_joining);  // D
    EXPECT_EQ(joiner_type_of(U'\x1E016'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x2DE1'), joiner_type::transparent);    // T
    EXPECT_EQ(joiner_type_of(U'\x10F4D'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x76B'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x81C'), joiner_type::transparent);     // T
    EXPECT_EQ(joiner_type_of(U'\xF9C'), joiner_type::transparent);     // T
    EXPECT_EQ(joiner_type_of(U'\x186D'), joiner_type::dual_joining);   // D
    EXPECT_EQ(joiner_type_of(U'\x13452'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x113BC'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\xFAF'), joiner_type::transparent);     // T
    EXPECT_EQ(joiner_type_of(U'\x1928'), joiner_type::transparent);    // T
    EXPECT_EQ(joiner_type_of(U'\xE017F'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\xE011E'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\xE0137'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\xE4E'), joiner_type::transparent);     // T
    EXPECT_EQ(joiner_type_of(U'\xE01C9'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x1BC9E'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\xE01D5'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\xE0026'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x1DA05'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x719'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x11C38'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x885'), joiner_type::join_causing);    // C
    EXPECT_EQ(joiner_type_of(U'\x10B80'), joiner_type::dual_joining);  // D
    EXPECT_EQ(joiner_type_of(U'\x1DA51'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\xA948'), joiner_type::transparent);    // T
    EXPECT_EQ(joiner_type_of(U'\xEBA'), joiner_type::transparent);     // T
    EXPECT_EQ(joiner_type_of(U'\x1DA54'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x8AF'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x77D'), joiner_type::dual_joining);    // D
    EXPECT_EQ(joiner_type_of(U'\x1DA0C'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x1E026'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x1A77'), joiner_type::transparent);    // T
    EXPECT_EQ(joiner_type_of(U'\x1AC7'), joiner_type::transparent);    // T
    EXPECT_EQ(joiner_type_of(U'\x07FA'), joiner_type::join_causing);   // C
    EXPECT_EQ(joiner_type_of(U'\x1103F'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x1920'), joiner_type::transparent);    // T
    EXPECT_EQ(joiner_type_of(U'\x331'), joiner_type::transparent);     // T
    EXPECT_EQ(joiner_type_of(U'\x11102'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x11C94'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x1852'), joiner_type::dual_joining);   // D
    EXPECT_EQ(joiner_type_of(U'\x0DD6'), joiner_type::transparent);    // T
    EXPECT_EQ(joiner_type_of(U'\x1DA55'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\x81D'), joiner_type::transparent);     // T
    EXPECT_EQ(joiner_type_of(U'\xAE3'), joiner_type::transparent);     // T
    EXPECT_EQ(joiner_type_of(U'\xE0174'), joiner_type::transparent);   // T
    EXPECT_EQ(joiner_type_of(U'\xA85F'), joiner_type::dual_joining);   // D

    EXPECT_EQ(joiner_type_of(U'\x10ACD'), joiner_type::left_joining);  // L
    EXPECT_EQ(joiner_type_of(U'\x10D00'), joiner_type::left_joining);  // L
    EXPECT_EQ(joiner_type_of(U'\xA872'), joiner_type::left_joining);   // L
    EXPECT_EQ(joiner_type_of(U'\x10FCB'), joiner_type::left_joining);  // L
    EXPECT_EQ(joiner_type_of(U'\x10AD7'), joiner_type::left_joining);  // L

    EXPECT_EQ(joiner_type_of(U'\x874'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x0854'), joiner_type::right_joining);  // R
    EXPECT_EQ(joiner_type_of(U'\x10ADD'), joiner_type::right_joining); // R
    EXPECT_EQ(joiner_type_of(U'\x87A'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x0771'), joiner_type::right_joining);  // R
    EXPECT_EQ(joiner_type_of(U'\x672'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x10B89'), joiner_type::right_joining); // R
    EXPECT_EQ(joiner_type_of(U'\x8B2'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x10BAB'), joiner_type::right_joining); // R
    EXPECT_EQ(joiner_type_of(U'\x10FC3'), joiner_type::right_joining); // R
    EXPECT_EQ(joiner_type_of(U'\x632'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x676'), joiner_type::right_joining);   // R
    EXPECT_EQ(joiner_type_of(U'\x072F'), joiner_type::right_joining);  // R

    EXPECT_EQ(joiner_type_of(U'\x200D'), joiner_type::join_causing);   // C
    EXPECT_EQ(joiner_type_of(U'\x885'), joiner_type::join_causing);    // C
    EXPECT_EQ(joiner_type_of(U'\x883'), joiner_type::join_causing);    // C
    EXPECT_EQ(joiner_type_of(U'\x884'), joiner_type::join_causing);    // C
    EXPECT_EQ(joiner_type_of(U'\x07FA'), joiner_type::join_causing);   // C
    EXPECT_EQ(joiner_type_of(U'\x180A'), joiner_type::join_causing);   // C
    EXPECT_EQ(joiner_type_of(U'\x0640'), joiner_type::join_causing);   // C
}

TEST(BasicIDNATests, GeneralCategoryTest) {
    using webpp::unicode::general_category;
    using webpp::unicode::general_category_of;
    using webpp::unicode::is_general_category_of;


    EXPECT_TRUE(is_general_category_of(U'\x112E6', general_category::M));
    EXPECT_TRUE(is_general_category_of(U'\x300', general_category::M));

    // cat UnicodeData.txt | awk 'BEGIN{FS=";"} { if ($3 == "Mn" || $3 == "Me" || $3 == "Mc") print
    // "EXPECT_EQ(general_category_of(U\'\\\x" $1 "\'), general_category::" $3 "); // " $2; }' | sort
    // --random-sort | head -n 20
    EXPECT_EQ(general_category_of(U'\x05AC'), general_category::Mn);  // HEBREW ACCENT ILUY
    EXPECT_EQ(general_category_of(U'\x05A8'), general_category::Mn);  // HEBREW ACCENT QADMA
    EXPECT_EQ(general_category_of(U'\x1A5A'), general_category::Mn);  // TAI THAM CONSONANT SIGN LOW PA
    EXPECT_EQ(general_category_of(U'\x17D1'), general_category::Mn);  // KHMER SIGN VIRIAM
    EXPECT_EQ(general_category_of(U'\x1DA5B'), general_category::Mn); // SIGNWRITING TONGUE TIP BETWEEN LIPS
    EXPECT_EQ(general_category_of(U'\x1DFC'), general_category::Mn);  // COMBINING DOUBLE INVERTED BREVE BELOW
    EXPECT_EQ(general_category_of(U'\xA8E6'), general_category::Mn);  // COMBINING DEVANAGARI DIGIT SIX
    EXPECT_EQ(general_category_of(U'\xE0178'), general_category::Mn); // VARIATION SELECTOR-137
    EXPECT_EQ(general_category_of(U'\x16F51'), general_category::Mc); // MIAO SIGN ASPIRATION
    EXPECT_EQ(general_category_of(U'\x0B41'), general_category::Mn);  // ORIYA VOWEL SIGN U
    EXPECT_EQ(general_category_of(U'\xE01BA'), general_category::Mn); // VARIATION SELECTOR-203
    EXPECT_EQ(general_category_of(U'\x05B2'), general_category::Mn);  // HEBREW POINT HATAF PATAH
    EXPECT_EQ(general_category_of(U'\x031D'), general_category::Mn);  // COMBINING UP TACK BELOW
    EXPECT_EQ(general_category_of(U'\x1A58'), general_category::Mn);  // TAI THAM SIGN MAI KANG LAI
    EXPECT_EQ(general_category_of(U'\xE017B'), general_category::Mn); // VARIATION SELECTOR-140
    EXPECT_EQ(general_category_of(U'\x112E6'), general_category::Mn); // KHUDAWADI VOWEL SIGN AI
    EXPECT_EQ(general_category_of(U'\x036F'), general_category::Mn);  // COMBINING LATIN SMALL LETTER X
    EXPECT_EQ(general_category_of(U'\x1182F'), general_category::Mn); // DOGRA VOWEL SIGN U
    EXPECT_EQ(general_category_of(U'\xE0114'), general_category::Mn); // VARIATION SELECTOR-37
    EXPECT_EQ(general_category_of(U'\x11233'), general_category::Mc); // KHOJKI VOWEL SIGN AU

    EXPECT_EQ(general_category_of(U'\x0300'), general_category::Mn);  // COMBINING GRAVE ACCENT
    EXPECT_EQ(general_category_of(U'\x0301'), general_category::Mn);  // COMBINING ACUTE ACCENT
    EXPECT_EQ(general_category_of(U'\x0302'), general_category::Mn);  // COMBINING CIRCUMFLEX ACCENT
    EXPECT_EQ(general_category_of(U'\x0303'), general_category::Mn);  // COMBINING TILDE
    EXPECT_EQ(general_category_of(U'\x0304'), general_category::Mn);  // COMBINING MACRON
    EXPECT_EQ(general_category_of(U'\x0305'), general_category::Mn);  // COMBINING OVERLINE
    EXPECT_EQ(general_category_of(U'\x0306'), general_category::Mn);  // COMBINING BREVE
    EXPECT_EQ(general_category_of(U'\x0307'), general_category::Mn);  // COMBINING DOT ABOVE
    EXPECT_EQ(general_category_of(U'\x0308'), general_category::Mn);  // COMBINING DIAERESIS
    EXPECT_EQ(general_category_of(U'\x0309'), general_category::Mn);  // COMBINING HOOK ABOVE
    EXPECT_EQ(general_category_of(U'\x030A'), general_category::Mn);  // COMBINING RING ABOVE
    EXPECT_EQ(general_category_of(U'\x030B'), general_category::Mn);  // COMBINING DOUBLE ACUTE ACCENT
    EXPECT_EQ(general_category_of(U'\x030C'), general_category::Mn);  // COMBINING CARON
    EXPECT_EQ(general_category_of(U'\x030D'), general_category::Mn);  // COMBINING VERTICAL LINE ABOVE
    EXPECT_EQ(general_category_of(U'\x030E'), general_category::Mn);  // COMBINING DOUBLE VERTICAL LINE ABOVE
    EXPECT_EQ(general_category_of(U'\x030F'), general_category::Mn);  // COMBINING DOUBLE GRAVE ACCENT
    EXPECT_EQ(general_category_of(U'\x0310'), general_category::Mn);  // COMBINING CANDRABINDU
    EXPECT_EQ(general_category_of(U'\x0311'), general_category::Mn);  // COMBINING INVERTED BREVE
    EXPECT_EQ(general_category_of(U'\x0312'), general_category::Mn);  // COMBINING TURNED COMMA ABOVE
    EXPECT_EQ(general_category_of(U'\x0313'), general_category::Mn);  // COMBINING COMMA ABOVE
}

TEST(BasicIDNATests, IDNALabelSeparator) {
    using webpp::unicode::idna::mapped;
    // https://www.unicode.org/reports/tr46/#Notation
    // Label Separators are:
    //   - U+002E ( . ) FULL STOP
    //   - U+FF0E ( ． ) FULLWIDTH FULL STOP
    //   - U+3002 ( 。 ) IDEOGRAPHIC FULL STOP
    //   - U+FF61 ( ｡ ) HALFWIDTH IDEOGRAPHIC FULL STOP

    EXPECT_EQ(mapped(U'\x002E'), u8"\x002E");
    EXPECT_EQ(mapped(U'\xFF0E'), u8"\x002E");
    EXPECT_EQ(mapped(U'\x3002'), u8"\x002E");
    EXPECT_EQ(mapped(U'\xFF61'), u8"\x002E");
}

TEST(BasicIDNATests, CheckValidiyCriteria) {
    using webpp::stl::array;
    using webpp::stl::string;
    using webpp::stl::string_view;
    using webpp::unicode::idna::idna_options;
    using webpp::unicode::idna::is_label_valid;

    struct opts {
        string_view str;
        bool        is_valid  = true;
        int         opt_index = 0;
    };

    static constexpr array<idna_options, 3> idna_opts{
      {
       idna_options{},
       unicode::idna::strict_idna_options,
       unicode::idna::loose_idna_options,
       }
    };

    static constexpr array<opts, 27> tests{
      opts{"", true, -1},
      {"a", true, -1},
      {"-", true, 0},
      {"--", true, 0},
      {"---", true, 0},
      {"---", true, 2},
      {"-", true, 2},
      {"xn---", false},
      {"nn---"},
      {"nn---", false, 1},
      {"---", false, 1},
      {"--", false, 1},
      {"-", false, 1},
      {"correct", true, -1},
      {"a--b", false, 1},
      {"a--b", true, 0},
      {"שלום.1a", false, 1},
      {"שלום.1a", true, 2}, // LDH-only label cannot come after RTL label

      // Basic ASCII & LDH (Letter-Digit-Hyphen)
      {"1", true, 2}, // Single digit
      {"1", false, 0},
      {"1", false, 1},
      {"a1", true, -1}, // Letter followed by digit
      {"1a", true, -1}, // Digit followed by letter
      {"a-b", true, -1}, // Hyphen in middle

      {"ABC", true, 0}, // STD3 Rule
      {"ABC", false, 1}, // STD3 Rule

      {"تست", true, -1}
    };

    for (auto const [str, is_valid, opts_index] : tests) {
        switch (opts_index) {
            case -1:
                EXPECT_EQ(is_valid, is_label_valid<idna_opts.at(0)>(str.begin(), str.end())) << str;
                EXPECT_EQ(is_valid, is_label_valid<idna_opts.at(1)>(str.begin(), str.end())) << str;
                EXPECT_EQ(is_valid, is_label_valid<idna_opts.at(2)>(str.begin(), str.end())) << str;
                break;
            case 0: EXPECT_EQ(is_valid, is_label_valid<idna_opts.at(0)>(str.begin(), str.end())) << str; break;
            case 1: EXPECT_EQ(is_valid, is_label_valid<idna_opts.at(1)>(str.begin(), str.end())) << str; break;
            case 2: EXPECT_EQ(is_valid, is_label_valid<idna_opts.at(2)>(str.begin(), str.end())) << str; break;
            default: break;
        }
    }
}

TEST(BasicIDNATests, ToASCIITest) {
    using webpp::unicode::idna::to_ascii;
    using enum webpp::unicode::idna::to_ascii_status;
    using webpp::stl::array;
    using webpp::stl::string;
    using webpp::stl::string_view;

    static constexpr array<string_view, 11> invalids{
      "xn--",
      "xn--zn7c.com",
      "xn--a-yoc",
      "a.b.c.xn--pokxncvks",
      "xn--a",
      "xn--a.xn--zca",
      "xn--a.ß",
      "xn--ls8h=",
      "xn--tešla",
      "يa",
      "xn--zn7c.com",
    };

    static constexpr array<array<string_view, 2>, 29> valids{
      array<string_view, 2>{                                                               "straße.de","xn--strae-oqa.de"                                                                                                       },
      {                                                    "2001:4860:4860::8888", "2001:4860:4860::8888"},
      {                                                         "نامه‌ای",  "xn--mgba3gch31f060k"},
      {                                                         "xn--zca.xn--zca",      "xn--zca.xn--zca"},
      {                                                           "ab--c.xn--zca",        "ab--c.xn--zca"},
      {                                                              "x..xn--zca",           "x..xn--zca"},
      {                                                              "x-.xn--zca",           "x-.xn--zca"},
      {                                                              "-x.xn--zca",           "-x.xn--zca"},
      {                                                               "xn--zca.ß",      "xn--zca.xn--zca"},
      {                                                                 "ab--c.ß",        "ab--c.xn--zca"},
      {                                                                  "AMAZON",               "amazon"},
      {                                                                   "ab--c",                "ab--c"},
      {                                                                    "x..ß",           "x..xn--zca"},
      {                                                                    "x-.ß",           "x-.xn--zca"},
      {                                                                    "a†--",         "xn--a---kp0a"},
      {                                                                    "aa--",                 "aa--"},
      {                                                                    "-x.ß",           "-x.xn--zca"},
      {                                                                      "-†",            "xn----xhn"},
      {                                                                      "-x",                   "-x"},
      {        "x01234567890123456789012345678901234567890123456789012345678901x",
                            "x01234567890123456789012345678901234567890123456789012345678901x"                                },
      {        "x01234567890123456789012345678901234567890123456789012345678901†",
                            "xn--x01234567890123456789012345678901234567890123456789012345678901-6963b"                       },
      {"x01234567890123456789012345678901234567890123456789012345678901x.xn--zca",
                            "x01234567890123456789012345678901234567890123456789012345678901x.xn--zca"                        },
      {      "x01234567890123456789012345678901234567890123456789012345678901x.ß",
                            "x01234567890123456789012345678901234567890123456789012345678901x.xn--zca"                        },
      {                                                                "01234567890123456789012345678901234567890123456789."
                                                                "01234567890123456789012345678901234567890123456789."
                                                                "01234567890123456789012345678901234567890123456789."
                                                                "01234567890123456789012345678901234567890123456789.0123456789012345678901234567890123456789012345678."
                                                                "x",              "01234567890123456789012345678901234567890123456789."
              "01234567890123456789012345678901234567890123456789."
              "01234567890123456789012345678901234567890123456789."
              "01234567890123456789012345678901234567890123456789.0123456789012345678901234567890123456789012345678."
              "x"             },
      {                                                          "01234567890123456789012345678901234567890123456789."
                                                          "01234567890123456789012345678901234567890123456789."
                                                          "01234567890123456789012345678901234567890123456789."
                                                          "01234567890123456789012345678901234567890123456789.0123456789012345678901234567890123456789012345678."
                                                          "xn--zca",        "01234567890123456789012345678901234567890123456789."
        "01234567890123456789012345678901234567890123456789."
        "01234567890123456789012345678901234567890123456789."
        "01234567890123456789012345678901234567890123456789.0123456789012345678901234567890123456789012345678."
        "xn--zca"             },
      {                                                                "01234567890123456789012345678901234567890123456789."
                                                                "01234567890123456789012345678901234567890123456789."
                                                                "01234567890123456789012345678901234567890123456789."
                                                                "01234567890123456789012345678901234567890123456789.0123456789012345678901234567890123456789012345678."
                                                                "ß",        "01234567890123456789012345678901234567890123456789."
        "01234567890123456789012345678901234567890123456789."
        "01234567890123456789012345678901234567890123456789."
        "01234567890123456789012345678901234567890123456789.0123456789012345678901234567890123456789012345678."
        "xn--zca"             },
      {                                                                       "≯",              "xn--hdh"},
      {                                                                       "≮",              "xn--gdh"},
      {                                                                       "≠",              "xn--1ch"},
    };

    EXPECT_FALSE(unicode::idna::is_label_valid(u8"1"));
    EXPECT_FALSE(unicode::idna::is_label_valid(u8"1a"));
    EXPECT_TRUE(unicode::idna::is_label_valid(u8"نامه‌ای"));
    EXPECT_TRUE(unicode::idna::is_label_valid(u8"correct"));
    EXPECT_EQ(to_ascii(u8"straße.de"), u8"xn--strae-oqa.de");
    EXPECT_FALSE(to_ascii(u8"xn--zn7c.com")); // invalid
    EXPECT_EQ(to_ascii(u8"x-.ß"), u8"x-.xn--zca");
    EXPECT_EQ(to_ascii(u8"نامه‌ای"), u8"xn--mgba3gch31f060k");
    EXPECT_EQ(to_ascii(u8"TESTING-UPPER"), u8"testing-upper");
    EXPECT_EQ(to_ascii(u8"xn--zca.xn--zca"), u8"xn--zca.xn--zca");
    EXPECT_FALSE(to_ascii(u8"يa"));
    EXPECT_EQ(to_ascii(u8"example.org"), u8"example.org");
    EXPECT_EQ(to_ascii(u8"example.org."), u8"example.org.");
    EXPECT_EQ(to_ascii(u8"one"), u8"one");
    EXPECT_EQ(to_ascii(u8"à.\u05D0\u0308"), u8"xn--0ca.xn--ssa73l");
    EXPECT_EQ(to_ascii(u8"xn--0ca.xn--ssa73l"), u8"xn--0ca.xn--ssa73l");
    EXPECT_EQ(to_ascii(u8"à.\u05D0\u0308"), u8"xn--0ca.xn--ssa73l");
    EXPECT_EQ(to_ascii(u8"\u05D0\u0308"), u8"xn--ssa73l");
    EXPECT_EQ(to_ascii(u8"..."), u8"..."); // an empty string is invalid

    for (auto const invalid : invalids) {
        EXPECT_EQ(to_ascii(invalid).value_or(u8""), u8"") << invalid;
        string out;
        EXPECT_FALSE(unicode::idna::is_valid(to_ascii(invalid, out))) << "'" << invalid << "'";
    }

    for (auto const [raw, mappedTo] : valids) {
        // todo: support validateDNS option
        auto const res = to_ascii<string>(raw);
        EXPECT_EQ(res.value_or(""), mappedTo) << raw << "\n" << res.value_or("Nothing");
        string out;
        EXPECT_TRUE(unicode::idna::is_valid(to_ascii(raw, out))) << raw;
        EXPECT_EQ(out, mappedTo) << raw;
    }
}

TEST(BasicIDNATests, ToASCIITestBadInput) {
    using std::array;
    using std::string;
    using std::string_view;
    using std::u16string;
    using std::u32string;
    using std::u8string;
    using webpp::unicode::toNFC;
    using webpp::unicode::idna::mapped;
    using webpp::unicode::idna::status_of;
    using webpp::unicode::idna::to_ascii;

    EXPECT_EQ(to_ascii<string>("128.0,0.1"), "128.0,0.1");
    EXPECT_FALSE(to_ascii<u16string>("\232"));
    EXPECT_FALSE(to_ascii<u16string>("\330"));
    EXPECT_FALSE(to_ascii<u16string>("\012\241"));
    EXPECT_FALSE(to_ascii<u16string>("\012\012\012\377"));
    EXPECT_FALSE(to_ascii<u16string>("\367"));
    EXPECT_FALSE(to_ascii<u16string>("\300\205"));
    EXPECT_FALSE(to_ascii<u16string>("\232G"));
    EXPECT_FALSE(to_ascii<u16string>("\365"));
    EXPECT_FALSE(to_ascii<u16string>("\376\001\001"));
    EXPECT_FALSE(to_ascii<u16string>("\341\012"));

    EXPECT_FALSE(to_ascii<string>("\232"));
    EXPECT_FALSE(to_ascii<string>("\330"));
    EXPECT_FALSE(to_ascii<string>("\012\241"));
    EXPECT_FALSE(to_ascii<string>("\012\012\012\377"));
    EXPECT_FALSE(to_ascii<string>("\367"));
    EXPECT_FALSE(to_ascii<string>("\300\205"));
    EXPECT_FALSE(to_ascii<string>("\232G"));
    EXPECT_FALSE(to_ascii<string>("\365"));
    EXPECT_FALSE(to_ascii<string>("\376\001\001"));
    EXPECT_FALSE(to_ascii<string>("\341\012"));

    EXPECT_FALSE(to_ascii<u32string>("\232"));
    EXPECT_FALSE(to_ascii<u32string>("\330"));
    EXPECT_FALSE(to_ascii<u32string>("\012\241"));
    EXPECT_FALSE(to_ascii<u32string>("\012\012\012\377"));
    EXPECT_FALSE(to_ascii<u32string>("\367"));
    EXPECT_FALSE(to_ascii<u32string>("\300\205"));
    EXPECT_FALSE(to_ascii<u32string>("\232G"));
    EXPECT_FALSE(to_ascii<u32string>("\365"));
    EXPECT_FALSE(to_ascii<u32string>("\376\001\001"));
    EXPECT_FALSE(to_ascii<u32string>("\341\012"));

    EXPECT_FALSE(to_ascii("\232"));
    EXPECT_FALSE(to_ascii("\330"));
    EXPECT_FALSE(to_ascii("\012\241"));
    EXPECT_FALSE(to_ascii("\012\012\012\377"));
    EXPECT_FALSE(to_ascii("\367"));
    EXPECT_FALSE(to_ascii("\300\205"));
    EXPECT_FALSE(to_ascii("\232G"));
    EXPECT_FALSE(to_ascii("\365"));
    EXPECT_FALSE(to_ascii("\376\001\001"));
    EXPECT_FALSE(to_ascii("\341\012"));

    constexpr array buffer = {'x', 'n', '-', '-', 'z', 'c', 'a', char{-1}, '\0'};
    EXPECT_FALSE(to_ascii(string_view{buffer.data(), buffer.size()}));
    EXPECT_FALSE(to_ascii("xn--zcaش"));

    // German capital sharp S (ẞ)
    EXPECT_EQ(to_ascii<u8string>("\xe1\xba\x9e"), u8"xn--zca")
      << "German capital sharp S should convert to expected Punycode";
    EXPECT_EQ(to_ascii<string>(U"\u1E9E"), "xn--zca") << "German capital sharp S should convert to expected Punycode";

    // Replacement character (U+FFFD)
    EXPECT_FALSE(to_ascii<string>("\xef\xbf\xbd.com"));
    EXPECT_FALSE(to_ascii<string>(U"\uFFFD.com"));

    // soft hyphen (U+00AD)
    // Soft Hyphen will be ignored in the IDNA Mapping, but the string is still valid after toASCII
    EXPECT_NE(status_of(U'\xAD'), unicode::idna::details::not_mapped);
    EXPECT_EQ(mapped<u32string>(U'\xAD'), U"");
    EXPECT_FALSE(to_ascii("\xAD")) << "This is not Soft hyphen, this should be interpreted as replacement char";
    EXPECT_EQ(to_ascii(u8"\u00AD"), u8"") << "This is soft hyphen, C++ converts it to UTF-8 automagically.";
    EXPECT_EQ(to_ascii<u32string>(U"\xAD"), U"") << "Soft hyphen should result in empty string";
}

namespace {
    //----------------------------------------------------------------------------
    // HELPER FUNCTIONS FOR PARSING IdnaTestV2.txt
    //----------------------------------------------------------------------------

    /**
     * Converts a Unicode code point to a UTF-8 encoded string.
     */
    void codepoint_to_utf8(char32_t const code_point, std::string& out) {
        // NOLINTBEGIN(*-signed-bitwise)
        if (code_point <= 0x7F) {
            out += static_cast<char>(code_point);
        } else if (code_point <= 0x7FF) {
            out += static_cast<char>(0xC0 | (code_point >> 6));
            out += static_cast<char>(0x80 | (code_point & 0x3F));
        } else if (code_point <= 0xFFFF) {
            out += static_cast<char>(0xE0 | (code_point >> 12));
            out += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (code_point & 0x3F));
        } else if (code_point <= 0x10'FFFF) {
            out += static_cast<char>(0xF0 | (code_point >> 18));
            out += static_cast<char>(0x80 | ((code_point >> 12) & 0x3F));
            out += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (code_point & 0x3F));
        }
        // NOLINTEND(*-signed-bitwise)
    }

    /**
     * Unescapes a string containing \\uXXXX or \\x{XXXX} sequences.
     */
    std::string unescape(std::string_view const src) {
        std::string res;
        res.reserve(src.length());
        for (size_t i = 0; i < src.length(); ++i) {
            if (src.at(i) == '\\' && i + 1 < src.length()) {
                char32_t codepoint = 0;
                size_t   len       = 0;
                try {
                    if (src.at(i + 1) == 'u') {
                        codepoint  = static_cast<char32_t>(std::stoul(std::string(src.substr(i + 2, 4)), &len, 16));
                        i         += 5; // Skip '\u' and 4 hex digits
                    } else if (src.at(i + 1) == 'x' && src.at(i + 2) == '{') {
                        size_t end_pos = src.find('}', i + 3);
                        if (end_pos != std::string_view::npos) {
                            auto hex_part = src.substr(i + 3, end_pos - (i + 3));
                            codepoint     = static_cast<char32_t>(std::stoul(std::string(hex_part), &len, 16));
                            i             = end_pos; // Move index to '}'
                        }
                    } else {
                        res += src.at(i);            // Not a unicode escape, treat as literal
                        continue;
                    }
                    codepoint_to_utf8(codepoint, res);
                } catch (std::exception const&) {
                    // Handle malformed escape sequences if necessary
                    res += '?'; // Add replacement character on error
                }
            } else {
                res += src.at(i);
            }
        }
        return res;
    }

    std::vector<std::string> split(std::string const& line, char delimiter) {
        std::vector<std::string> tokens;
        std::string              token;
        std::istringstream       tokenStream(line);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        // Add empty strings for any trailing delimiters
        if (!line.empty() && line.back() == delimiter) {
            tokens.emplace_back("");
        }
        return tokens;
    }

    std::string_view trim(std::string_view s) {
        s.remove_prefix(std::min(s.find_first_not_of(" \t"), s.size()));
        s.remove_suffix(std::min(s.size() - s.find_last_not_of(" \t") - 1, s.size()));
        return s;
    }

    // Parses a status string like "[B1, V2]" into a set of codes.
    std::set<std::string> parse_status_codes(std::string_view status_str) {
        std::set<std::string> codes;
        if (status_str.length() <= 2) {
            return codes;
        }
        assert(status_str.front() == '[');
        assert(status_str.back() == ']');
        status_str.remove_prefix(1);
        status_str.remove_suffix(1);

        std::stringstream ss(std::string(status_str.data(), status_str.size()));
        std::string       code;
        while (std::getline(ss, code, ',')) {
            codes.insert(std::string(trim(code)));
        }
        return codes;
    }

    // template <typename OutStrT, unsigned Flags, typename... Args>
    // [[nodiscard]] static constexpr webpp::stl::expected<OutStrT, webpp::unicode::idna::to_ascii_status_type>
    // to_ascii_impl(Args&&... args) {
    //     // dependent on Flags at compile time
    //     return webpp::unicode::idna::to_ascii<OutStrT, webpp::unicode::idna::idna_flags(Flags)>(
    //       std::forward<Args>(args)...);
    // }

    // // Helper alias for function pointer type
    // template <typename OutStrT, typename... Args>
    // using to_ascii_fn = webpp::stl::expected<OutStrT, webpp::unicode::idna::to_ascii_status_type> (*)(Args&&...);

    // // Build a constexpr lookup table for all possible flag values
    // template <typename OutStrT, typename... Args, size_t... Is>
    // constexpr auto make_to_ascii_table(std::index_sequence<Is...>) {
    //     return std::array<to_ascii_fn<OutStrT, Args...>, sizeof...(Is)>{&to_ascii_impl<OutStrT, Is, Args...>...};
    // }
    //
    // // Main entry point: runtime flags -> compile-time dispatch
    // template <typename OutStrT = stl::u8string, typename... Args>
    // [[nodiscard]] stl::expected<OutStrT, webpp::unicode::idna::to_ascii_status_type> to_ascii(
    //   webpp::unicode::idna::idna_options options,
    //   Args&&... args) {
    //     constexpr size_t      NumFlags = 0b1 << 12; // 12-bit mask (adjust if wider)
    //     static constexpr auto table    = make_to_ascii_table<OutStrT, Args...>(std::make_index_sequence<NumFlags>{});

    //     auto flags = idna_flags(options);
    //     if (flags < table.size()) {
    //         return table[flags](std::forward<Args>(args)...);
    //     }
    //     return webpp::stl::unexpected(webpp::stl::to_underlying(webpp::unicode::idna::to_ascii_status::unknown));
    // }

    // Main entry point: runtime flags -> compile-time dispatch
    template <typename OutStrT = stl::u8string, typename... Args>
    [[nodiscard]] stl::expected<OutStrT, webpp::unicode::idna::to_ascii_status_type> to_ascii(
      webpp::unicode::idna::idna_options options,
      Args&&... args) {
        using webpp::unicode::idna::idna_flags;
        using webpp::unicode::idna::to_ascii;

        auto const flags = idna_flags(options);
#define webpp_to_ascii_case(flag) \
    case flag: return to_ascii<OutStrT, idna_flags(flag)>(std::forward<Args>(args)...)
        switch (flags) {
            webpp_to_ascii_case(0b0000'0000'0000);
            webpp_to_ascii_case(0b0000'0000'0001);
            webpp_to_ascii_case(0b0000'0000'0010);
            webpp_to_ascii_case(0b0000'0000'0100);
            webpp_to_ascii_case(0b0000'0000'1000);
            webpp_to_ascii_case(0b0000'0001'0000);
            webpp_to_ascii_case(0b0000'0010'0000);
            webpp_to_ascii_case(0b0000'0100'0000);
            webpp_to_ascii_case(0b0000'1000'0000);
            webpp_to_ascii_case(0b0001'0000'0000);
            webpp_to_ascii_case(0b0010'0000'0000);
            webpp_to_ascii_case(0b0100'0000'0000);
            webpp_to_ascii_case(0b1000'0000'0000);

            webpp_to_ascii_case(0b1101'1101'1111);
            webpp_to_ascii_case(0b0111'1101'1111);
            webpp_to_ascii_case(0b1011'1101'1111);
            webpp_to_ascii_case(0b1110'1101'1111);
            webpp_to_ascii_case(0b1111'0101'1111);
            webpp_to_ascii_case(0b1111'1001'1111);
            webpp_to_ascii_case(0b1111'1100'1111);
            webpp_to_ascii_case(0b1111'1101'0111);
            webpp_to_ascii_case(0b1111'1101'1011);
            webpp_to_ascii_case(0b1111'1101'1101);
            webpp_to_ascii_case(0b1111'1101'1110);

            // Special cases, run the debugger and watch for `idna_flags(relaxed_options)` or `flags`:
            // Or comment out the throw statement, replace it with a `break`, add a debugger log
            // `webpp_to_ascii_case({flags});`.
            webpp_to_ascii_case(1307);
            webpp_to_ascii_case(1311);
            webpp_to_ascii_case(1369);
            webpp_to_ascii_case(1371);
            webpp_to_ascii_case(1375);
            webpp_to_ascii_case(1435);
            webpp_to_ascii_case(1437);
            webpp_to_ascii_case(1439);
            webpp_to_ascii_case(1497);
            webpp_to_ascii_case(1499);
            webpp_to_ascii_case(1501);
            webpp_to_ascii_case(1503);
            webpp_to_ascii_case(1759);
            webpp_to_ascii_case(1817);
            webpp_to_ascii_case(1819);
            webpp_to_ascii_case(1821);
            webpp_to_ascii_case(1823);
            webpp_to_ascii_case(1881);
            webpp_to_ascii_case(1883);
            webpp_to_ascii_case(1885);
            webpp_to_ascii_case(1887);
            webpp_to_ascii_case(1945);
            webpp_to_ascii_case(1947);
            webpp_to_ascii_case(1949);
            webpp_to_ascii_case(1951);
            webpp_to_ascii_case(2009);
            webpp_to_ascii_case(2011);
            webpp_to_ascii_case(2013);
            webpp_to_ascii_case(2335);
            webpp_to_ascii_case(2399);
            webpp_to_ascii_case(2523);
            webpp_to_ascii_case(2527);
            webpp_to_ascii_case(2843);
            webpp_to_ascii_case(2845);
            webpp_to_ascii_case(2847);
            webpp_to_ascii_case(2905);
            webpp_to_ascii_case(2907);
            webpp_to_ascii_case(2909);
            webpp_to_ascii_case(2911);
            webpp_to_ascii_case(2969);
            webpp_to_ascii_case(2971);
            webpp_to_ascii_case(2973);
            webpp_to_ascii_case(2975);
            webpp_to_ascii_case(3033);
            webpp_to_ascii_case(3035);
            webpp_to_ascii_case(3037);
            webpp_to_ascii_case(3353);
            webpp_to_ascii_case(3355);
            webpp_to_ascii_case(3357);
            webpp_to_ascii_case(3359);
            webpp_to_ascii_case(3417);
            webpp_to_ascii_case(3419);
            webpp_to_ascii_case(3421);
            webpp_to_ascii_case(3423);
            webpp_to_ascii_case(3483);
            webpp_to_ascii_case(3485);
            webpp_to_ascii_case(3487);
            webpp_to_ascii_case(351);
            webpp_to_ascii_case(3545);
            webpp_to_ascii_case(3547);
            webpp_to_ascii_case(3549);
            webpp_to_ascii_case(3550);
            webpp_to_ascii_case(3865);
            webpp_to_ascii_case(3867);
            webpp_to_ascii_case(3869);
            webpp_to_ascii_case(3871);
            webpp_to_ascii_case(3929);
            webpp_to_ascii_case(3931);
            webpp_to_ascii_case(3933);
            webpp_to_ascii_case(3993);
            webpp_to_ascii_case(3995);
            webpp_to_ascii_case(3997);
            webpp_to_ascii_case(4057);
            webpp_to_ascii_case(4063);
            webpp_to_ascii_case(4091);
            webpp_to_ascii_case(479);
            webpp_to_ascii_case(859);
            webpp_to_ascii_case(925);
            webpp_to_ascii_case(987);
            webpp_to_ascii_case(989);
            webpp_to_ascii_case(991);
            default:
                throw stl::runtime_error(
                  "The specified option at run-time is not present at compile time; add it above.");
        }
#undef webpp_to_ascii_case
        return webpp::stl::unexpected(webpp::stl::to_underlying(webpp::unicode::idna::to_ascii_status::unknown));
    }




} // namespace

TEST(BasicIDNATests, IDNAComplianceTestsExplicit1) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    EXPECT_TRUE(to_ascii<std::string>("fass.de"));
    EXPECT_EQ(to_ascii<std::string>("fass.de"), "fass.de");
    EXPECT_EQ(::to_ascii<std::string>(unicode::idna::idna_options{}, "fass.de"), "fass.de");
    EXPECT_EQ(::to_ascii<std::string>(unicode::idna::strict_idna_options, "fass.de"), "fass.de");

    auto const res = ::to_ascii<std::string>(unicode::idna::strict_idna_options, "faß.de");
    EXPECT_EQ(res, "xn--fa-hia.de") << res.value_or("Nothing");

    EXPECT_EQ(::to_ascii<std::string>(idna_options{}, "faß.de"), "xn--fa-hia.de");
    EXPECT_EQ(::to_ascii<std::string>(idna_options{.CheckHyphens = true}, "faß.de"), "xn--fa-hia.de");
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit2) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    EXPECT_FALSE(::to_ascii<std::string>(unicode::idna::strict_idna_options, "à\u05D0"));
    EXPECT_FALSE(::to_ascii<std::string>(idna_options{.CheckBidi = true}, "à\u05D0"));

    auto res = ::to_ascii<std::string>(idna_options{.CheckBidi = false}, "à\u05D0");
    EXPECT_EQ(res, "xn--0ca24w") << res.value_or("Nothing");
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit3) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    // 0à.\u05D0; ; [B1]; xn--0-sfa.xn--4db; ; ;
    EXPECT_FALSE(::to_ascii<std::string>(unicode::idna::strict_idna_options, "0à.\u05D0"));
    EXPECT_FALSE(::to_ascii<std::string>(idna_options{.CheckBidi = true}, "0à.\u05D0"));

    auto res = ::to_ascii<std::string>(idna_options{.CheckBidi = false}, "0à.\u05D0");
    EXPECT_EQ(res, "xn--0-sfa.xn--4db") << res.value_or("Nothing");
}

// ## UTS #46 Compliance Tests
//
// This test reads `IdnaTestV2.txt` to verify full compliance with the Unicode
// Technical Standard #46 for Internationalized Domain Names in Applications (IDNA2008).
// https://www.unicode.org/reports/tr46/#Conformance_Testing
TEST(BasicIDNATests, IDNAComplianceTests) {
    std::filesystem::path const cur_file   = __FILE__;
    std::filesystem::path       file_path  = cur_file.parent_path();
    file_path                             /= "assets/IdnaTestV2.txt";
    std::ifstream file(file_path);
    ASSERT_TRUE(file.is_open())
      << "Could not open IdnaTestV2.txt. Make sure it's in the same directory as the test executable.";

    std::string line;
    int         failed_tests = 0;
    while (std::getline(file, line)) {
        if (line.empty() || line.at(0) == '#') {
            continue;
        }

        auto comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }
        line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

        auto parts = split(line, ';');
        if (parts.size() < 5) {
            continue;
        }


        std::string       source                = unescape(trim(parts.at(0)));
        std::string       to_unicode_exp        = unescape(trim(parts.at(1)));
        std::string const to_unicode_status     = std::string(trim(parts.at(2)));
        std::string       to_ascii_n_exp        = unescape(trim(parts.at(3)));
        std::string       to_ascii_n_status_str = std::string(trim(parts.at(4)));

        // SCOPED_TRACE("Line: " + std::to_string(line_num) + " | Source: " + source + " | line: " + line);

        if (to_unicode_exp.empty() && source != "\"\"") {
            to_unicode_exp = source;
        }
        if (to_ascii_n_exp.empty()) {
            to_ascii_n_exp = to_unicode_exp;
        }
        if (to_ascii_n_status_str.empty()) {
            to_ascii_n_status_str = to_unicode_status;
        }
        if (source == "\"\"") {
            source = "";
        }
        if (to_ascii_n_status_str == "\"\"") {
            to_ascii_n_status_str = "";
        }
        if (to_ascii_n_exp == "\"\"") {
            to_ascii_n_exp = "";
        }
        // Note: Transitional Processing (columns 5, 6) is skipped as per the idna_options struct.

        // --- Test toASCII with default (strict) options ---
        auto const expected_errors = parse_status_codes(to_ascii_n_status_str);

        // error, if "[<non-empty>]"
        bool const to_ascii_can_fail = to_ascii_n_status_str.length() >= 3 && to_ascii_n_status_str[0] == '[' &&
                                       to_ascii_n_status_str[to_ascii_n_status_str.length() - 1] == ']';

        auto        default_options = unicode::idna::strict_idna_options;
        auto        ascii_n_res     = to_ascii<std::string>(default_options, source);
        std::string error_string;

        if (!ascii_n_res.has_value()) {
            for (auto const status : unicode::idna::to_ascii_status_iterator{ascii_n_res.error()}) {
                error_string += to_string(status);
                error_string += ", ";
            }
        } else {
            error_string = "No error.";
        }

        if (to_ascii_can_fail == ascii_n_res.has_value()) {
            failed_tests++;
        }
        EXPECT_NE(ascii_n_res.has_value(), to_ascii_can_fail)
          << "If we should fail, there should be no value.\n  Error: " << error_string
          << "\n  Failed Tests so far: " << failed_tests << "\n  Expected: " << to_ascii_n_exp;

        if (ascii_n_res) {
            EXPECT_EQ(*ascii_n_res, to_ascii_n_exp);
        }


        // --- If it failed, test again with relaxed options to see if it passes ---
        if (to_ascii_can_fail) {
            auto relaxed_options = unicode::idna::strict_idna_options;

            // Disable checks corresponding to the errors on this line
            // SCOPED_TRACE("Expected Errors: " + to_ascii_n_status_str);
            std::string debug_str;
            for (auto const& error_code : expected_errors) {
                char prefix = error_code.empty() ? ' ' : error_code[0];
                // debug_str   += "Disable " + error_code + " check\n";
                if (error_code == "V2" || error_code == "V3") {
                    relaxed_options.CheckHyphens  = false;
                    debug_str                    += "Disable Hyphens check, ";
                } else if (error_code == "V7") {
                    relaxed_options.CheckMappingRequired  = false;
                    debug_str                            += "Disable Mapping Required Check, ";
                } else if (error_code == "V1") {
                    relaxed_options.CheckNFC  = false;
                    debug_str                += "Disable NFC, ";
                } else if (error_code == "V4") {
                    relaxed_options.CheckACE  = false;
                    debug_str                += "Disable xn-- checking, ";
                } else if (error_code == "V6") {
                    relaxed_options.CheckCombiningMarkAtLabelStart  = false;
                    debug_str                                      += "V6 failure (Combining Mark at start), ";
                } else if (error_code == "A3") {
                    relaxed_options.IgnoreInvalidPunycode  = true;
                    debug_str                             += "Ignore punycode failures, ";
                } else if (prefix == 'B') {
                    relaxed_options.CheckBidi  = false;
                    debug_str                 += "Disable Bidi, ";
                } else if (prefix == 'C') {
                    relaxed_options.CheckJoiners  = false;
                    debug_str                    += "Disable Joiners check, ";
                } else if (prefix == 'U') {
                    relaxed_options.UseSTD3ASCIIRules  = false;
                    debug_str                         += "Disable STD3 ASCII Rules check, ";
                } else if (error_code.starts_with("A4")) {
                    relaxed_options.VerifyDnsLength  = false;
                    debug_str                       += "Disable DNS Length check, ";
                } else if (error_code == "P4") {
                    relaxed_options.CheckDecodeAndValidateLabels  = false;
                    debug_str                                    += "Disable Decode and Validate labels, ";
                } else {
                    throw stl::runtime_error(stl::format("Unknown error code: {}; line: {}", error_code, line));
                }
            }

            // SCOPED_TRACE(debug_str);

            // If all errors are ignorable by our relaxed options, this call should now succeed.
            // if (all_errors_ignored(expected_errors, relaxed_options)) {
            auto ascii_relaxed_res = to_ascii<std::string>(relaxed_options, source);
            error_string           = "";
            if (!ascii_relaxed_res.has_value()) {
                for (auto const status : unicode::idna::to_ascii_status_iterator{ascii_relaxed_res.error()}) {
                    error_string += to_string(status);
                    error_string += ", ";
                }
            }
            if (!ascii_relaxed_res.has_value()) {
                failed_tests++;
            }
            EXPECT_TRUE(ascii_relaxed_res.has_value())
              << "to_ascii should succeed when relevant checks are disabled.\n  Error: " << error_string
              << "\n  Failed Tests so far: " << failed_tests << "\n  Expected: " << to_ascii_n_exp;

            if (ascii_relaxed_res.has_value()) {
                EXPECT_EQ(*ascii_relaxed_res, to_ascii_n_exp)
                  << "  Source: " << source << "\n  Relaxed options failed on line: " << line << "\n  Errors: "
                  << error_string << "\n  Failed Tests so far: " << failed_tests << "\n  Expected: " << to_ascii_n_exp;
            }
        }
    }
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit4) {
    using unicode::idna::loose_idna_options;
    using unicode::idna::to_ascii;

    EXPECT_EQ((to_ascii<std::u8string, loose_idna_options>(u8"xn--?-c1g3623d.xn--1ug73gl146a").value_or(u8"Failed")),
              u8"xn--?-c1g3623d.xn--1ug73gl146a");
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit5) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    static constexpr idna_options options{
      .CheckHyphens                   = false,
      .UseSTD3ASCIIRules              = false,
      .VerifyDnsLength                = false,
      .CheckBidi                      = false,
      .CheckJoiners                   = false,
      .IgnoreInvalidPunycode          = true,
      .CheckNFC                       = false,
      .CheckDotInclusions             = false,
      .CheckMappingRequired           = false,
      .CheckCombiningMarkAtLabelStart = true,
    };


    EXPECT_EQ((to_ascii<std::u8string, options>(u8"𑆀䁴񤧣．ⴕ𝟜\u200C\u0348").error()),
              unicode::idna::to_ascii_status::validity_combining_mark_at_start);

    EXPECT_EQ((to_ascii<std::u8string, options>(u8"xn--1mnx647cg3x1b.xn--4-zfb502tlsl").error()),
              unicode::idna::to_ascii_status::validity_combining_mark_at_start);
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit6) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    static constexpr idna_options options{
      .CheckHyphens                   = true,
      .UseSTD3ASCIIRules              = true,
      .VerifyDnsLength                = true,
      .CheckBidi                      = true,
      .CheckJoiners                   = true,
      .IgnoreInvalidPunycode          = false,
      .CheckNFC                       = true,
      .CheckDotInclusions             = true,
      .CheckMappingRequired           = true,
      .CheckCombiningMarkAtLabelStart = true,
    };

    EXPECT_EQ((to_ascii<std::u8string, options>(u8"xn--2g1d14o.xn--jti").error()),
              unicode::idna::to_ascii_status::validity_combining_mark_at_start);
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit7) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    static constexpr idna_options options{
      .CheckHyphens                   = false,
      .UseSTD3ASCIIRules              = false,
      .VerifyDnsLength                = false,
      .CheckBidi                      = false,
      .CheckJoiners                   = false,
      .IgnoreInvalidPunycode          = false,
      .CheckNFC                       = false,
      .CheckDotInclusions             = false,
      .CheckMappingRequired           = false,
      .CheckCombiningMarkAtLabelStart = false,
    };

    EXPECT_EQ((to_ascii<std::u8string, options>(u8"xn--1mnx647cg3x1b.xn--4-zfb324h32o").value_or(u8"Failed")),
              u8"xn--1mnx647cg3x1b.xn--4-zfb324h32o");
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit8) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    static constexpr idna_options options{
      .CheckHyphens                   = false,
      .UseSTD3ASCIIRules              = false,
      .VerifyDnsLength                = false,
      .CheckBidi                      = false,
      .CheckJoiners                   = false,
      .IgnoreInvalidPunycode          = false,
      .CheckNFC                       = false,
      .CheckDotInclusions             = false,
      .CheckMappingRequired           = false,
      .CheckCombiningMarkAtLabelStart = false,
    };

    EXPECT_EQ((to_ascii<std::u8string, options>(u8"xn--2v9a.xn--ss-q40dp97m").value_or(u8"Failed")),
              u8"xn--2v9a.xn--ss-q40dp97m");
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit9) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    static constexpr idna_options options{
      .CheckHyphens                   = false,
      .UseSTD3ASCIIRules              = false,
      .VerifyDnsLength                = true,
      .CheckBidi                      = false,
      .CheckJoiners                   = false,
      .IgnoreInvalidPunycode          = false,
      .CheckNFC                       = false,
      .CheckDotInclusions             = false,
      .CheckMappingRequired           = false,
      .CheckCombiningMarkAtLabelStart = false,
    };

    // From: https://www.unicode.org/reports/tr46/#ToASCII
    // If the VerifyDnsLength flag is true, then verify DNS length restrictions. This may record an error. For more
    // information, see [STD13] and [STD3].
    //  - The length of the domain name, excluding the root label and its dot, is from 1 to 253.
    //  - The length of each label is from 1 to 63.
    //      Note: Technically, a complete domain name ends with an empty label for the DNS root (see [STD13] [RFC1034]
    //      section 3). This empty label, and the trailing dot, is almost always omitted. When VerifyDnsLength is false,
    //      the empty root label is passed through. When VerifyDnsLength is true, the empty root label is disallowed.
    //      This corresponds to the syntax in [RFC1034] section 3.5 Preferred name syntax which also defines the label
    //      length restrictions.
    EXPECT_EQ((to_ascii<std::u8string, options>(u8"xn--r97c.").error()),
              unicode::idna::to_ascii_status::empty_root_label);
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit10) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    static constexpr idna_options options{
      .CheckHyphens                   = true,
      .UseSTD3ASCIIRules              = false,
      .VerifyDnsLength                = false,
      .CheckBidi                      = false,
      .CheckJoiners                   = false,
      .IgnoreInvalidPunycode          = false,
      .CheckNFC                       = false,
      .CheckDotInclusions             = false,
      .CheckMappingRequired           = false,
      .CheckCombiningMarkAtLabelStart = false,
    };

    // line: xn----ufo9661d.xn--1ug79cm620c71sh; ꡣ-≠.\u200D𞤹𐅢Ↄ; [B1, B6, C2, V7]; xn----ufo9661d.xn--1ug79cm620c71sh; ;
    EXPECT_EQ((to_ascii<std::u8string, options>(u8"xn----ufo9661d.xn--1ug79cm620c71sh").value_or(u8"Failed")),
              u8"xn----ufo9661d.xn--1ug79cm620c71sh");

    // line: 𐹸䚵-ꡡ。⺇; 𐹸䚵-ꡡ.⺇; [B1]; xn----bm3an932a1l5d.xn--xvj; ; ;
    EXPECT_EQ((to_ascii<std::u8string, options>(u8"𐹸䚵-ꡡ。⺇").value_or(u8"Failed")), u8"xn----bm3an932a1l5d.xn--xvj");

    // line: xn----bm3an932a1l5d.xn--xvj; 𐹸䚵-ꡡ.⺇; [B1]; xn----bm3an932a1l5d.xn--xvj; ; ;
    EXPECT_EQ((to_ascii<std::u8string, options>(u8"xn----bm3an932a1l5d.xn--xvj").value_or(u8"Failed")),
              u8"xn----bm3an932a1l5d.xn--xvj");
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit11) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    // Test bidirectional character handling - these should fail with strict options due to Bidi rules
    // From the failing test: 𐫀．ډ𑌀 and 𐫀.ډ𑌀 and xn--pw9c.xn--fjb8658k
    EXPECT_FALSE((to_ascii<std::u8string, unicode::idna::strict_idna_options>(u8"𐫀．ډ𑌀")));
    EXPECT_FALSE((to_ascii<std::u8string, unicode::idna::strict_idna_options>(u8"𐫀.ډ𑌀")));
    EXPECT_FALSE((to_ascii<std::u8string, unicode::idna::strict_idna_options>(u8"xn--pw9c.xn--fjb8658k")));

    // With Bidi checking disabled, these should succeed
    static constexpr idna_options bidi_disabled_options{
      .CheckHyphens                   = true,
      .UseSTD3ASCIIRules              = true,
      .VerifyDnsLength                = true,
      .CheckBidi                      = false,
      .CheckJoiners                   = true,
      .IgnoreInvalidPunycode          = false,
      .CheckNFC                       = true,
      .CheckDotInclusions             = true,
      .CheckMappingRequired           = true,
      .CheckCombiningMarkAtLabelStart = true,
    };

    EXPECT_TRUE((to_ascii<std::u8string, bidi_disabled_options>(u8"𐫀．ډ𑌀")));
    EXPECT_TRUE((to_ascii<std::u8string, bidi_disabled_options>(u8"𐫀.ډ𑌀")));
    EXPECT_TRUE((to_ascii<std::u8string, bidi_disabled_options>(u8"xn--pw9c.xn--fjb8658k")));
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit12) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    static constexpr idna_options options{
      .CheckHyphens                   = true,
      .UseSTD3ASCIIRules              = true,
      .VerifyDnsLength                = true,
      .CheckBidi                      = false,
      .CheckJoiners                   = true,
      .IgnoreInvalidPunycode          = true,
      .CheckNFC                       = true,
      .CheckDotInclusions             = true,
      .CheckMappingRequired           = true,
      .CheckCombiningMarkAtLabelStart = true,
    };

    // Source: ٱ．σߜ | line: \u0671．σ\u07DC; \u0671.σ\u07DC; [B5, B6]; xn--qib.xn--4xa21s; ; ;
    EXPECT_EQ((to_ascii<std::u32string, options>(U"\u0671．σ\u07DC").value_or(U"Failed")), U"xn--qib.xn--4xa21s");
    EXPECT_EQ((to_ascii<std::u8string, options>(u8"\u0671．σ\u07DC").value_or(u8"Failed")), u8"xn--qib.xn--4xa21s");
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit13) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    static constexpr idna_options options{
      .CheckHyphens                   = true,
      .UseSTD3ASCIIRules              = true,
      .VerifyDnsLength                = true,
      .CheckBidi                      = true,
      .CheckJoiners                   = true,
      .IgnoreInvalidPunycode          = true,
      .CheckNFC                       = true,
      .CheckDotInclusions             = true,
      .CheckMappingRequired           = true,
      .CheckCombiningMarkAtLabelStart = true,
    };

    static constexpr idna_options options2{
      .CheckHyphens                   = false,
      .UseSTD3ASCIIRules              = true,
      .VerifyDnsLength                = false,
      .CheckBidi                      = true,
      .CheckJoiners                   = true,
      .IgnoreInvalidPunycode          = true,
      .CheckNFC                       = true,
      .CheckDotInclusions             = true,
      .CheckMappingRequired           = true,
      .CheckCombiningMarkAtLabelStart = true,
    };

    //  Line: 6023 | Source: .xn----938f | line: .xn----938f; .쮇-; [V3, X4_2]; .xn----938f; [V3, A4_2]; ;
    EXPECT_EQ((to_ascii<std::u32string, options>(U".xn----938f").value_or(U"Failed")), U"Failed");
    EXPECT_EQ((to_ascii<std::u8string, options>(u8".xn----938f").value_or(u8"Failed")), u8"Failed");

    EXPECT_EQ((to_ascii<std::u32string, options2>(U".xn----938f").value_or(U"Failed")), U".xn----938f");
    EXPECT_EQ((to_ascii<std::u8string, options2>(u8".xn----938f").value_or(u8"Failed")), u8".xn----938f");
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit14) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    static constexpr idna_options options{
      .CheckHyphens                   = false,
      .UseSTD3ASCIIRules              = false,
      .VerifyDnsLength                = false,
      .CheckBidi                      = false,
      .CheckJoiners                   = false,
      .IgnoreInvalidPunycode          = false,
      .CheckNFC                       = false,
      .CheckDotInclusions             = false,
      .CheckMappingRequired           = false,
      .CheckCombiningMarkAtLabelStart = false,
    };

    // Line: 5759 | Source: xn--0ugba05538b.xn--o8e4044k | line: xn--0ugba05538b.xn--o8e4044k;
    // \u200D\u200D𞵪\u200C.ᡘ𑲭; [B1, C1, C2, V7]; xn--0ugba05538b.xn--o8e4044k; ; ;
    EXPECT_EQ((to_ascii<std::u32string, options>(U"xn--0ugba05538b.xn--o8e4044k").value_or(U"Failed")),
              U"xn--0ugba05538b.xn--o8e4044k");
    EXPECT_EQ((to_ascii<std::u8string, options>(u8"xn--0ugba05538b.xn--o8e4044k").value_or(u8"Failed")),
              u8"xn--0ugba05538b.xn--o8e4044k");
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit15) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    static constexpr idna_options options{
      .CheckHyphens                   = true,
      .UseSTD3ASCIIRules              = true,
      .VerifyDnsLength                = true,
      .CheckBidi                      = false,
      .CheckJoiners                   = true,
      .IgnoreInvalidPunycode          = true,
      .CheckNFC                       = true,
      .CheckDotInclusions             = true,
      .CheckMappingRequired           = true,
      .CheckCombiningMarkAtLabelStart = false,
    };

    // Line: 5707 | Source: ڣ．്‍ϟ | line: \u06A3．\u0D4D\u200Dϟ; \u06A3.\u0D4D\u200Dϟ; [B1, V6];
    // xn--5jb.xn--xya149bpvp; ; xn--5jb.xn--xya149b;
    EXPECT_EQ((to_ascii<std::u32string, options>(U"\u06A3．\u0D4D\u200Dϟ").value_or(U"Failed")),
              U"xn--5jb.xn--xya149bpvp");
    EXPECT_EQ((to_ascii<std::u8string, options>(u8"\u06A3．\u0D4D\u200Dϟ").value_or(u8"Failed")),
              u8"xn--5jb.xn--xya149bpvp");
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit16) {
    using unicode::idna::to_ascii;

    // Line: 4334 | Source: 齙--4.Ss | line: 齙--4.Ss; 齙--4.ss; ; xn----4-p16k.ss; ; ;
    EXPECT_EQ((to_ascii<std::u32string>(U"齙--4.Ss").value_or(U"Failed")), U"xn----4-p16k.ss");
    EXPECT_EQ((to_ascii<std::u8string>(u8"齙--4.Ss").value_or(u8"Failed")), u8"xn----4-p16k.ss");
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit17) {
    using unicode::idna::to_ascii;

    //  Source: .. | line: ..; ; [X4_2]; ; [A4_2]; ;
    EXPECT_FALSE(to_ascii<std::u32string>(U"..").has_value());
    EXPECT_FALSE(to_ascii<std::u8string>(u8"..").has_value());
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit18) {
    using unicode::idna::to_ascii;

    // Line: 209 | Source: 1234567890ä1234567890123456789012345678901234567890123456 | line:
    // 1234567890ä1234567890123456789012345678901234567890123456; ; ;
    // xn--12345678901234567890123456789012345678901234567890123456-fxe; [A4_2]; ;
    EXPECT_FALSE(to_ascii<std::u32string>(U"1234567890ä1234567890123456789012345678901234567890123456").has_value());
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit19) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    // Test case from failing test: Line 302: xn--0.pt
    // Error: The ASCII-Only label was unnecessarily encoded into punycode
    static constexpr idna_options relaxed_options{
      .CheckHyphens                   = false,
      .UseSTD3ASCIIRules              = false,
      .VerifyDnsLength                = false,
      .CheckBidi                      = false,
      .CheckJoiners                   = false,
      .IgnoreInvalidPunycode          = true,
      .CheckNFC                       = false,
      .CheckDotInclusions             = false,
      .CheckMappingRequired           = false,
      .CheckCombiningMarkAtLabelStart = false,
      .CheckDecodeAndValidateLabels   = false,
    };

    EXPECT_EQ((to_ascii<std::string, relaxed_options>("xn--0.pt").value_or("Failed")), "xn--0.pt");
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit20) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    // Test case from failing test: Line 304: xn--a-Ä.pt
    // Error: Invalid code point was found
    static constexpr idna_options relaxed_options{
      .CheckHyphens                   = false,
      .UseSTD3ASCIIRules              = false,
      .VerifyDnsLength                = false,
      .CheckBidi                      = false,
      .CheckJoiners                   = false,
      .IgnoreInvalidPunycode          = true,
      .CheckNFC                       = false,
      .CheckDotInclusions             = false,
      .CheckMappingRequired           = false,
      .CheckCombiningMarkAtLabelStart = false,
      .CheckDecodeAndValidateLabels   = false,
    };

    EXPECT_TRUE((to_ascii<std::string, relaxed_options>("xn--a-Ä.pt").has_value()));
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit21) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    // Test case from failing test: Line 305: xn--a-Ä.pt (with combining mark)
    // Error: Invalid code point was found
    static constexpr idna_options relaxed_options{
      .CheckHyphens                   = false,
      .UseSTD3ASCIIRules              = false,
      .VerifyDnsLength                = false,
      .CheckBidi                      = false,
      .CheckJoiners                   = false,
      .IgnoreInvalidPunycode          = true,
      .CheckNFC                       = false,
      .CheckDotInclusions             = false,
      .CheckMappingRequired           = false,
      .CheckCombiningMarkAtLabelStart = false,
      .CheckDecodeAndValidateLabels   = false,
    };

    EXPECT_TRUE((to_ascii<std::string, relaxed_options>("xn--a-A\u0308.pt").has_value()));
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit22) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    // Test case from failing test: Line 306: xn--a-ä.pt (with combining mark)
    // Error: Invalid code point was found
    static constexpr idna_options relaxed_options{
      .CheckHyphens                   = false,
      .UseSTD3ASCIIRules              = false,
      .VerifyDnsLength                = false,
      .CheckBidi                      = false,
      .CheckJoiners                   = false,
      .IgnoreInvalidPunycode          = true,
      .CheckNFC                       = false,
      .CheckDotInclusions             = false,
      .CheckMappingRequired           = false,
      .CheckCombiningMarkAtLabelStart = false,
      .CheckDecodeAndValidateLabels   = false,
    };

    EXPECT_TRUE((to_ascii<std::string, relaxed_options>("xn--a-a\u0308.pt").has_value()));
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit23) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    // Test case from failing test: Line 307: xn--a-ä.pt
    // Error: Invalid code point was found
    static constexpr idna_options relaxed_options{
      .CheckHyphens                   = false,
      .UseSTD3ASCIIRules              = false,
      .VerifyDnsLength                = false,
      .CheckBidi                      = false,
      .CheckJoiners                   = false,
      .IgnoreInvalidPunycode          = true,
      .CheckNFC                       = false,
      .CheckDotInclusions             = false,
      .CheckMappingRequired           = false,
      .CheckCombiningMarkAtLabelStart = false,
      .CheckDecodeAndValidateLabels   = false,
    };

    EXPECT_TRUE((to_ascii<std::string, relaxed_options>("xn--a-ä.pt").has_value()));
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit24) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    // Test case from failing test: Line 308: XN--A-Ä.PT (uppercase)
    // Error: Invalid code point was found
    static constexpr idna_options relaxed_options{
      .CheckHyphens                   = false,
      .UseSTD3ASCIIRules              = false,
      .VerifyDnsLength                = false,
      .CheckBidi                      = false,
      .CheckJoiners                   = false,
      .IgnoreInvalidPunycode          = true,
      .CheckNFC                       = false,
      .CheckDotInclusions             = false,
      .CheckMappingRequired           = false,
      .CheckCombiningMarkAtLabelStart = false,
      .CheckDecodeAndValidateLabels   = false,
    };

    EXPECT_TRUE((to_ascii<std::string, relaxed_options>("XN--A-Ä.PT").has_value()));
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit25) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    // Test case from failing test: Line 309: XN--A-Ä.PT (with combining mark)
    // Error: Invalid code point was found
    static constexpr idna_options relaxed_options{
      .CheckHyphens                   = false,
      .UseSTD3ASCIIRules              = false,
      .VerifyDnsLength                = false,
      .CheckBidi                      = false,
      .CheckJoiners                   = false,
      .IgnoreInvalidPunycode          = true,
      .CheckNFC                       = false,
      .CheckDotInclusions             = false,
      .CheckMappingRequired           = false,
      .CheckCombiningMarkAtLabelStart = false,
      .CheckDecodeAndValidateLabels   = false,
    };

    EXPECT_TRUE((to_ascii<std::string, relaxed_options>("XN--A-A\u0308.PT").has_value()));
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit26) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    // Test case from failing test: Line 310: Xn--A-Ä.pt (mixed case)
    // Error: Invalid code point was found
    static constexpr idna_options relaxed_options{
      .CheckHyphens                   = false,
      .UseSTD3ASCIIRules              = false,
      .VerifyDnsLength                = false,
      .CheckBidi                      = false,
      .CheckJoiners                   = false,
      .IgnoreInvalidPunycode          = true,
      .CheckNFC                       = false,
      .CheckDotInclusions             = false,
      .CheckMappingRequired           = false,
      .CheckCombiningMarkAtLabelStart = false,
      .CheckDecodeAndValidateLabels   = false,
    };

    EXPECT_TRUE((to_ascii<std::string, relaxed_options>("Xn--A-A\u0308.pt").has_value()));
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit27) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    // Test case from failing test: Line 311: Xn--A-Ä.pt (mixed case)
    // Error: Invalid code point was found
    static constexpr idna_options relaxed_options{
      .CheckHyphens                   = false,
      .UseSTD3ASCIIRules              = false,
      .VerifyDnsLength                = false,
      .CheckBidi                      = false,
      .CheckJoiners                   = false,
      .IgnoreInvalidPunycode          = true,
      .CheckNFC                       = false,
      .CheckDotInclusions             = false,
      .CheckMappingRequired           = false,
      .CheckCombiningMarkAtLabelStart = false,
      .CheckDecodeAndValidateLabels   = false,
    };

    EXPECT_TRUE((to_ascii<std::string, relaxed_options>("Xn--A-Ä.pt").has_value()));
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit28) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    // Test case from failing test: xn--ASCII-.pt
    static constexpr idna_options relaxed_options{
      .CheckHyphens                   = false,
      .UseSTD3ASCIIRules              = false,
      .VerifyDnsLength                = false,
      .CheckBidi                      = false,
      .CheckJoiners                   = false,
      .IgnoreInvalidPunycode          = true,
      .CheckNFC                       = false,
      .CheckDotInclusions             = false,
      .CheckMappingRequired           = false,
      .CheckCombiningMarkAtLabelStart = false,
      .CheckDecodeAndValidateLabels   = false,
    };

    EXPECT_EQ((to_ascii<std::string, relaxed_options>("xn--ASCII-.pt").value_or("Failed")), "ascii.pt");
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit29) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    //  Line: 527 | Source: ۯ‌ۯ | line: \u06EF\u200C\u06EF; ; [C1]; xn--cmba004q; ; xn--cmba; []
    EXPECT_FALSE((to_ascii<std::u32string>(U"\u06EF\u200C\u06EF").has_value()));
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit30) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    static constexpr idna_options relaxed_options{
      .CheckHyphens                   = true,
      .UseSTD3ASCIIRules              = true,
      .VerifyDnsLength                = true,
      .CheckBidi                      = true,
      .CheckJoiners                   = false,
      .IgnoreInvalidPunycode          = false,
      .CheckNFC                       = true,
      .CheckDotInclusions             = true,
      .CheckMappingRequired           = true,
      .CheckCombiningMarkAtLabelStart = true,
      .CheckDecodeAndValidateLabels   = true,
    };

    // Line: 4408 | Source: xn--9-i0j5967eg3qz.xn--ss-l1t | line: xn--9-i0j5967eg3qz.xn--ss-l1t;
    // 𲮚9ꍩ\u17D3.\u200Dss; [C2]; xn--9-i0j5967eg3qz.xn--ss-l1t; ; ;
    EXPECT_TRUE((to_ascii<std::u32string, relaxed_options>(U"xn--9-i0j5967eg3qz.xn--ss-l1t").has_value()));
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit31) {
    using unicode::idna::idna_options;
    using unicode::idna::to_ascii;

    static constexpr idna_options relaxed_options{
      .CheckHyphens                   = true,
      .UseSTD3ASCIIRules              = true,
      .VerifyDnsLength                = true,
      .CheckBidi                      = false,
      .CheckJoiners                   = true,
      .IgnoreInvalidPunycode          = false,
      .CheckNFC                       = true,
      .CheckDotInclusions             = true,
      .CheckMappingRequired           = false,
      .CheckCombiningMarkAtLabelStart = true,
      .CheckDecodeAndValidateLabels   = true,
    };

    // Line: 4725 | Source: xn--lgb32f2753cosb.xn--jkb91hlz1azih | line: xn--lgb32f2753cosb.xn--jkb91hlz1azih;
    // \u07BB𐹳\u0626𑁆.\u08A7\u06B0\u200Cᢒ; [B2, B3, V7]; xn--lgb32f2753cosb.xn--jkb91hlz1azih; ; ;
    EXPECT_TRUE((to_ascii<std::u32string, relaxed_options>(U"xn--lgb32f2753cosb.xn--jkb91hlz1azih").has_value()));
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit32) {
    using unicode::idna::idna_options;
    using unicode::idna::strict_idna_options;
    using unicode::idna::to_ascii;

    static constexpr idna_options relaxed_options{
      .CheckHyphens                   = true,
      .UseSTD3ASCIIRules              = true,
      .VerifyDnsLength                = false,
      .CheckBidi                      = true,
      .CheckJoiners                   = true,
      .IgnoreInvalidPunycode          = false,
      .CheckNFC                       = true,
      .CheckDotInclusions             = true,
      .CheckMappingRequired           = true,
      .CheckCombiningMarkAtLabelStart = true,
      .CheckDecodeAndValidateLabels   = false,
    };

    // Line: 204 | Source: "" | line: ""; ; [X4_2]; ; [A4_1, A4_2]; ;
    EXPECT_FALSE((to_ascii<std::u32string, strict_idna_options>(U"").has_value()));
    EXPECT_TRUE((to_ascii<std::u32string, relaxed_options>(U"").has_value()));

    // Line: 550 | Source: xn-- | line: xn--; ""; [P4, X4_2]; ; [P4, A4_1, A4_2]; ;
    EXPECT_EQ((to_ascii<std::u32string, relaxed_options>(U"xn--").value_or(U"Failed")), U"");
}

TEST(BasicIDNATests, IDNAComplianceTestsExplicit33) {
    using unicode::idna::idna_options;
    using unicode::idna::strict_idna_options;
    using unicode::idna::to_ascii;

    static constexpr idna_options relaxed_options{
      .CheckHyphens                   = false,
      .UseSTD3ASCIIRules              = true,
      .VerifyDnsLength                = true,
      .CheckBidi                      = true,
      .CheckJoiners                   = true,
      .IgnoreInvalidPunycode          = false,
      .CheckNFC                       = true,
      .CheckDotInclusions             = true,
      .CheckMappingRequired           = true,
      .CheckCombiningMarkAtLabelStart = true,
      .CheckDecodeAndValidateLabels   = true,
    };

    // V3 = Starting or ending with a hyphen
    // Line: 3285 | Source: xn----zmb.xn--rlj2573p | line: xn----zmb.xn--rlj2573p; σ-.ⴣ𦟙; [V3]; xn----zmb.xn--rlj2573p;
    EXPECT_FALSE((to_ascii<std::u32string, strict_idna_options>(U"xn----zmb.xn--rlj2573p").has_value()));
    EXPECT_FALSE((to_ascii<std::u8string, strict_idna_options>(u8"xn----zmb.xn--rlj2573p").has_value()));
    EXPECT_EQ((to_ascii<std::u32string, relaxed_options>(U"xn----zmb.xn--rlj2573p").value_or(U"Failed")),
              U"xn----zmb.xn--rlj2573p");
}

// NOLINTEND(*-magic-numbers, *-pro-bounds-pointer-arithmetic, *-use-designated-initializers)
