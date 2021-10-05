// Created by moisrex on 2/4/20.

#include "../core/include/webpp/crypto/gzip.hpp"
#include "./common_pch.hpp"
#include <gtest/gtest.h>


using namespace webpp;


TEST(Crypto, GZip) {
  std::string original{"zip me"};
  auto compressed = gzip::compress(original.data(), original.size());
  auto decompressed = gzip::decompress(compressed.data(), compressed.size());
  EXPECT_EQ(original, decompressed);
}
