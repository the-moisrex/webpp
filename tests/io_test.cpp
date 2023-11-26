// Created by moisrex on 6/2/23.
#include "../webpp/io/io_concepts.hpp"
// #include "../webpp/io/io_task.hpp"
#include "../webpp/io/buffer.hpp"
#include "../webpp/io/file_options.hpp"
#include "../webpp/io/io_uring/io_uring.hpp"
#include "../webpp/io/open.hpp"
#include "common/tests_common_pch.hpp"

#include <array>
#include <sstream>

using namespace webpp;
using namespace webpp::io;

TEST(IO, FileOptionsTest) {
    file_options const options = "r+";
    EXPECT_EQ(options, "r+");
    EXPECT_TRUE(options.is_writeable());
    EXPECT_FALSE(options.is_readonly());
}

TEST(IO, IOConcepts) {
#ifdef WEBPP_IO_URING_SUPPORT
    EXPECT_TRUE(IOService<basic_io_uring_service<>>);
#endif
}


#ifdef WEBPP_IO_URING_SUPPORT

TEST(IO, BasicIOUring) {
    managed_io_uring_service<> io;

    ASSERT_TRUE(io.is_success());

    std::array<char, 100> buf{};

    int executed = 0;

    auto                   file = io::open(io); // open temp file
    std::string_view const data = "this is a text";
    static_assert(
      stl::tag_invocable<syscall_write,
                         managed_io_uring_service<>&,
                         io::file_handle,
                         buffer_view,
                         stl::size_t,
                         std::function<void(io_result)>>,
      "Should be invocable");
    io::syscall(
      syscall_write{},
      io,
      file,
      buffer_view{reinterpret_cast<stl::byte const*>(data.data()), data.size()},
      0ull,
      stl::function<void(io_result)>{[data, &executed](io_result result) {
          ++executed;
          EXPECT_TRUE(result.is_ok());
          EXPECT_FALSE(result.is_error()) << result.to_string();
          EXPECT_EQ(data.size(), result.value());
      }});
    io::syscall(
      syscall_read{},
      io,
      file,
      buffer_span{reinterpret_cast<stl::byte*>(buf.data()), buf.size()},
      0ull,
      [data, &buf, &executed](io_result result) {
          ++executed;
          EXPECT_TRUE(result.is_ok());
          std::string_view const buf_value{buf.data(), static_cast<stl::size_t>(result.value())};
          EXPECT_EQ(data, buf_value);
      });
    io::syscall(syscall_close{}, io, file);
    // io::syscall(syscall_remove{}, io, file);

    io(2);

    EXPECT_EQ(executed, 2) << "Some of the callback functions didn't run";
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
