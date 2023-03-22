// Created by moisrex on 12/10/19.

#include "../webpp/std/tuple.hpp"

#include "common_pch.hpp"



using namespace webpp;
using namespace webpp::istl;

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)


// The test fixture
class TupleTest : public ::testing::Test {
  protected:
    void SetUp() override {
        test_tuple_ = stl::make_tuple(1, 2, 3, 4);
    }

    // Member variables
    stl::tuple<int, int, int, int> test_tuple_;
};

// The tests
TEST_F(TupleTest, AdjacentApplyHelper) {
    int result = 0;

    // Lambda function to test
    auto add_elements = [&result](const auto& x, const auto& y) {
        result += x + y;
    };

    // Call the template function to test
    adjacent_apply(test_tuple_, add_elements);

    EXPECT_EQ(result, 1 + 2 + 2 + 3 + 3 + 4);
}

TEST_F(TupleTest, AdjacentApply) {
    int result = 0;

    // Lambda function to test
    auto subtract_elements = [&result](const auto& x, const auto& y) {
        result += (x - y);
    };

    // Call the template function to test
    adjacent_apply(test_tuple_, subtract_elements);

    EXPECT_EQ(result, -1 - 1 - 1);
}

TEST_F(TupleTest, AdjacentApplySmallTuple) {
    stl::tuple<int, int> small_tuple = stl::make_tuple(1, 2);
    bool                 was_called  = false;

    // Lambda function to test
    auto was_called_func = [&was_called](const auto&, const auto&) {
        was_called = true;
    };

    // Call the template function to test
    adjacent_apply(small_tuple, was_called_func);

    ASSERT_TRUE(was_called); // Make sure the lambda was actually called
}

TEST_F(TupleTest, AdjacentApplyTwoElementsTuple) {
    stl::tuple<int, int> two_elements_tuple = stl::make_tuple(1, 2);

    // Lambda function to test
    auto identity_func = [](const auto& x, const auto& y) {
        EXPECT_EQ(x, 1);
        EXPECT_EQ(y, 2);
    };

    // Call the template function to test
    adjacent_apply(two_elements_tuple, identity_func);
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
