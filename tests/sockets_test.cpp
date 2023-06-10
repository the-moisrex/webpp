#include "../webpp/socket/socket.hpp"
#include "common_pch.hpp"

using namespace webpp;

TEST(SocketTest, Init) {}

TEST(SocketTest, DefaultCtor) {
    basic_socket sock;

    EXPECT_FALSE(sock);
    EXPECT_FALSE(sock.is_open());
    EXPECT_EQ(sock.native_handle(), basic_socket::invalid_handle_value);
    EXPECT_EQ(sock.last_error(), 0);
}

TEST(SocketTest, HandleCtor) {
    constexpr auto HANDLE = basic_socket::native_handle_type(3);
    basic_socket   sock(HANDLE);

    EXPECT_TRUE(sock);
    EXPECT_TRUE(sock.is_open());
    EXPECT_EQ(sock.native_handle(), HANDLE);
    EXPECT_EQ(sock.last_error(), 0);
}


TEST(SocketTest, MoveCtor) {
    constexpr auto HANDLE = basic_socket::native_handle_type(3);
    basic_socket   org_sock(HANDLE);

    basic_socket sock(std::move(org_sock));

    // Make sure the new socket got the handle
    EXPECT_TRUE(sock);
    EXPECT_EQ(sock.native_handle(), HANDLE);
    EXPECT_EQ(sock.last_error(), 0);

    // Make sure the handle was moved out of the org_sock
    EXPECT_FALSE(org_sock);
    EXPECT_EQ(org_sock.native_handle(), basic_socket::invalid_handle_value);
}
