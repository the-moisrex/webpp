#ifndef WEBPP_STRING_VECTOR
#define WEBPP_STRING_VECTOR


#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/variant.hpp"


namespace webpp {


    /**
     * String Variant
     *
     * Holds string_views of the same string efficiantly. 
     */
    template <istl::StringView StrVT, StrVT... Names>
    struct basic_string_variant {};


    // string vector: same as above, but you can add to it


} // namespace webpp


#endif
