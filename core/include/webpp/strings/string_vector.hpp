#ifndef WEBPP_STRING_VECTOR
#define WEBPP_STRING_VECTOR


#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/type_traits.hpp"

namespace webpp {


    /**
     * String Splits
     *
     * Holds string_views of the same string efficiantly.
     */
    template <istl::CharType CharT, auto... Names>
    struct basic_string_splits {
        using char_type                          = CharT;
        using str_ptr                            = char_type const*;
        static constexpr stl::size_t piece_count = sizeof...(Names);
        using tuple_type                         = istl::repeat_type<stl::tuple, str_ptr, piece_count + 1>;

      private:
        tuple_type data;

      public:
        constexpr basic_string_splits() noexcept = default;
        constexpr basic_string_splits(str_ptr ptr, stl::size_t len) noexcept
          : data{ptr, (Names, ptr + len), ...} {}

        template <stl::size_t Index, istl::StringView StrV = stl::string_view>
        constexpr StrV view() const noexcept {
            auto const start_ptr = stl::get<Index>(data);
            auto const end_ptr   = stl::get<Index + 1>(data);
            auto const len       = end_ptr - start_ptr;
            return StrV{start_ptr, len};
        }
    };


    // string vector: same as above, but you can add to it


} // namespace webpp


#endif
