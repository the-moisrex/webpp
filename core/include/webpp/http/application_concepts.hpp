// Created by moisrex on 5/25/20.

#ifndef WEBPP_APPLICATION_CONCEPTS_H
#define WEBPP_APPLICATION_CONCEPTS_H

#include "../std/concepts.hpp"
#include "./request_concepts.hpp"
#include "./response_concepts.hpp"
#include "./routes/context.hpp"

namespace webpp {

    // todo: change this
    template <typename T>
    concept Application =
      stl::default_initializable<T> /*&& requires(T app) {
{ app(fake_request_type{}) }
->Response;
}*/;


    template <typename T, typename LoggerType, typename AllocType>
    concept ConstructibleWithLoggerAndAllocator = requires(AllocType const& alloc, LoggerType logger) {
        T{logger, alloc};
    };

    template <typename T, typename AllocType>
    concept ConstructibleWithAllocator = requires(AllocType const& alloc) {
        T{alloc};
    };

    template <typename T, typename LoggerType>
    concept ConstructibleWithLogger = requires(LoggerType logger) {
        T{logger};
    };

} // namespace webpp

#endif // WEBPP_APPLICATION_CONCEPTS_H
