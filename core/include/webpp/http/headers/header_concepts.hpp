// Created by moisrex on 10/8/20.

#ifndef WEBPP_HEADER_CONCEPTS_HPP
#define WEBPP_HEADER_CONCEPTS_HPP

namespace webpp::headers {

    template <typename T>
    concept header = requires (T h) {
        T{"Content-Type: string"};
        h.parse();
        h.is_valid();
    };

}

#endif // WEBPP_HEADER_CONCEPTS_HPP
