// Created by moisrex on 10/8/20.

#ifndef WEBPP_HEADER_CONCEPTS_HPP
#define WEBPP_HEADER_CONCEPTS_HPP

namespace webpp::http {

    template <typename T>
    concept Header = requires(T h) {
                         T{"Content-Type: string"};
                         T::versions;
                         h.parse();
                         h.is_valid();
                     };

} // namespace webpp::http

#endif // WEBPP_HEADER_CONCEPTS_HPP
