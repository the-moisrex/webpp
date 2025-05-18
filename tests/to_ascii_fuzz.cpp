#include "../webpp/unicode/idna.hpp"
#include "./common/fuzz_common.hpp"

void to_ascii_fuzz(std::string_view data) {
    using std::string;
    using std::u16string;
    using std::u32string;
    using std::u8string;
    using webpp::unicode::idna::idna_options;
    using webpp::unicode::idna::to_ascii;

    auto run = [&]<idna_options Options>() {
        auto const res   = to_ascii<string, Options>(data);
        auto const res8  = to_ascii<u8string, Options>(data);
        auto const res16 = to_ascii<u16string, Options>(data);
        auto const res32 = to_ascii<u32string, Options>(data);

        if (res.has_value()) {
            ASSERT_EQ(res->size(), res8->size());
        }

        // todo: add to_unicode to the tests as well after implementing it
    };

    run.operator()<idna_options{}>();
    run.operator()<idna_options{.VerifyDnsLength = true}>();
    run.operator()<idna_options{.CheckHyphens = true}>();
    run.operator()<idna_options{.UseSTD3ASCIIRules = true}>();
    run.operator()<idna_options{.IgnoreInvalidPunycode = true}>();
    // run.operator()<idna_options{.CheckNFC = false}>();
    run.operator()<idna_options{.CheckDotInclusions = true}>();
    run.operator()<idna_options{.CheckStatusValues = true}>();
    run.operator()<idna_options{.CheckHyphens          = true,
                                .CheckBidi             = true,
                                .CheckJoiners          = true,
                                .UseSTD3ASCIIRules     = true,
                                .VerifyDnsLength       = true,
                                .IgnoreInvalidPunycode = true,
                                .CheckNFC           = false, // todo: enable it after implementation of isNFC
                                .CheckDotInclusions = true,
                                .CheckStatusValues  = true}>();
}

register_fuzz(to_ascii_fuzz);
