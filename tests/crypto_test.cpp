// Created by moisrex on 2/4/20.

#include "../core/include/webpp/crypto/base64.hpp"
#include "../core/include/webpp/crypto/base64_url.hpp"
#include "../core/include/webpp/crypto/gzip.hpp"
#include "./common_pch.hpp"


using namespace webpp;


TEST(Crypto, GZip) {
    std::string original{"zip me"};
    auto        compressed   = gzip::compress(original.data(), original.size());
    auto        decompressed = gzip::decompress(compressed.data(), compressed.size());
    EXPECT_EQ(original, decompressed);
}


TEST(Crypto, Base64) {
    std::string orig = "encode me up";
    std::string enc, dec;
    base64::encode(orig, enc);
    EXPECT_TRUE(base64::decode(enc, dec));
    EXPECT_EQ(orig, dec);
}
