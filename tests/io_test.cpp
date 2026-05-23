// Created by moisrex on 6/2/23.
#include "../webpp/io/file_options.hpp"
#include "../webpp/io/io_uring/io_uring.hpp"
#include "common/test.hpp"

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

using namespace webpp;
using namespace webpp::io;


// Verify concept compliance at compile time
static_assert(OperationHandle<io_uring_operation_handle>);
static_assert(CompletionToken<io_uring_completion_token>);
static_assert(IOBackend<io_uring_backend>);
static_assert(ReadableBackend<io_uring_backend>);
static_assert(WritableBackend<io_uring_backend>);
static_assert(AcceptableBackend<io_uring_backend>);
static_assert(ConnectableBackend<io_uring_backend>);
static_assert(ClosableBackend<io_uring_backend>);
static_assert(FullIOBackend<io_uring_backend>);

// static_assert(ChainableBackend<io_uring_backend>);


TEST(IO, FileOptionsTest) {
    file_options const options{"r+"};
    EXPECT_EQ(options, "r+");
    EXPECT_TRUE(options.is_writeable());
    EXPECT_FALSE(options.is_readonly());
}

// ============================================================================
// Test Fixtures
// ============================================================================

class IOUringBackendTest : public ::testing::Test {
  protected:
    io_uring_backend backend;

    void SetUp() override {
        backend = io_uring_backend::create(256, 0);
        ASSERT_TRUE(backend.is_valid()) << "Failed to initialize io_uring backend";
    }

    void TearDown() override {
        // Backend cleanup is automatic via RAII
    }

    // Helper: Create a temporary file for testing
    io_handle create_temp_file() {
        char temp_name[] = "/tmp/webpp_test_XXXXXX";
        int  fd          = mkstemp(temp_name);
        if (fd < 0) {
            return io_handle::invalid(errno);
        }
        unlink(temp_name); // Delete on close
        return io_handle{fd};
    }

    // Helper: Create a socket pair for testing
    std::pair<io_handle, io_handle> create_socket_pair() {
        int fds[2];
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) < 0) {
            return {io_handle::invalid(errno), io_handle::invalid(errno)};
        }
        return {io_handle{fds[0]}, io_handle{fds[1]}};
    }

    // Helper: Close file descriptor
    void close_fd(io_handle fd) {
        if (fd.is_valid()) {
            close(fd.native_handle());
        }
    }
};

// ============================================================================
// io_result Tests
// ============================================================================

TEST(IOResultTest, DefaultConstructor) {
    io_result result;
    EXPECT_TRUE(result.is_ok());
    EXPECT_FALSE(result.is_error());
    EXPECT_EQ(result.value(), 0);
}

TEST(IOResultTest, SuccessValue) {
    io_result result{42};
    EXPECT_TRUE(result.is_ok());
    EXPECT_FALSE(result.is_error());
    EXPECT_EQ(result.value(), 42);
    EXPECT_TRUE(static_cast<bool>(result));
}

TEST(IOResultTest, ErrorFromErrno) {
    errno = ENOENT;
    io_result result{-1};
    EXPECT_FALSE(result.is_ok());
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error(), ENOENT);
    EXPECT_FALSE(static_cast<bool>(result));
}

TEST(IOResultTest, InvalidResult) {
    io_result result = io_result::invalid(EINVAL);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error(), EINVAL);
}

TEST(IOResultTest, SetError) {
    io_result result{10};
    EXPECT_TRUE(result.is_ok());

    result.set_error(EACCES);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error(), EACCES);
}

TEST(IOResultTest, ErrorCodeConversion) {
    io_result       result = io_result::invalid(EPERM);
    std::error_code ec     = result.error_code();

    EXPECT_EQ(ec.value(), EPERM);
    EXPECT_EQ(ec.category(), std::generic_category());
}

TEST(IOResultTest, ErrorCodeImplicitConversion) {
    io_result       result = io_result::invalid(ENOENT);
    std::error_code ec     = result;

    EXPECT_EQ(ec.value(), ENOENT);
}

