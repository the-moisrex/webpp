// Created by moisrex on 10/8/20.

#ifndef WEBPP_HEADER_CONCEPTS_HPP
#define WEBPP_HEADER_CONCEPTS_HPP

namespace webpp::http {

    template <typename T>
    concept Header = requires(T h) {
        T{""};
        h.is_valid();
        static_cast<bool>(h); // is_valid
    };

} // namespace webpp::http

#endif // WEBPP_HEADER_CONCEPTS_HPP
