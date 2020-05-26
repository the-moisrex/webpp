// Created by moisrex on 5/25/20.

#ifndef WEBPP_APPLICATION_CONCEPTS_H
#define WEBPP_APPLICATION_CONCEPTS_H

#include "../std/concepts.h"

namespace webpp {

    template <typename T>
    concept Application = ::std::default_initializable<T>;

}

#endif // WEBPP_APPLICATION_CONCEPTS_H