TEST(IOResultTest, ToStringSuccess) {
    io_result   result{0};
    std::string str = result.to_string<std::string>();
    EXPECT_TRUE(str.empty());
}

TEST(IOResultTest, ToStringError) {
    io_result   result = io_result::invalid(ENOENT);
    std::string str    = result.to_string<std::string>();
    EXPECT_FALSE(str.empty());
    EXPECT_NE(str.find("No such file"), std::string::npos);
}

// ============================================================================
// io_handle Tests
// ============================================================================

TEST(FileHandleTest, DefaultConstructor) {
    io_handle fh;
    EXPECT_FALSE(fh.is_valid());
    EXPECT_FALSE(static_cast<bool>(fh));
}

TEST(FileHandleTest, ValidHandle) {
    io_handle fh{5};
    EXPECT_TRUE(fh.is_valid());
    EXPECT_TRUE(static_cast<bool>(fh));
    EXPECT_EQ(fh.native_handle(), 5);
}

TEST(FileHandleTest, InvalidHandle) {
    io_handle fh = io_handle::invalid(EBADF);
    EXPECT_FALSE(fh.is_valid());
    EXPECT_EQ(fh.native_handle(), -EBADF);
}

TEST(FileHandleTest, CheckValidHandle) {
    io_handle fh = io_handle::check(10);
    EXPECT_TRUE(fh.is_valid());
    EXPECT_EQ(fh.native_handle(), 10);
}

TEST(FileHandleTest, CheckInvalidHandle) {
    errno        = EINVAL;
    io_handle fh = io_handle::check(-1);
    EXPECT_FALSE(fh.is_valid());
}

TEST(FileHandleTest, Comparison) {
    io_handle fh1{5};
    io_handle fh2{5};
    io_handle fh3{10};

    EXPECT_EQ(fh1, fh2);
    EXPECT_NE(fh1, fh3);
    EXPECT_LT(fh1, fh3);
    EXPECT_GT(fh3, fh1);
}

TEST(FileHandleTest, Assignment) {
    io_handle fh1{5};
    io_handle fh2;

    fh2 = fh1;
    EXPECT_EQ(fh1, fh2);

    fh2 = 10;
    EXPECT_EQ(fh2.native_handle(), 10);
}

TEST(FileHandleTest, ToStringError) {
    io_handle   fh  = io_handle::invalid(EACCES);
    std::string str = fh.to_string<std::string>();
    EXPECT_FALSE(str.empty());
}

// ============================================================================
// io_uring_operation_handle Tests
// ============================================================================

TEST(IOUringOperationHandleTest, DefaultConstructor) {
    io_uring_operation_handle handle;
    EXPECT_FALSE(handle.is_valid());
    EXPECT_FALSE(handle.is_cancelled());
}

TEST(IOUringOperationHandleTest, ValidHandle) {
    struct io_uring ring;
    io_uring_queue_init(8, &ring, 0);

    void*                     user_data = reinterpret_cast<void*>(0x1234);
    io_uring_operation_handle handle{&ring, user_data};

    EXPECT_TRUE(handle.is_valid());
    EXPECT_FALSE(handle.is_cancelled());

    handle.cancel(); // or handle = {}; to move-assign empty handle
    io_uring_queue_exit(&ring);
}

TEST(IOUringOperationHandleTest, MoveConstructor) {
    struct io_uring ring;
    io_uring_queue_init(8, &ring, 0);
    {
        io_uring_operation_handle handle1{&ring, nullptr};
        EXPECT_TRUE(handle1.is_valid());

        io_uring_operation_handle handle2{std::move(handle1)};
        EXPECT_FALSE(handle1.is_valid());
        EXPECT_TRUE(handle2.is_valid());
    }

    io_uring_queue_exit(&ring);
}

TEST(IOUringOperationHandleTest, MoveAssignment) {
    struct io_uring ring;
    io_uring_queue_init(8, &ring, 0);

    {
        io_uring_operation_handle handle1{&ring, nullptr};
        io_uring_operation_handle handle2;

        handle2 = std::move(handle1);
        EXPECT_FALSE(handle1.is_valid());
        EXPECT_TRUE(handle2.is_valid());
    }

    io_uring_queue_exit(&ring);
}

