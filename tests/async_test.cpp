// Created by moisrex on 7/22/23.

#include "../webpp/async/async_concepts.hpp"
#include "../webpp/async/tasks.hpp"
#include "common/tests_common_pch.hpp"

#include <array>
#include <sstream>

using namespace webpp;
using namespace webpp::async;


TEST(IO, TaskChain) {
    auto chain = task() >> [] {
        return "Hello World.";
    } >> [](stl::string_view str) {
        return str.substr(str.find(' '));
    };
    EXPECT_EQ(chain(), "Hello");
}


struct custom_yeilder {
  private:
    stl::string_view data  = "Hello World";
    stl::size_t      index = 0;

  public:
    char yeilder() {
        return data[index++];
    }

    [[nodiscard]] bool is_done() const noexcept {
        return index == data.size();
    }
};

struct custom_consumer {
  private:
    stl::string data;

  public:
    void resume(char ch) {
        data.push_back(ch);
    }

    [[nodiscard]] stl::string_view operator()() const noexcept {
        return {data.data(), data.size()};
    }
};

TEST(IO, YeildingTest) {
    auto chain = task() >> custom_yeilder() >> custom_consumer();
    EXPECT_eq(chain(), "Hello World");
}
