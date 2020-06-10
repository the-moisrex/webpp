// Created by moisrex on 6/1/20.

#ifndef WEBPP_OPTIONAL_H
#define WEBPP_OPTIONAL_H


#include "./std.h"

// from example here: https://eel.is/c++draft/cpp.cond#15
// todo: we can improve this more:

#if __has_include(<optional>)
#    include <optional>
#    if __cpp_lib_optional >= 201603
#        define have_optional 1
#    endif
#elif __has_include(<experimental/optional>)
#    include <experimental/optional>
#    if __cpp_lib_experimental_optional >= 201411
#        define have_optional         1
#        define experimental_optional 1
namespace webpp::stl {
    using namespace ::std::experimental;
}
#    endif
#endif

#ifndef have_optional
#    define have_optional 0
#    error "There's no <optional>" // todo: check if we need this
#endif

#endif // WEBPP_OPTIONAL_H