// ============================================================================
// io_uring_completion_token Tests
// ============================================================================

TEST(IOUringCompletionTokenTest, DefaultConstructor) {
    io_uring_completion_token token;
    EXPECT_TRUE(token.result().is_ok());
    EXPECT_EQ(token.data(), nullptr);
    EXPECT_EQ(token.size(), 0);
}

TEST(IOUringCompletionTokenTest, WithData) {
    void*                     user_data = reinterpret_cast<void*>(0x5678);
    io_uring_completion_token token{io_result{100}, user_data, 100};

    EXPECT_TRUE(token.result().is_ok());
    EXPECT_EQ(token.result().value(), 100);
    EXPECT_EQ(token.data(), user_data);
    EXPECT_EQ(token.size(), 100);
}

TEST(IOUringCompletionTokenTest, WithError) {
    io_uring_completion_token token{io_result::invalid(EIO), nullptr, 0};

    EXPECT_TRUE(token.result().is_error());
    EXPECT_EQ(token.result().error(), EIO);
    EXPECT_EQ(token.size(), 0);
}

// ============================================================================
// io_uring_backend Basic Tests
// ============================================================================

TEST_F(IOUringBackendTest, CreateBackend) {
    EXPECT_TRUE(backend.is_valid()) << backend.init_status().to_string();
    EXPECT_EQ(backend.pending_count(), 0);
}

TEST_F(IOUringBackendTest, CreateBackendWithCustomParams) {
    auto custom_backend = io_uring_backend::create(128, 0);
    EXPECT_TRUE(custom_backend.is_valid());
}

TEST_F(IOUringBackendTest, MoveConstructor) {
    auto backend2 = std::move(backend);
    EXPECT_FALSE(backend.is_valid());
    EXPECT_TRUE(backend2.is_valid());
}

TEST_F(IOUringBackendTest, MoveAssignment) {
    io_uring_backend backend2;
    backend2 = std::move(backend);
    EXPECT_FALSE(backend.is_valid());
    EXPECT_TRUE(backend2.is_valid());
}

TEST_F(IOUringBackendTest, SubmitWithNoPending) {
    io_result result = backend.submit();
    EXPECT_TRUE(result.is_ok());
    EXPECT_EQ(result.value(), 0);
}

// TEST_F(IOUringBackendTest, FlushWithNoPending) {
//     EXPECT_NO_THROW(backend.flush());
// }

// ============================================================================
// Read/Write Operations Tests
// ============================================================================

TEST_F(IOUringBackendTest, PrepareReadOperation) {
    io_handle fd = create_temp_file();
    ASSERT_TRUE(fd.is_valid());

    std::array<char, 64> buffer{};
    void*                user_data = reinterpret_cast<void*>(0x1111);

    {
        auto handle = prep_read(backend, fd, std::span{buffer}, user_data);
        EXPECT_TRUE(handle.is_valid());
        EXPECT_EQ(backend.pending_count(), 1);
    }

    close_fd(fd);
}

TEST_F(IOUringBackendTest, PrepareWriteOperation) {
    io_handle fd = create_temp_file();
    ASSERT_TRUE(fd.is_valid());

    std::array<char, 64> buffer{};
    std::memcpy(buffer.data(), "test data", 9);
    void* user_data = reinterpret_cast<void*>(0x2222);

    auto handle = prep_write(backend, fd, std::span<char const>{buffer}, user_data);
    EXPECT_TRUE(handle.is_valid());
    EXPECT_EQ(backend.pending_count(), 1);

    close_fd(fd);
}

