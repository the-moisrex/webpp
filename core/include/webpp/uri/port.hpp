// Created by moisrex on 11/5/20.

#ifndef WEBPP_PORT_HPP
#define WEBPP_PORT_HPP

#include "../convert/casts.hpp"
#include "../std/string.hpp"
#include "../validators/validators.hpp"

#include <charconv>

namespace webpp::uri {


    /**
     * Basic port is designed as a string because it also should be able to handle services,
     * not that URIs can handle services but that operating systems APIs like Unix systems can
     * handle services instead of only port numbers.
     *
     * todo: implement handling of services at construction and to convert port number to a service
     */
    template <istl::String StringType = stl::string>
    struct basic_port : stl::remove_cvref_t<StringType> {
        using string_type = stl::remove_cvref_t<StringType>;

        template <typename ...T>
        constexpr basic_fragments(T&&...args) : string_type{stl::forward<T>(args)...} {
            // todo: make sure if it's a valid port number
            if (!is::digit(*this)) {
                // convert the service name to port number
            }
        }

        template <typename T>
        requires(stl::is_integral_v<stl::remove_cvref_t<T>>
                 && (sizeof(stl::remove_cvref_t<T>) > sizeof(char))
                 && !stl::is_floating_point_v<stl::remove_cvref_t<T>>)
        constexpr basic_port(T port_num) : string_type{} {
            if (port_num < 0 || port_num > 65535)
                throw stl::invalid_argument("The specified port number is not in a valid range.");

            append_to(*this, port_num, stl::chars_format::fixed);
        }

        void append_to(istl::String auto&out) {
            if (this->empty())
                return; // nothing to add
            out.reserve(out.size() + this->size() + 1);
            out += ':';
            out += *this;
        }

        [[nodiscard]] constexpr uint16_t value() const {
            return to_uint16(*this);
        }

    };


}

#endif // WEBPP_PORT_HPP
