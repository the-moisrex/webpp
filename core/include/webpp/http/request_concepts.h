//
// Created by moisrex on 5/9/20.
//

#ifndef WEBPP_REQUEST_CONCEPTS_H
#define WEBPP_REQUEST_CONCEPTS_H

#include "../traits/traits_concepts.h"
#include "./interfaces/basic_interface_concepts.h"

namespace webpp {
    template <typename T>
    concept Request = requires(T x) {
        typename T::traits;
        Traits<typename T::traits>;

        typename T::interface;
        Interface<typename T::interface>;
    };
} // namespace webpp

#endif // WEBPP_REQUEST_CONCEPTS_H