TEST_F(IOUringBackendTest, WriteAndReadRoundtrip) {
    io_handle fd = create_temp_file();
    ASSERT_TRUE(fd.is_valid());

    // Write data
    std::array<char, 64> write_buffer{};
    char const*          test_data = "Hello, io_uring!";
    std::memcpy(write_buffer.data(), test_data, std::strlen(test_data));

    auto write_handle = prep_write_at(backend, fd, std::span<char const>{write_buffer}, 0, nullptr);
    EXPECT_TRUE(write_handle.is_valid());

    io_result submit_result = backend.submit();
    EXPECT_TRUE(submit_result.is_ok());

    auto write_token = backend.wait_one();
    EXPECT_TRUE(write_token.result().is_ok());
    EXPECT_GT(write_token.size(), 0);

    // Read data back
    std::array<char, 64> read_buffer{};
    auto                 read_handle = prep_read_at(backend, fd, std::span{read_buffer}, 0, nullptr);
    EXPECT_TRUE(read_handle.is_valid());

    submit_result = backend.submit();
    EXPECT_TRUE(submit_result.is_ok());

    auto read_token = backend.wait_one();
    EXPECT_TRUE(read_token.result().is_ok());
    EXPECT_EQ(read_token.size(), write_token.size());
    EXPECT_STREQ(read_buffer.data(), test_data);

    close_fd(fd);
}

TEST_F(IOUringBackendTest, ReadFromInvalidFd) {
    io_handle            invalid_fd{-1};
    std::array<char, 64> buffer{};

    auto handle = prep_read(backend, invalid_fd, std::span{buffer}, nullptr);
    EXPECT_TRUE(handle.is_valid());

    backend.submit();
    auto token = backend.wait_one();
    EXPECT_TRUE(token.result().is_error());
}

TEST_F(IOUringBackendTest, WriteToInvalidFd) {
    io_handle            invalid_fd{-1};
    std::array<char, 64> buffer{};

    auto handle = prep_write(backend, invalid_fd, std::span<char const>{buffer}, nullptr);
    EXPECT_TRUE(handle.is_valid());

    backend.submit();
    auto token = backend.wait_one();
    EXPECT_TRUE(token.result().is_error());
}

TEST_F(IOUringBackendTest, MultipleOperationsBatch) {
    io_handle fd = create_temp_file();
    ASSERT_TRUE(fd.is_valid());

    std::array<char, 32> buffer1{};
    std::array<char, 32> buffer2{};
    std::array<char, 32> buffer3{};

    std::memcpy(buffer1.data(), "data1", 5);
    std::memcpy(buffer2.data(), "data2", 5);
    std::memcpy(buffer3.data(), "data3", 5);

    prep_write_at(backend, fd, std::span<char const>{buffer1}, 0, reinterpret_cast<void*>(1));
    prep_write_at(backend, fd, std::span<char const>{buffer2}, 32, reinterpret_cast<void*>(2));
    prep_write_at(backend, fd, std::span<char const>{buffer3}, 64, reinterpret_cast<void*>(3));

    EXPECT_EQ(backend.pending_count(), 3);

    backend.submit();

    std::array<io_uring_completion_token, 3> tokens;
    stl::size_t                              count = backend.wait_batch(std::span{tokens});
    EXPECT_EQ(count, 3);

    for (auto const& token : tokens) {
        EXPECT_TRUE(token.result().is_ok());
    }

    close_fd(fd);
}

// ============================================================================
// Socket Operations Tests
// ============================================================================

TEST_F(IOUringBackendTest, PrepareAcceptOperation) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_GE(listen_fd, 0);

    struct sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = 0;

    ASSERT_EQ(::bind(listen_fd, reinterpret_cast<struct ::sockaddr*>(&addr), sizeof(addr)), 0);
    ASSERT_EQ(listen(listen_fd, 5), 0);

    io_handle fd{listen_fd};
    auto      handle = prep_accept(backend, fd, nullptr);
    EXPECT_TRUE(handle.is_valid());
    EXPECT_EQ(backend.pending_count(), 1);

    close_fd(fd);
}

