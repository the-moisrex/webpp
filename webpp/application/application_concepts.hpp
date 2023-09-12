// Created by moisrex on 5/25/20.

#ifndef WEBPP_APPLICATION_CONCEPTS_H
#define WEBPP_APPLICATION_CONCEPTS_H

#include "../std/concepts.hpp"

namespace webpp {

    template <typename T>
    concept Application = stl::default_initializable<T> || stl::is_copy_constructible_v<T>;


    template <typename T, typename LoggerType, typename AllocType>
    concept ConstructibleWithLoggerAndAllocator = requires(AllocType const& alloc, LoggerType logger) {
        T{logger, alloc};
    };

    template <typename T, typename AllocType>
    concept ConstructibleWithAllocator = requires(AllocType const& alloc) { T{alloc}; };

    template <typename T, typename LoggerType>
    concept ConstructibleWithLogger = requires(LoggerType logger) { T{logger}; };

    template <typename T>
    concept ApplicationWrapper = Application<stl::remove_cvref_t<T>>;

} // namespace webpp

#endif // WEBPP_APPLICATION_CONCEPTS_H
