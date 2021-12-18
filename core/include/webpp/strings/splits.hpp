#ifndef WEBPP_STRING_VECTOR
#define WEBPP_STRING_VECTOR


#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/type_traits.hpp"
#include "./fixed_string.hpp"

namespace webpp::strings {


    /**
     * String Splits
     *
     * Holds string_views of the same string in a manner that's memory efficiant.
     */
    template <istl::basic_fixed_string... Names>
    struct string_splits {
        using char_type        = typename istl::first_parameter<stl::tuple<decltype(Names)...>>::value_type;
        using string_view_type = stl::basic_string_view<char_type>;
        using str_ptr          = char_type const*;
        static constexpr stl::size_t piece_count = sizeof...(Names);
        using tuple_type                         = istl::repeat_type<piece_count + 1, stl::tuple, str_ptr>;

        template <istl::basic_fixed_string the_name>
        constexpr static stl::size_t index_of = istl::index_of_item<the_name, Names...>::value;

      private:
        tuple_type data;

      public:
        constexpr string_splits() noexcept = default;
        constexpr string_splits(string_view_type str) noexcept : string_splits{str.data(), str.size()} {}

        constexpr string_splits(str_ptr ptr, stl::size_t len) noexcept
          : data{(
                   [last_pos = 0ul, data_str = string_view_type{ptr, len}](auto&& name) mutable -> str_ptr {
                       last_pos = data_str.find(name, last_pos);
                       return data_str.data() + last_pos;
                   }(Names),
                   ...),
                 ptr + len} {}

        template <stl::size_t Index, istl::StringView StrV = stl::string_view>
        constexpr StrV view() const noexcept {
            auto const start_ptr = stl::get<Index>(data);
            auto const end_ptr   = stl::get<Index + 1>(data);
            auto const len       = end_ptr - start_ptr;
            return StrV{start_ptr, len};
        }

        template <istl::basic_fixed_string Name, istl::StringView StrV = stl::string_view>
        constexpr StrV view_of() const noexcept {
            constexpr auto index = index_of<Name>;
            return view<index, StrV>();
        }
    };




    template <istl::CharType CharT>
    struct string_piece {};

    // string vector: same as above, but you can add to it
    template <istl::CharType CharT = char, Allocator AllocType = stl::allocator<CharT>>
    struct basic_string_vector : stl::vector<string_piece<CharT>, AllocType> {};

    using string_vector = basic_string_vector<>;


    // split strings with the specified delimiter
    template <typename StringVec = string_vector, istl::StringViewifiable StrV>
    StringVec split(StrV&& str, istl::CharType auto... delims) {}

    template <typename StringVec = string_vector,
              istl::StringViewifiable InpStrV,
              istl::StringViewifiable... StrV>
    StringVec split(InpStrV&& str, StrV&&... delims) {}


} // namespace webpp::strings


#endif
