// Created by moisrex on 12/17/20.

#include "../core/include/webpp/std/concepts.hpp"
#include "../core/include/webpp/traits/default_traits.hpp"
#include "../core/include/webpp/traits/enable_traits.hpp"

using namespace webpp;
using namespace webpp::stl;
using namespace webpp::istl;

using def = enable_traits<default_traits>;

static_assert(requires(def et) { def{et}; });

static_assert(requires {
    requires is_convertible_v<def, def>;
    requires copy_constructible<def>;
});
