// Created by moisrex on 12/17/20.

#include "../webpp/std/concepts.hpp"
#include "../webpp/traits/default_traits.hpp"
#include "../webpp/traits/enable_traits.hpp"

using namespace webpp;
using namespace webpp::stl;
using namespace webpp::istl;

using def = enable_traits<default_traits>;

static_assert(requires(def etraits) { def{etraits}; });
static_assert(is_convertible_v<def, def>);
static_assert(copy_constructible<def>);
static_assert(is_constructible_v<def, def&>);
// static_assert(stl::is_same_v<traits::generalify_allocators<default_traits, int>, int>);


using def2 = enable_traits_with<default_traits, nothing_type>;

static_assert(requires(def2 etraits) { def2{etraits}; });
static_assert(is_convertible_v<def2, def2>);
static_assert(copy_constructible<def2>);
static_assert(is_constructible_v<def2, def2&>);
// static_assert(stl::is_same_v<traits::generalify_allocators<default_traits, int>, int>);
