#include "../webpp/socket/socket.hpp"
#include "common/tests_common_pch.hpp"

using namespace webpp;

inline constexpr in_port_t test_port   = 12'332;
constexpr auto             test_handle = basic_socket::native_handle_type(3);

TEST(SocketTest, DefaultCtor) {
    basic_socket const sock;

    EXPECT_FALSE(sock);
    EXPECT_FALSE(sock.is_open());
    EXPECT_EQ(sock.native_handle(), basic_socket::invalid_handle_value);
    EXPECT_EQ(sock.last_error(), 0);
}

TEST(SocketTest, HandleCtor) {
    basic_socket const sock(test_handle);

    EXPECT_TRUE(sock);
    EXPECT_TRUE(sock.is_open());
    EXPECT_EQ(sock.native_handle(), test_handle);
    EXPECT_EQ(sock.last_error(), 0);
}

TEST(SocketTest, MoveCtor) {
    basic_socket       org_sock(test_handle);
    basic_socket const sock(std::move(org_sock));

    // Make sure the new socket got the handle
    EXPECT_TRUE(sock);
    EXPECT_EQ(sock.native_handle(), test_handle);
    EXPECT_EQ(sock.last_error(), 0);

    // Make sure the handle was moved out of the org_sock
    EXPECT_FALSE(org_sock); // NOLINT(bugprone-use-after-move)
    EXPECT_EQ(org_sock.native_handle(), basic_socket::invalid_handle_value);
}

TEST(SocketTest, BasicErrorsTest) {
    basic_socket sock;

    // Operations on an unopened socket should give an error
    int        reuse = 1;
    socklen_t  len   = sizeof(int);
    bool const ok    = sock.get_option(SOL_SOCKET, SO_REUSEADDR, &reuse, &len);

    // Socket should be in error state
    EXPECT_FALSE(ok);
    EXPECT_FALSE(sock);

    int const err = sock.last_error();
    EXPECT_NE(err, 0);

    // last_error() is sticky, unlike `errno`
    EXPECT_EQ(sock.last_error(), err);

    // We can clear the error
    sock.clear_error();
    EXPECT_EQ(sock.last_error(), 0);

    // Test arbitrary clear value
    sock.clear_error(42);
    EXPECT_EQ(sock.last_error(), 42);
    EXPECT_FALSE(sock);
}

TEST(SocketTest, ClearErrorTest) {
    auto sock = basic_socket(AF_INET, SOCK_STREAM);
    EXPECT_TRUE(sock);

    sock.clear_error(42);
    EXPECT_FALSE(sock);

    sock.clear_error();
    EXPECT_TRUE(sock);
}

TEST(SocketTest, Release) {
    basic_socket sock(test_handle);

    EXPECT_EQ(sock.native_handle(), test_handle);
    EXPECT_EQ(sock.release(), test_handle);

    // Make sure the handle was moved out of the sock
    EXPECT_FALSE(sock);
    EXPECT_EQ(sock.native_handle(), basic_socket::invalid_handle_value);
}

TEST(SocketTest, Reset) {
    basic_socket sock(test_handle);
    EXPECT_TRUE(sock.native_handle() == test_handle);

    sock.reset(); // Default reset acts like release w/o return

    // Make sure the handle was moved out of the sock
    EXPECT_TRUE(!sock);
    EXPECT_EQ(sock.native_handle(), basic_socket::invalid_handle_value);

    // Now reset with a "valid" handle
    sock = test_handle;
    EXPECT_TRUE(sock);
    EXPECT_EQ(sock.native_handle(), test_handle);
}

TEST(SocketTest, FamilyOfUninitializedSocket) {
    // Uninitialized socket should have unspecified family
    basic_socket const sock;
    EXPECT_EQ(sock.family(), AF_UNSPEC);
}

TEST(SocketTest, FamilyOfBoundSocket) {
    // Bound socket should have same family as
    // address to which it's bound
    auto       sock = basic_socket(AF_INET, SOCK_STREAM);
    ipv4 const addr;

    int const reuse = 1;
    EXPECT_TRUE(sock.set_option(SOL_SOCKET, SO_REUSEADDR, reuse));
    EXPECT_TRUE(sock.bind(addr, test_port));
}

TEST(SocketTest, AddressOfUninitializedSocket) {
    // Uninitialized socket should have invalid address
    basic_socket const sock;
    EXPECT_EQ(sock.address(), sock_address_any::invalid());
}

// The address has the specified family but all zeros
TEST(SocketTest, AddressOfNonBoundSocket) {
    auto                   sock = basic_socket(AF_INET, SOCK_STREAM);
    sock_address_any const zero_addr{};
    EXPECT_TRUE(zero_addr.is_valid());
    EXPECT_TRUE(zero_addr.operator ipv4().is_zero());
    EXPECT_FALSE(zero_addr.operator ipv4().is_valid()); // family of zero_addr is AF_UNSPEC
    EXPECT_NE(sock.address(), zero_addr)
      << ip_address{sock.address()}.status_string() << " != " << ip_address{zero_addr}.status_string();
}

TEST(SocketTest, AddressOfBoundSocket) {
    // Bound socket should have same family as address to which it's bound
    auto       sock = basic_socket(AF_INET, SOCK_STREAM);
    const ipv4 addr;

    int const reuse = 1;
    EXPECT_TRUE(sock.set_option(SOL_SOCKET, SO_REUSEADDR, reuse));

    EXPECT_TRUE(sock.bind(addr, test_port));
    EXPECT_TRUE(ipv4{sock.address()}.is_valid());
    EXPECT_TRUE(addr.is_valid());
    EXPECT_TRUE(sock.is_error_free());
    EXPECT_EQ(ipv4{sock.address()}, addr) << ipv4{sock.address()}.string() << " != " << addr.string();
}

TEST(SocketTest, SocketNonBlockingMode) {
    auto sock = basic_socket(AF_INET, SOCK_STREAM);

#ifndef _WIN32
    EXPECT_TRUE(!sock.is_non_blocking());
#endif

    EXPECT_TRUE(sock.set_non_blocking());
#ifndef _WIN32
    EXPECT_TRUE(sock.is_non_blocking());
#endif

    EXPECT_TRUE(sock.set_non_blocking(false));
#ifndef _WIN32
    EXPECT_TRUE(!sock.is_non_blocking());
#endif
}
