// Created by moisrex on 7/22/23.

#include "../webpp/async/async_concepts.hpp"
#include "../webpp/async/run_loop.hpp"
#include "../webpp/async/task_chain.hpp"
#include "common/tests_common_pch.hpp"

#include <array>
#include <sstream>

using namespace webpp;
using namespace webpp::async;

static_assert(ExecutionContext<basic_run_loop<>>, "A Run Loop is an execution context");
static_assert(Task<decltype([] {})>, "A lambda is a task");

TEST(AsyncTest, TaskChain) {
    auto chain = task_chain() >> [] {
        return "Hello World.";
    } >> [](stl::string_view str) {
        return str.substr(str.find(' '));
    };
    EXPECT_EQ(chain(), "Hello");
}


struct custom_yielder {
  private:
    stl::string_view data = "Hello World";

  public:
    [[nodiscard]] auto begin() const noexcept {
        return data.begin();
    }
    [[nodiscard]] auto end() const noexcept {
        return data.end();
    }
};

struct custom_consumer {
  private:
    stl::string data;

  public:
    template <typename Iter>
    [[nodiscard]] stl::string_view operator()(Iter beg, Iter end) const noexcept {
        return {beg, end};
    }
};

TEST(AsyncTest, YeildingTest) {
    auto chain = task_chain() >> custom_yielder() >> custom_consumer();
    EXPECT_EQ(chain(), "Hello World");
}
