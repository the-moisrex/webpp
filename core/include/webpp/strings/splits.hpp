#ifndef WEBPP_STRING_VECTOR
#define WEBPP_STRING_VECTOR


#include "../std/algorithm.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/type_traits.hpp"
#include "./fixed_string.hpp"
#include "./size.hpp"

namespace webpp::strings {

    template <typename D>
    concept Delimiter = istl::StringViewifiable<D> || istl::CharType<D>;


    /**
     * String Splits (an array of string_views
     *
     * Holds string_views of the same string in a manner that's memory efficiant.
     */
    template <istl::StringView StrV, istl::basic_fixed_string... Names>
    struct string_splits : public stl::array<StrV, sizeof...(Names)> {
        using string_view_type                   = StrV;
        using char_type                          = istl::char_type<string_view_type>;
        static constexpr stl::size_t piece_count = sizeof...(Names);
        using data_type                          = stl::array<string_view_type, piece_count>;

        template <istl::basic_fixed_string the_name>
        constexpr static stl::size_t index_of = istl::index_of_item<the_name, Names...>::value;

        static constexpr char_type default_delimiter = ' ';

      private:
        data_type data;

      public:
        constexpr string_splits() noexcept = default;
        constexpr string_splits(string_view_type str) noexcept : string_splits{str.data(), str.size()} {}

        template <Delimiter... DelimT>
        constexpr string_splits(string_view_type str, DelimT&&... delims) noexcept
          : string_splits{str.data(), str.size(), stl::forward<DelimT>(delims)...} {}

        constexpr string_splits(str_ptr ptr, stl::size_t len) : string_splits{ptr, len, default_delimiter} {}

        template <Delimiter... DelimT>
        constexpr string_splits(str_ptr ptr, stl::size_t len, DelimT&&... delims) {
            auto pos_finder = [delimiters = stl::make_tuple(stl::forward<DelimT>(delims)...),
                               last_pos   = 0ul,
                               data_str   = string_view_type{ptr, len}]<stl::size_t Index>(
                                istl::value_holder<Index>) mutable -> str_ptr {
                constexpr stl::size_t delim_index = stl::clamp(Index, 0ul, sizeof...(delims) - 1);
                const auto            delim       = stl::get<delim_index>(delimiters);
                last_pos                          = data_str.find(delim, last_pos);
                last_pos += ascii::size(delim);
                return data_str.data() + last_pos;
            };
            data[0] = ptr; // first element is the start of the string
            ([&]<stl::size_t... I>(stl::index_sequence<I...>) {
                ((data[I + 1] = pos_finder(istl::value_holder<I>{})), ...); // call the func
            })(stl::make_index_sequence<piece_count - 1>());
            data.back() = ptr + len; // last element is the end of the string
        }

        template <stl::size_t Index>
        requires(Index + 1 < piece_count) // Index should be in bounds of data array
          constexpr StrV view() const noexcept {
            return stl::get<Index>(data);
        }

        template <istl::basic_fixed_string Name, istl::StringView StrV = stl::string_view>
        constexpr StrV view_of() const noexcept {
            constexpr auto index = index_of<Name>;
            return view<index, StrV>();
        }
    };




    // string vector: same as above, but you can add to it
    template <istl::CharType CharT = char, Allocator AllocType = stl::allocator<CharT>>
    struct basic_string_vector : stl::vector<string_piece<CharT>, AllocType> {};

    using string_vector = basic_string_vector<>;


    template <typename T>
    struct splitter_iterator final {
        using splitter_type    = T;
        using string_view_type = typename splitter_type::string_view_type;
    };

    /**
     * String splitter struct.
     *
     * This class will help to split a string_splits
     */
    template <istl::StringView StrV = stl::string_view, Delimiter... DelimT>
    struct basic_splitter final {
        using string_view_type        = StrV;
        using delimiter_type          = stl::tuple<DelimT...>;
        using self_type               = basic_splitter;
        using iterator_type           = splitter_iterator<self_type>;
        using default_collection_type = stl::vector<string_view_type>;

      private:
        string_view_type str;
        delimiter_type   delims;

      public:
        constexpr basic_splitter(string_view_type str, DelimT&&... delims_input) noexcept
          : str{str},
            delims{stl::forward<DelimT>(delims_input)...} {}

        iterator_type begin() noexcept {}

        iterator_type end() noexcept {}

        template <typename Vec = default_collection_type>
        Vec& split(Vec& vec) {
            return vec;
        }

        /**
         * Split the strings and get a vector
         */
        template <typename Vec = default_collection_type, typename... Args>
        Vec split(Args&&... args) {
            Vec vec{stl::forward<Args>(args)...};
            split<Vec>(vec);
            return vec;
        }
    };


    // split strings with the specified delimiter
    template <typename StringVec = string_vector, istl::StringViewifiable StrV>
    StringVec split(StrV&& str, istl::CharType auto... delims) {}

    template <typename StringVec = string_vector,
              istl::StringViewifiable InpStrV,
              istl::StringViewifiable... StrV>
    StringVec split(InpStrV&& str, StrV&&... delims) {}


} // namespace webpp::strings


#endif