TEST_F(IOUringBackendTest, PrepareConnectOperation) {
    int sock_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    ASSERT_GE(sock_fd, 0);

    struct sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port        = htons(12'345);

    io_handle fd{sock_fd};
    auto      handle = prep_connect(backend, fd, &addr, sizeof(addr), nullptr);
    EXPECT_TRUE(handle.is_valid());
    EXPECT_EQ(backend.pending_count(), 1);

    close_fd(fd);
}

TEST_F(IOUringBackendTest, SocketPairCommunication) {
    auto [fd1, fd2] = create_socket_pair();
    ASSERT_TRUE(fd1.is_valid());
    ASSERT_TRUE(fd2.is_valid());

    // Write to fd1
    std::array<char, 32> write_buffer{};
    char const*          message = "socket test";
    std::memcpy(write_buffer.data(), message, std::strlen(message));

    prep_write(backend, fd1, std::span<char const>{write_buffer}, reinterpret_cast<void*>(1));
    backend.submit();

    auto write_token = backend.wait_one();
    EXPECT_TRUE(write_token.result().is_ok());

    // Read from fd2
    std::array<char, 32> read_buffer{};
    prep_read(backend, fd2, std::span{read_buffer}, reinterpret_cast<void*>(2));
    backend.submit();

    auto read_token = backend.wait_one();
    EXPECT_TRUE(read_token.result().is_ok());
    EXPECT_STREQ(read_buffer.data(), message);

    close_fd(fd1);
    close_fd(fd2);
}

// ============================================================================
// Close Operation Tests
// ============================================================================

TEST_F(IOUringBackendTest, PrepareCloseOperation) {
    io_handle fd = create_temp_file();
    ASSERT_TRUE(fd.is_valid());

    auto handle = prep_close(backend, fd, nullptr);
    EXPECT_TRUE(handle.is_valid());
    EXPECT_EQ(backend.pending_count(), 1);

    backend.submit();
    auto token = backend.wait_one();
    EXPECT_TRUE(token.result().is_ok());
}

// ============================================================================
// Polling Tests
// ============================================================================

TEST_F(IOUringBackendTest, PollOneWithNoCompletions) {
    auto token = backend.poll_one();
    EXPECT_TRUE(token.result().is_error());
    EXPECT_EQ(token.result().error(), EAGAIN);
}

TEST_F(IOUringBackendTest, PollOneWithCompletion) {
    io_handle fd = create_temp_file();
    ASSERT_TRUE(fd.is_valid());

    std::array<char, 32> buffer{};
    std::memcpy(buffer.data(), "poll test", 9);

    prep_write(backend, fd, std::span<char const>{buffer}, nullptr);
    backend.submit();

    // Give it a moment to complete
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    auto token = backend.poll_one();
    EXPECT_TRUE(token.result().is_ok());

    close_fd(fd);
}

// ============================================================================
// Submit and Wait Tests
// ============================================================================

TEST_F(IOUringBackendTest, SubmitAndWait) {
    io_handle fd = create_temp_file();
    ASSERT_TRUE(fd.is_valid());

    std::array<char, 32> buffer{};
    prep_write(backend, fd, std::span<char const>{buffer}, nullptr);

    io_result result = backend.submit_and_wait(1);
    EXPECT_TRUE(result.is_ok());

    auto token = backend.poll_one();
    EXPECT_TRUE(token.result().is_ok());

    close_fd(fd);
}

// ============================================================================
// User Data Tests
// ============================================================================

TEST_F(IOUringBackendTest, UserDataPreservation) {
    io_handle fd = create_temp_file();
    ASSERT_TRUE(fd.is_valid());

    struct UserContext {
        int         id;
        char const* name;
    };

    UserContext ctx{42, "test_context"};
    void*       user_data = &ctx;

    std::array<char, 32> buffer{};
    prep_write(backend, fd, std::span<char const>{buffer}, user_data);
    backend.submit();

    auto token = backend.wait_one();
    ASSERT_TRUE(token.result().is_ok());
    EXPECT_EQ(token.data(), user_data);

    auto* retrieved_ctx = static_cast<UserContext*>(token.data());
    EXPECT_EQ(retrieved_ctx->id, 42);
    EXPECT_STREQ(retrieved_ctx->name, "test_context");

    close_fd(fd);
}

// ============================================================================
// Edge Cases and Error Handling
// ============================================================================

TEST_F(IOUringBackendTest, EmptyBufferWrite) {
    io_handle fd = create_temp_file();
    ASSERT_TRUE(fd.is_valid());

    std::span<char const> empty_buffer;
    auto                  handle = prep_write(backend, fd, empty_buffer, nullptr);
    EXPECT_TRUE(handle.is_valid());

    backend.submit();
    auto token = backend.wait_one();
    EXPECT_TRUE(token.result().is_ok());
    EXPECT_EQ(token.size(), 0);

    close_fd(fd);
}

TEST_F(IOUringBackendTest, LargeBufferOperation) {
    io_handle fd = create_temp_file();
    ASSERT_TRUE(fd.is_valid());

    std::vector<char> large_buffer(1024 * 1024, 'A'); // 1MB
    auto              handle = prep_write(backend, fd, std::span<char const>{large_buffer}, nullptr);
    EXPECT_TRUE(handle.is_valid());

    backend.submit();
    auto token = backend.wait_one();
    EXPECT_TRUE(token.result().is_ok());
    EXPECT_EQ(token.size(), large_buffer.size());

    close_fd(fd);
}

TEST_F(IOUringBackendTest, WaitBatchWithEmptySpan) {
    std::span<io_uring_completion_token> empty_span;
    stl::size_t                          count = backend.wait_batch(empty_span);
    EXPECT_EQ(count, 0);
}

TEST_F(IOUringBackendTest, PendingCountTracking) {
    io_handle fd = create_temp_file();
    ASSERT_TRUE(fd.is_valid());

    EXPECT_EQ(backend.pending_count(), 0);

    std::array<char, 32> buffer{};
    prep_write(backend, fd, std::span<char const>{buffer}, nullptr);
    EXPECT_EQ(backend.pending_count(), 1);

    prep_write(backend, fd, std::span<char const>{buffer}, nullptr);
    EXPECT_EQ(backend.pending_count(), 2);

    backend.submit();
    backend.wait_one();
    EXPECT_EQ(backend.pending_count(), 1);

    backend.wait_one();
    EXPECT_EQ(backend.pending_count(), 0);

    close_fd(fd);
}

// ============================================================================
// Concept Verification Tests
// ============================================================================

TEST(IOConceptsTest, OperationHandleConcept) {
    EXPECT_TRUE(OperationHandle<io_uring_operation_handle>);
}

TEST(IOConceptsTest, CompletionTokenConcept) {
    EXPECT_TRUE(CompletionToken<io_uring_completion_token>);
}

TEST(IOConceptsTest, IOBackendConcept) {
    EXPECT_TRUE(IOBackend<io_uring_backend>);
}

TEST(IOConceptsTest, ReadableBackendConcept) {
    EXPECT_TRUE(ReadableBackend<io_uring_backend>);
}

TEST(IOConceptsTest, WritableBackendConcept) {
    EXPECT_TRUE(WritableBackend<io_uring_backend>);
}

TEST(IOConceptsTest, AcceptableBackendConcept) {
    EXPECT_TRUE(AcceptableBackend<io_uring_backend>);
}

TEST(IOConceptsTest, ConnectableBackendConcept) {
    EXPECT_TRUE(ConnectableBackend<io_uring_backend>);
}

TEST(IOConceptsTest, ClosableBackendConcept) {
    EXPECT_TRUE(ClosableBackend<io_uring_backend>);
}

TEST(IOConceptsTest, FullIOBackendConcept) {
    EXPECT_TRUE(FullIOBackend<io_uring_backend>);
}

// ============================================================================
// Chaining Operations Tests
// ============================================================================

// TEST_F(IOUringBackendTest, LinkNextOperation) {
//     io_handle fd = create_temp_file();
//     ASSERT_TRUE(fd.is_valid());

//     std::array<char, 32> buffer{};
//     auto                 handle = prep_write(backend, fd, std::span<char const>{buffer}, nullptr);
//     backend.link_next(handle);
//     close_fd(fd);
// }

// TEST_F(IOUringBackendTest, SetFlags) {
//     io_handle fd = create_temp_file();
//     ASSERT_TRUE(fd.is_valid());

//     std::array<char, 32> buffer{};
//     auto                 handle = prep_write(backend, fd, std::span<char const>{buffer}, nullptr);
//     backend.set_flags(handle, 0);
//     close_fd(fd);
// }
