// Created by moisrex on 12/21/20.

#include "../webpp/unicode/ustring.hpp"

#include "common/tests_common_pch.hpp"

using namespace webpp;
using namespace std;

TEST(UString, UTF8_Init) {
    utf8<> const one{u8"hello world"};
    utf8<> const two{"hello world"};
    utf8<> const three{u8"سلام"};
    // EXPECT_TRUE(one.operator==("hello world"));
    EXPECT_EQ(one, "hello world");
    EXPECT_EQ(two, "hello world");
    EXPECT_EQ(three, u8"سلام");
    EXPECT_EQ(three, "سلام"); // super weird stuff, I know
}

/*

void test_1() {
    bool ok = true;
    printf("** Unit Test 1\n");

    // default constructor & destructor

    { ustring::ustring str; }

    printf("Create and Destroy an empty string\n");

    if (ok) {
        printf("End Unit Test One - PASSED\n\n");

    } else {
        printf("End Unit Test One - Failed\n\n");
        g_unitTest = false;
    }
}

void test_2() {
    printf("\n** Unit Test 2\n");

    // CsChar internationalization test

    // unicode 00 42, data type char
    ustring::CsChar c0 = 'B';

    // unicode 00 BF, data type maybe char or int, compile will return the value
    ustring::CsChar c127 = '¿';
    ustring::CsChar u127 = UCHAR('¿');

    // unicode 21 B4, data type int or a compile error, not safe
    ustring::CsChar c256 = '↴';

    // unicode 21 B4, data type char32_t, guaranteed to be the proper unicode value
    ustring::CsChar u256 = UCHAR('↴');

    // unicode 01 D1 60, data type int or a compile error, not safe
    ustring::CsChar cX = '𝅘𝅥𝅮';

    // unicode 01 D1 60, data type char32_t, guaranteed to be the proper unicode value
    ustring::CsChar uX = UCHAR('𝅘𝅥𝅮');

    printf("\n");
    printf("Char B    %08x \n", c0.unicode());

    printf("Char ¿    %08x    (implementation defined, unsafe) \n", c127.unicode());
    printf("Char ¿    %08x    (unicode literal)  \n", u127.unicode());

    printf("Char ↴    %08x    (implementation defined, unsafe) \n", c256.unicode());
    printf("Char ↴    %08x    (unicode literal)  \n", u256.unicode());

    printf("Char 𝅘𝅥𝅮    %08x    (implementation defined, unsafe) \n", cX.unicode());
    printf("Char 𝅘𝅥𝅮    %08x    (unicode literal)  \n", uX.unicode());

    printf("\n");

    // not safe
    ustring::ustring str("↴");

    printf("\nIn test 2B ustring() is passed a multi-byte string literal.\n"
           "This is not safe since the constructor will assume the data is Latin-1, \n"
           "which in this case is not true. Must be passed as a UCHAR.\n");

    printf("\nString literal ↴ : %s   (mangled, 86 is non printable)", str.constData());
    printf("\nUnicode value    : %x  %x  %x \n", str[0].unicode(), str[1].unicode(), str[2].unicode());

    for (auto c = str.constData(); *c != '\0'; ++c) {
        printf("\nRaw numeric value stored in the buffer : %02x", *c);
    }

    printf("\n");


    //
    ustring::ustring str2(U"ABCD↴");

    printf("\nIn test 2C ustring() is passed a UTF-32 string literal with a UTF \n"
           "specifier. This calls the constructor which takes a const char32_t *\n");

    printf("\nUTF-32 string literal ABCD↴ : %s", str2.constData());

    printf("\n\n");
}

void test_3() {
    bool ok = true;
    printf("\n** Unit Test 3\n");

    {
        ustring::ustring str;
        str.append('A');
        str.append(':');
        str.append(' ');
        str.append(UCHAR('¿'));
        str.append(' ');
        str.append(' ');
        str.append(' ');
        str.append('B');
        str.append(':');
        str.append(' ');
        str.append(UCHAR('↴'));
        str.append(' ');
        str.append(' ');
        str.append(' ');
        str.append('C');
        str.append(':');
        str.append(' ');
        str.append(UCHAR('𝅘𝅥𝅮'));

        printf(
          "\nA: (2 bytes) upside down question mark \nB: (3 bytes) rightwards arrow with corner downwards \n"
          "C: (4 bytes) musical symbol eighth note \n%s\n",
          str.constData());
    }

    printf("\n");

    if (ok) {
        printf("End Unit Test Three - PASSED\n\n");

    } else {
        printf("End Unit Test Three - Failed\n\n");
        g_unitTest = false;
    }
}

void test_4() {
    bool ok = true;
    printf("\n** Unit Test 4\n");

    static_assert(std::is_move_constructible<ustring::ustring>::value, "Unable to move ustring");

    // part a
    ustring::ustring str1("A wacky fox and sizeable pig jumped halfway over a blue moon.");
    printf("\nConstructor passed a string literal: %s", str1.constData());

    ustring::ustring str2(str1);
    printf("\nCopy Construct String 1 to String 2: %s", str2.constData());

    ustring::ustring str3(std::move(str1));
    printf("\nMove Construct String 1 to String 3: %s", str3.constData());

    // part b
    const char*        tmp = "This is a string literal assigned to a 'const char *'";
    ustring::ustring str4(tmp);
    printf("\n\nConstructor passed a const char *  : %s\n", str4.constData());

    printf("\n");

    if (ok) {
        printf("End Unit Test Four - PASSED\n\n");

    } else {
        printf("End Unit Test Fout - Failed\n\n");
        g_unitTest = false;
    }
}

void test_5() {
    bool ok = true;
    printf("\n** Unit Test 5\n");

    ustring::ustring str1(5, UCHAR('↴'));
    printf("\nConstructed string with 5 copies of the same 3 byte character: %s", str1.constData());

    std::vector<ustring::CsChar> v = {'G', 'I', 'N', 'G', 'E', 'R'};
    ustring::ustring            str2(v.begin(), v.begin() + 3);
    printf("\nConstructed string from vector GINGER, use iterator to copy first 3 elements: %s\n",
           str2.constData());

    if (str2 != "GIN") {
        ok = false;
    }

    // find
    // To dream t he impossi ble dream
    // 0123456789 0123456789 012345678

    ustring::ustring str3("To dream the impossible dream");

    int x1 = str3.find(ustring::ustring("dream"));
    int x2 = str3.find("dream", 10);
    int x3 = str3.find_first_not_of('s', 17);

    printf("\nGiven the following string              : %s", str3.constData());
    printf("\nStart at index  0, find 'dream'         : %d", x1);
    printf("\nStart at index 10, find 'dream'         : %d", x2);
    printf("\nStart at index 17, find first not of 's': %d", x3);

    printf("\n\n");

    if (x1 != 3 || x2 != 24) {
        ok = false;
    }

    if (ok) {
        printf("End Unit Test Five - PASSED\n\n");

    } else {
        printf("End Unit Test Five - Failed\n\n");
        g_unitTest = false;
    }
}

void test_6() {
    bool ok = true;
    printf("\n** Unit Test 6\n");

    ustring::ustring str1("Ending character is 3 bytes ");
    str1.append(UCHAR('↴'));

    ustring::ustring str2;
    str2 = str1;

    printf("\nAssign String 1 to String 2: %s", str2.constData());

    ustring::ustring str3;
    str3 = std::move(str1);

    printf("\nMove   String 1 to String 3: %s\n", str3.constData());

    if (str2 != str3) {
        ok = false;
    }

    // insert tests
    str3.insert(6, 2, UCHAR('↵'));
    printf("\nInsert  2 left arrows  at the 7th character: %s", str3.constData());

    str3.insert(6, " [string literal] ");
    printf("\nInsert  string literal at the 7th character: %s", str3.constData());


    // replace
    ustring::ustring tmp = " [string literal] xx ";
    int                len = tmp.size();

    str3.replace(6, len, " { new string text } ");
    printf("\nReplace string literal at the 7th character: %s", str3.constData());


    printf("\n\n");

    if (ok) {
        printf("End Unit Test Six - PASSED\n\n");

    } else {
        printf("End Unit Test Six - Failed\n\n");
        g_unitTest = false;
    }
}

void test_7() {
    bool ok = true;
    printf("\n** Unit Test 7\n");

    ustring::ustring str1("ABCD");
    str1.append(UCHAR('↴')); // 3 bytes
    str1.append(UCHAR('¿')); // 2 bytes
    str1.append('E');
    str1.append(UCHAR('𝅘𝅥𝅮')); // 4 bytes, unicode 01 D1 60
    str1.append('F');

    printf("\nOriginal String: %s", str1.constData());
    printf("\n");


    // 2
    ustring::ustring::const_iterator iter2 = str1.end();

    if (iter2 == str1.begin()) {
        // do nothing

    } else {
        --iter2;

        while (true) {
            const ustring::CsChar c = *iter2;

            ustring::ustring tmp(1, c);
            printf("\nWalk backwards: %s", tmp.constData());

            if (iter2 == str1.begin()) {
                break;
            }

            --iter2;
        }
    }

    printf("\n");


    // 3
    ustring::ustring str3 = str1.substr(3, 4);
    printf("\nSubstring beginning at 3, length 4: %s", str3.constData());

    printf("\n");


    // 4
    ustring::ustring::const_iterator iter4    = str1.begin();
    int                                eraseCnt = 0;

    while (!str1.empty()) {
        ustring::ustring tmp(1, *iter4);

        iter4 = str1.erase(iter4);
        ++eraseCnt;

        printf("\nErase %s element: %s", tmp.constData(), str1.constData());
    }

    printf("\n\n");

    if (eraseCnt != 9) {
        ok = false;
    }

    if (ok) {
        printf("End Unit Test Seven - PASSED\n\n");

    } else {
        printf("End Unit Test Seven - Failed\n\n");
        g_unitTest = false;
    }
}

void test_8() {
    bool ok = true;
    printf("\n** Unit Test 8\n");

    // 1
    ustring::ustring str1("ABCD");
    str1.append(UCHAR('↴'));
    printf("\nOriginal String (↴ is 3 bytes): %s\n", str1.constData());

    // 2
    printf("\nString - size storage    : %d", str1.size_storage());
    printf("\nString - size code points: %d", str1.size_codePoints());
    printf("\nString - size            : %d", str1.size());
    printf("\nString - length          : %d\n", str1.length());

    if (str1.size_codePoints() != 5) {
        ok = false;
    }

    // 3
    ustring::ustring::const_iterator iter = str1.begin();
    iter                                    = iter + 2;

    ustring::ustring str2(iter, str1.end());
    printf("\nCopy original string from begin() + 2: %s\n", str2.constData());

    if (str2 != ustring::ustring("CD") + UCHAR('↴')) {
        ok = false;
    }

    // 4
    ustring::ustring str4 = str1.substr(3, 2);
    printf("\nSubstring beginning at 3, length 2: %s", str4.constData());

    printf("\n\n");


    if (ok) {
        printf("End Unit Test Eight - PASSED\n\n");

    } else {
        printf("End Unit Test Eight - Failed\n\n");
        g_unitTest = false;
    }
}

void test_9() {
    bool ok = true;
    printf("\n** Unit Test 9\n");

    ustring::ustring str1("ABCD (");
    str1.append(UCHAR('↴')); // 21b4
    str1.append(UCHAR(')'));

    ustring::ustring str2("ABCD (");
    str2.append(UCHAR('↵')); // 21b5
    str2.append(UCHAR(')'));

    printf("\nString 1  (unicode 21b4):  %s", str1.constData());
    printf("\nString 2  (unicode 21b5):  %s\n", str2.constData());

    // 1
    printf("\nTest if String 1 == String 2:  ");

    if (str1 == str2) {
        printf("Test == is true");
        ok = false;

    } else {
        printf("Test == is false");
    }

    // 2
    printf("\nTest if String 1 != String 2:  ");

    if (str1 != str2) {
        printf("Test != is true");

    } else {
        printf("Test != is false");
        ok = false;
    }

    // 3
    printf("\nTest if String 1 >  String 2:  ");

    if (str1 > str2) {
        printf("Test >  is true");
        ok = false;

    } else {
        printf("Test >  is false");
    }

    // 4
    printf("\nTest if String 1 <  String 2:  ");

    if (str1 < str2) {
        printf("Test <  is true");

    } else {
        printf("Test <  is false");
        ok = false;
    }

    printf("\n\n");

    if (ok) {
        printf("End Unit Test Nine - PASSED\n\n");

    } else {
        printf("End Unit Test Nine - Failed\n\n");
        g_unitTest = false;
    }
}

void test_10() {
    bool ok = true;
    printf("\n** Unit Test 10\n");

    ustring::CsString_utf16 str1("ABCD");
    str1.append(UCHAR('↴')); // 2 bytes     21 B4
    str1.append(UCHAR('¿')); // 2 bytes        BF
    str1.append('E');
    str1.append(UCHAR('𝅘𝅥𝅮')); // 4 bytes  01 D1 60
    str1.append('F');

    // 1
    ustring::ustring str2;
    convert(str1, str2);

    printf("\nString Constructed using UTF-16 then convert() to UTF-8 : %s", str2.constData());

    ustring::CsChar c = str1[7];
    printf("\nCsChar 7 (unicode 01 d1 60) : %08x", c.unicode());

    printf("\n\n");

    if (c != UCHAR('𝅘𝅥𝅮')) {
        ok = false;
    }

    if (ok) {
        printf("End Unit Test Ten - PASSED\n\n");

    } else {
        printf("End Unit Test Ten - Failed\n\n");
        g_unitTest = false;
    }
}

void test_11() {
    bool ok = true;
    printf("\n** Unit Test 11\n");

    ustring::ustring str1("ABCD");
    str1.append(UCHAR('↴')); // 3 bytes     21 B4
    printf("\nOriginal string : %s", str1.constData());

    str1.resize(8, UCHAR('¿'));
    printf("\nResize to 8, passing ¿ : %s", str1.constData());

    str1.resize(7);
    printf("\nResize new string to 7 : %s", str1.constData());

    str1.resize(3);
    printf("\nResize new string to 3 : %s", str1.constData());

    printf("\n\n");

    if (str1 != "ABC") {
        ok = false;
    }

    if (ok) {
        printf("End Unit Test Eleven - PASSED\n\n");

    } else {
        printf("End Unit Test Eleven - Failed\n\n");
        g_unitTest = false;
    }
}

void test_12() {
    bool ok = true;
    printf("\n** Unit Test 12\n");

    ustring::ustring str1("ABCD");
    str1.append(UCHAR('↴')); // 3 bytes     21 B4
    printf("\nOriginal string : %s", str1.constData());

    // find_first_not_of
    int pos = str1.find_first_not_of("DAZB");
    printf("\nFind first not of 'DAZB' no start pos : %d", pos);

    pos = str1.find_first_not_of("B↴", 1);
    printf("\nFind first not of 'B↴'   start pos 1  : %d", pos);

    printf("\n");

    // find_last_not_of
    pos = str1.find_last_not_of("E", 3);
    printf("\nFind last not of 'E'     start pos 3  : %d", pos);

    printf("\n");

    // rfind
    pos = str1.rfind('D');
    printf("\nReverse find 'D' no start pos         : %d", pos);

    pos = str1.rfind('D', 3);
    printf("\nReverse find 'D' start pos 3          : %d", pos);

    pos = str1.rfind('A', 9);
    printf("\nReverse find 'A' start pos 9          : %d", pos);

    printf("\n\n");

    if (ok) {
        printf("End Unit Test Twelve - PASSED\n\n");

    } else {
        printf("End Unit Test Twelve - Failed\n\n");
        g_unitTest = false;
    }
}

void test_13() {
    bool ok = true;
    printf("\n** Unit Test 13\n");

    // currently unused

    if (ok) {
        printf("End Unit Test Thirteen - PASSED\n\n");

    } else {
        printf("End Unit Test Thirteen - Failed\n\n");
        g_unitTest = false;
    }
}

void test_14() {
    bool ok = true;
    printf("\n** Unit Test 14\n");

    // currently unused

    if (ok) {
        printf("End Unit Test Fourteen - PASSED\n\n");

    } else {
        printf("End Unit Test Fourteen - Failed\n\n");
        g_unitTest = false;
    }
}

void test_15() {
    bool ok = true;
    printf("\n** Unit Test 15\n");

    // currently unused

    if (ok) {
        printf("End Unit Test Fifteen - PASSED\n\n");

    } else {
        printf("End Unit Test Fifteen - Failed\n\n");
        g_unitTest = false;
    }
}

//  ustring &operator=(const ustring &str); where S1 is UTF-8 and S2 is UTF-16 -> should be a compile error



 */
