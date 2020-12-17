// Created by moisrex on 12/17/20.

#include "../core/include/webpp/std/type_traits.hpp"
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

static_assert(is_void_v<should_be_void>);




