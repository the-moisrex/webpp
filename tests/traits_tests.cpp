// Created by moisrex on 12/17/20.

#include "../core/include/webpp/traits/enable_traits.hpp"
#include "../core/include/webpp/traits/default_traits.hpp"
#include "../core/include/webpp/std/concepts.hpp"

using namespace webpp;
using namespace webpp::stl;
using namespace webpp::istl;

static_assert(requires{
  requires copy_constructible<enable_traits<default_traits>>;
});

