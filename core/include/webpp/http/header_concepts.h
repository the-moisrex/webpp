// Created by moisrex on 5/28/20.

#ifndef WEBPP_HEADER_CONCEPTS_H
#define WEBPP_HEADER_CONCEPTS_H

#include "../extensions/extension.h"

namespace webpp {

    template <typename T>
    concept Header = ;

    template <typename T>
    concept HeaderField = ;

    template <typename T>
    concept HeaderExtension = ;

    template <typename T>
    concept HeaderFieldExtension = ;

} // namespace webpp

#endif // WEBPP_HEADER_CONCEPTS_H
