#ifndef WEBPP_STRING_VECTOR
#define WEBPP_STRING_VECTOR


#include "../common/meta.hpp"
#include "../std/algorithm.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "./size.hpp"

namespace webpp::strings {

    template <typename D>
    concept Delimiter = istl::StringViewifiable<D> || istl::CharType<D>;


    /**
     * String Splits (an array of string_views)
     *
     * Holds string_views of the same string in a manner that's memory efficiant.
    template <istl::StringView StrV, istl::basic_fixed_string... Names>
    struct string_splits : public stl::array<StrV, sizeof...(Names)> {
        using string_view_type                   = StrV;
        using char_type                          = istl::char_type_of<string_view_type>;
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

     */



    // string vector: same as above, but you can add to it
    /*
    template <istl::CharType CharT = char, Allocator AllocType = stl::allocator<CharT>>
    struct basic_string_vector : stl::vector<string_piece<CharT>, AllocType> {};

    using string_vector = basic_string_vector<>;
    */


    /**
     * The logic of finding the next piece will be placed here.
     *
     * I'd love to make this a coroutine!
     */
    template <typename T>
    struct splitter_iterator final {
        using splitter_type    = T;
        using splitter_ptr     = stl::add_pointer_t<splitter_type>;
        using string_view_type = typename splitter_type::string_view_type;
        using iterator         = splitter_iterator;

        // iterator traits
        using difference_type   = stl::size_t;
        using value_type        = string_view_type;
        using pointer           = stl::add_pointer_t<value_type>;
        using reference         = value_type;
        using iterator_category = stl::forward_iterator_tag;
        using iterator_concept  = stl::forward_iterator_tag;

        constexpr splitter_iterator() noexcept                         = default; // .end()
        constexpr splitter_iterator(splitter_iterator const&) noexcept = default; // .end()
        constexpr splitter_iterator(splitter_ptr    ptr,
                                    difference_type start_pos   = 0,
                                    difference_type finish_pos  = 0,
                                    stl::size_t     delim_index = 0) noexcept
          : spltr{ptr},
            start_pos{start_pos},
            finish_pos{finish_pos},
            delim_index{delim_index} {}

        iterator& operator++() {
            start_pos = finish_pos;
            spltr->on_delimiter(delim_index++, [this]<Delimiter DT>(DT&& delim) {
                if constexpr (istl::CharType<DT> || istl::StringView<DT>) {
                    finish_pos = spltr.str.find(delim, finish_pos);
                    finish_pos += ascii::size(delim);
                    // todo: add array support
                    // todo: add functor support
                } else {
                    static_assert_false(DT, "Unknown delimiter type");
                }
            });
            return *this;
        }

        iterator operator++(int) {
            iterator retval = *this;
            ++(*this);
            return retval;
        }

        [[nodiscard]] constexpr bool operator==(iterator other) const noexcept {
            return start_pos == other.start_pos && finish_pos == other.finish_pos && spltr == other.spltr &&
                   delim_index == other.delim_index;
        }

        [[nodiscard]] constexpr bool operator!=(iterator other) const noexcept {
            return !(*this == other);
        }

        value_type operator*() {
            // can't dereference an iterator that points to nothing
            assert(spltr != nullptr);
            return spltr->substr(start_pos, finish_pos);
        }


      private:
        splitter_ptr    spltr       = nullptr;
        difference_type start_pos   = 0;
        difference_type finish_pos  = 0;
        stl::size_t     delim_index = 0;
    };


    /**
     * String splitter struct.
     *
     * This class will help to split a string_splits
     */
    template <istl::StringView StrV = stl::string_view, Delimiter... DelimT>
    requires(sizeof...(DelimT) > 0) // we must have at least one delimiter
      struct basic_splitter {
        using string_view_type        = StrV;
        using delimiter_type          = stl::tuple<DelimT...>;
        using self_type               = basic_splitter;
        using iterator_type           = splitter_iterator<self_type>;
        using default_collection_type = stl::vector<string_view_type>;

        friend iterator_type;

      private:
        string_view_type str;
        delimiter_type   delims; // todo: add whitespaces as default delimiters

      public:
        constexpr basic_splitter(string_view_type str, DelimT&&... delims_input) noexcept
          : str{str},
            delims{stl::forward<DelimT>(delims_input)...} {}

        iterator_type begin() noexcept {
            return {this};
        }

        [[nodiscard]] constexpr iterator_type end() const noexcept {
            return {};
        }

        template <typename Vec = default_collection_type>
        Vec& split(Vec& vec) {
            stl::copy(this->begin(), this->end(), stl::back_inserter(vec));
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

        template <stl::size_t Index>
        [[nodiscard]] constexpr auto delimiter() const noexcept {
            constexpr stl::size_t delim_index = delimiter_clamp(Index);
            const auto            delim       = stl::get<delim_index>(delims);
            return delim;
        }

        [[nodiscard]] static constexpr auto delimiter_clamp(stl::size_t index) noexcept {
            return stl::clamp(index, 0ul, sizeof...(DelimT) - 1);
        }

        template <typename FuncT>
        constexpr void on_delimiter(stl::size_t index, FuncT&& functor) {
            auto const dindex = delimiter_clamp(index);
            istl::for_index(dindex, delims, stl::forward<FuncT>(functor));
        }


        [[nodiscard]] constexpr string_view_type substr(stl::size_t start_pos, stl::size_t finish_pos) {
            return str.substr(start_pos, finish_pos);
        }

        // todo: add a way to use coroutines here as another way of doing the same thing
    };


    template <Delimiter... DelimT>
    requires(sizeof...(DelimT) > 0) // we must have at least one delimiter
      struct splitter : basic_splitter<stl::string_view, DelimT...> {
        using basic_splitter<stl::string_view, DelimT...>::basic_splitter;
    };


    template <typename T, Delimiter... DelimT>
    splitter(T&&, DelimT&&...) -> splitter<DelimT...>;

    /*
    // split strings with the specified delimiter
    template <typename StringVec = string_vector, istl::StringViewifiable StrV>
    StringVec split(StrV&& str, istl::CharType auto... delims) {}

    template <typename StringVec = string_vector,
              istl::StringViewifiable InpStrV,
              istl::StringViewifiable... StrV>
    StringVec split(InpStrV&& str, StrV&&... delims) {}
    */

} // namespace webpp::strings


#endif
