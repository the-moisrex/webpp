#ifndef WEBPP_STRINGS_CONCEPTS_HPP
#define WEBPP_STRINGS_CONCEPTS_HPP

#include "../std/string_view.hpp"

namespace webpp::strings {

    template <typename D>
    concept Delimiter = istl::StringViewifiable<D> || istl::CharType<D>;

} // namespace webpp::strings

#endif // WEBPP_STRINGS_CONCEPTS_HPP
