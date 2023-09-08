// Created by moisrex on 6/2/23.
#include "../webpp/io/io_concepts.hpp"
// #include "../webpp/io/io_task.hpp"
#include "../webpp/io/file_options.hpp"
#include "../webpp/io/io_uring/io_uring.hpp"
#include "common/tests_common_pch.hpp"

#include <array>
#include <sstream>

using namespace webpp;
using namespace webpp::io;

TEST(IO, FileOptionsTest) {
    file_options options = "rw";
}

TEST(IO, IOConcepts) {
#ifdef WEBPP_IO_URING_SUPPORT
    EXPECT_TRUE(IOService<io_uring_service<>>);
#endif
}


#ifdef WEBPP_IO_URING_SUPPORT

TEST(IO, BasicIOUring) {
    io_uring_service<> io;

    std::array<char, 100> buf{};

    auto                   file = io.open(); // open temp file
    std::string_view const data = "this is a text";
    io.write(file, data.data(), data.size(), [data](int result) {
        ASSERT_NE(result, -1);
        ASSERT_EQ(result, data.size());
    });
    io.read(file, buf.data(), buf.size(), [data, &buf](int result) {
        ASSERT_NE(result, -1);
        std::string_view const buf_value{buf.data(), static_cast<stl::size_t>(result)};
        EXPECT_EQ(data, buf_value);
    });
    io.close(file);
    io.remove(file);
}

#    if 0
TEST(IO, BasicIdea) {
    io_uring_service<> io;
    std::stringstream  fake_file;
    fake_file << "content";

    std::array<char, 100> buf{};
    io >> read(fake_file, buf.data(), buf.size()) >>
        [&buf]() {
            std::string const str{buf.data(), ::strlen(buf.data())};
            EXPECT_EQ(str, "content");
            return str;
        } >>
        write(fake_file) >>
        [&] {
            EXPECT_EQ(fake_file.str(), "content");
        } ||
      [](auto io) {
          throw io.error();
      };
}


TEST(IO, BasicIdea2) {
    io_uring_service  io;
    std::stringstream fake_file;
    fake_file << "content";

    io >> read(fake_file) >>
        [](auto buf) {
            std::string const str{buf.data(), ::strlen(buf.data())};
            EXPECT_EQ(str, "content");
            return str;
        } >>
        write(fake_file) >>
        [&] {
            EXPECT_EQ(fake_file.str(), "content");
        } ||
      [](auto io) {
          throw io.error();
      };
}
#    endif // 0

#endif

TEST(IO, IOBuffersTest) {
    buffer_manager buf_man;
}

TEST(IO, IOTraits) {
    //
}
