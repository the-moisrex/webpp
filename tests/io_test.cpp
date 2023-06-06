// Created by moisrex on 6/2/23.
#include "../webpp/io/io_concepts.hpp"
#include "common_pch.hpp"

#include <array>
#include <sstream>

using namespace webpp;
using namespace std;


TEST(IO, IOConcepts) {
    //
}


TEST(IO, BasicIdea) {
    io_service   io;
    stringstream fake_file;
    fake_file << "content";

    array<char, 100> buf{};
    io.read(fake_file, buf.data(), buf.size()) >>
        [&buf]() {
            string const str{buf.data(), ::strlen(buf.data())};
            EXPECT_EQ(str, "content");
            return str;
        } >>
        io.write(fake_file) >>
        [&] {
            EXPECT_EQ(fake_file.str(), "content");
        } ||
      [](auto io) {
          throw io.error();
      };
}


TEST(IO, IOTraits) {
    //
}
