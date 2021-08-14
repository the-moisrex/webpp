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
    template <istl::CharType CharT, auto... Names>
    struct basic_string_variant {
        using char_type                          = CharT;
        static constexpr stl::size_t field_count = sizeof...(Names);
        using tuple_type = istl::repeat_type<stl::tuple, char_type const*, field_count>;

      private:
        tuple_type data;

      public:
    };


    // string vector: same as above, but you can add to it


} // namespace webpp


#endif
