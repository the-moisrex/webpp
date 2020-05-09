// Created by moisrex on 5/9/20.

#ifndef WEBPP_BASIC_INTERFACE_CONCEPTS_H
#define WEBPP_BASIC_INTERFACE_CONCEPTS_H

namespace webpp {

    template <typename T>
    concept Interface = requires(T u) {
        { u.operator()() }
        noexcept;
    };

} // namespace webpp

#endif // WEBPP_BASIC_INTERFACE_CONCEPTS_H
