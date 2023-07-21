// Created by moisrex on 10/7/21.

#ifndef WEBPP_STD_ITERATOR_HPP
#define WEBPP_STD_ITERATOR_HPP

#include "concepts.hpp"

#include <iterator>

#if !__cpp_lib_concepts
#    include "./iterator_impl.hpp"
#endif // cpp_lib_concepts


namespace webpp::istl {

    template <typename T>
    concept Iterable = requires(T iter) {
                           iter.begin();
                           iter.end();
                       };
}

#endif // WEBPP_STD_ITERATOR_HPP
