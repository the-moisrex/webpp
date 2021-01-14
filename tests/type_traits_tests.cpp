// Created by moisrex on 12/17/20.

#include "../core/include/webpp/std/type_traits.hpp"
#include "../core/include/webpp/std/tuple.hpp"
#include <string>
#include <memory_resource>

using namespace std;
using namespace webpp::istl;

///////////////////////////////////////////// replace_parameter /////////////////////////////////////////////

///////////////////////////////////////////
// changing std::string's allocator ///////
///////////////////////////////////////////

using new_str_t = replace_parameter<
  string,
  std::allocator<char>,
  pmr::polymorphic_allocator<char>
>;

static_assert(std::is_same_v<new_str_t, pmr::string>);


using should_be_void = replace_parameter<
  string,
  std::allocator<int>,
  pmr::polymorphic_allocator<char>
>;

//static_assert(is_void_v<should_be_void>);



///////////////////// Tuple ///////////////////


template <typename ...T>
struct fake_tuple {};

// tuple_filter
static_assert(is_same_v<
  filter_parameters_t<is_void, fake_tuple<int, void, int, void, double, void>>,
  fake_tuple<void, void, void>
>);


using t1 = tuple<int, string, vector<int>, int>;
using t2 = replace_parameter<t1, int, double>;
using t3 = recursively_replace_parameter<t1, int, double>;
static_assert(is_same_v<t2, tuple<double, string, vector<int>, double>>);
static_assert(is_same_v<t3, tuple<double, string, vector<double>, double>>);



